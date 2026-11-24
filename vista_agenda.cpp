// Pantalla de la agenda optima (algoritmo voraz de seleccion de actividades).

#include <algorithm>
#include <cmath>

#include "app.h"
#include "greedy.h"
#include "persistencia.h"
#include "raylib.h"
#include "ui.h"

using namespace ui;

namespace {
const float M = 36;
}

void App::generarAgenda() {
    agSel.clear();
    agCruza.clear();
    if (!ordenadas || actividades.empty()) return;

    // La agenda es la que devuelve seleccionActividades() (greedy.cpp, sin cambios).
    ultimaAgenda = seleccionActividades(actividades);

    // Solo para la vista: marcar cuales se eligieron y con cual se cruza cada descartada,
    // aplicando la misma regla del greedy (inicio >= fin de la ultima elegida).
    int n = (int)actividades.size();
    agSel.assign(n, 0);
    agCruza.assign(n, -1);
    int ultima = -1;
    for (int i = 0; i < n; i++) {
        if (ultima < 0 || actividades[i].inicio >= actividades[ultima].fin) { agSel[i] = 1; ultima = i; }
        else agCruza[i] = ultima;
    }
}

void App::guardarAgendaActual() {
    if (ultimaAgenda.empty()) { aviso("Primero genera una agenda óptima.", true); return; }
    if (guardarAgenda("agenda_optima.txt", ultimaAgenda)) aviso("Agenda guardada en agenda_optima.txt");
    else aviso("No se pudo escribir agenda_optima.txt.", true);
}

void App::pantallaAgenda() {
    encabezado("Agenda óptima", "Algoritmo voraz: siempre la que termina primero");
    if (IsKeyPressed(KEY_ESCAPE)) irA(Pantalla::Menu);

    // ---------- requisito: actividades ordenadas por hora de fin ----------
    if (!ordenadas || actividades.empty()) {
        float w = std::min(560.0f, W - 2 * M), h = 250;
        Rectangle c = {(W - w) / 2, std::max(110.0f, (H - h) / 2 - 10), w, h};
        tarjeta(c, 14, SUPERFICIE, LINEA);
        if (actividades.empty()) {
            texto("No hay actividades cargadas", c.x + 32, c.y + 30, 22, TEXTO, true);
            parrafo("Carga el archivo o agrega algunas, y luego ordénalas por hora de fin.", {c.x + 32, c.y + 74, c.width - 64, 60}, 16, TEXTO_SUAVE);
            if (boton({c.x + 32, c.y + 170, 200, 48}, "Ir a Actividades", Boton::Primario)) irA(Pantalla::Actividades);
        } else {
            texto("Primero ordena las actividades", c.x + 32, c.y + 30, 22, TEXTO, true);
            parrafo("El algoritmo voraz necesita las actividades ordenadas por hora de fin. Ordénalas con Merge Sort o con Quick Sort.",
                    {c.x + 32, c.y + 74, c.width - 64, 80}, 16, TEXTO_SUAVE);
            if (boton({c.x + 32, c.y + 170, 160, 48}, "Merge Sort", Boton::Primario)) irAOrdenar(Algoritmo::Merge);
            if (boton({c.x + 204, c.y + 170, 160, 48}, "Quick Sort", Boton::Secundario)) irAOrdenar(Algoritmo::Quick);
        }
        return;
    }

    // ---------- resumen ----------
    int n = (int)actividades.size(), k = (int)ultimaAgenda.size();
    texto(std::to_string(k) + " de " + std::to_string(n), M, 84, 36, TEXTO, true);
    float rw = anchoTexto(std::to_string(k) + " de " + std::to_string(n), 36, true);
    texto(n == 1 ? "actividad" : "actividades, sin que se crucen.", M + rw + 14, 100, 16, TEXTO_SUAVE);

    float top = 150, gap = 20;
    float altoCont = H - top - M;
    float wIzq = std::floor((W - 2 * M - gap) * 0.64f);
    Rectangle linea = {M, top, wIzq, altoCont};
    Rectangle lista = {M + wIzq + gap, top, W - 2 * M - wIzq - gap, altoCont};

    // ---------- linea de tiempo ----------
    tarjeta(linea, 14, SUPERFICIE, LINEA);
    int t0 = actividades[0].inicio, t1 = actividades[0].fin;
    for (const auto& a : actividades) { t0 = std::min(t0, a.inicio); t1 = std::max(t1, a.fin); }
    if (t1 <= t0) t1 = t0 + 1;

    float etiquetaW = 150, pad = 22;
    float bx = linea.x + pad + etiquetaW, bw = linea.width - 2 * pad - etiquetaW;
    float paso = std::max(1.0f, std::ceil((t1 - t0) / 12.0f));
    auto xTiempo = [&](float t) { return bx + (t - t0) / (float)(t1 - t0) * bw; };

    float yEje = linea.y + 18;
    Rectangle zona = {linea.x + 1, linea.y + 44, linea.width - 2, linea.height - 44 - 14};
    float rh = std::clamp((zona.height - 4) / n, 20.0f, 32.0f);
    float contenido = n * rh;
    float maxScroll = std::max(0.0f, contenido - zona.height);
    if (sobre(zona)) scrollAgenda -= GetMouseWheelMove() * rh * 2;
    scrollAgenda = std::clamp(scrollAgenda, 0.0f, maxScroll);

    for (float t = t0; t <= t1 + 0.001f; t += paso) {
        float xx = xTiempo(t);
        textoCentro(std::to_string((int)t), xx, yEje, 12, TEXTO_SUAVE);
        DrawLine((int)xx, (int)(linea.y + 42), (int)xx, (int)(linea.y + linea.height - 14), {236, 240, 245, 255});
    }

    int hovered = -1;
    BeginScissorMode((int)zona.x, (int)zona.y, (int)zona.width, (int)zona.height);
    for (int i = 0; i < n; i++) {
        float yy = zona.y + 2 + i * rh - scrollAgenda;
        if (yy + rh < zona.y || yy > zona.y + zona.height) continue;
        const Actividad& a = actividades[i];
        bool sel = agSel[i] != 0;
        Rectangle fila = {zona.x, yy, zona.width, rh};
        if (sobre(fila) && sobre(zona)) { hovered = i; rectRedondo({fila.x + 8, fila.y, fila.width - 16, fila.height}, 6, {246, 248, 251, 255}); }
        texto(ajustar(a.nombre, etiquetaW - 14, 13, sel), linea.x + pad, yy + (rh - 19) / 2, 13, sel ? TEXTO : TEXTO_SUAVE, sel);
        float x1 = xTiempo((float)a.inicio), x2 = xTiempo((float)a.fin);
        rectRedondo({x1, yy + 4, std::max(6.0f, x2 - x1), rh - 8}, std::min(6.0f, (rh - 8) / 2), sel ? ACENTO : BARRA_TENUE);
    }
    EndScissorMode();
    if (maxScroll > 0) {
        float th = std::max(30.0f, zona.height * zona.height / contenido);
        float ty = zona.y + (zona.height - th) * (scrollAgenda / maxScroll);
        rectRedondo({linea.x + linea.width - 10, ty, 4, th}, 2, {196, 206, 220, 255});
    }
    if (hovered >= 0) {
        const Actividad& a = actividades[hovered];
        std::vector<std::string> l = {a.nombre, "Inicio " + std::to_string(a.inicio) + ", fin " + std::to_string(a.fin)};
        if (agSel[hovered]) l.push_back("Elegida: no se cruza con las anteriores.");
        else l.push_back("Descartada: se cruza con «" + actividades[agCruza[hovered]].nombre + "».");
        tooltip(l);
    }

    // ---------- lista de la agenda ----------
    tarjeta(lista, 14, SUPERFICIE, LINEA);
    float lx = lista.x + 24, lw = lista.width - 48;
    texto("Tu agenda", lx, lista.y + 20, 18, TEXTO, true);
    float yl = lista.y + 62, bajoL = lista.y + lista.height - 24 - 48 - 16;
    int visibles = std::max(0, (int)((bajoL - yl) / 44));
    for (int i = 0; i < k && i < visibles; i++) {
        const Actividad& a = ultimaAgenda[i];
        float yy = yl + i * 44;
        Rectangle chip = {lx, yy + 4, 74, 30};
        rectRedondo(chip, 8, ACENTO_TENUE);
        textoCentro(std::to_string(a.inicio) + " - " + std::to_string(a.fin), chip.x + chip.width / 2, chip.y + 4, 14, ACENTO, true);
        texto(ajustar(a.nombre, lw - 90, 15), lx + 88, yy + 8, 15, TEXTO);
    }
    if (k > visibles) texto("y " + std::to_string(k - visibles) + " más. Se guardan todas.", lx, bajoL - 4, 13, TEXTO_SUAVE);

    if (boton({lx, lista.y + lista.height - 24 - 48, lw, 48}, "Guardar en agenda_optima.txt", Boton::Primario)) guardarAgendaActual();
}
