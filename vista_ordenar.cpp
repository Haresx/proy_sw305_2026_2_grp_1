// Pantalla de Merge Sort / Quick Sort: animacion + tiempo real.
//
// IMPORTANTE: el tiempo que se muestra sale de medicion.cpp (mergeSort/quickSort
// reales, sin instrumentar). La animacion solo reproduce una traza grabada aparte
// (pasos_orden.cpp), asi que animar mas lento, pausar o saltar no cambia el tiempo.

#include <algorithm>
#include <cmath>

#include "app.h"
#include "merge_sort.h"
#include "quick_sort.h"
#include "raylib.h"
#include "ui.h"

using namespace ui;

namespace {
const float M = 36;
const double VEL[] = {0.25, 0.5, 1, 2, 4, 8};
const int NUM_VEL = 6;
const size_t MAX_ANIMADAS = 500;      // con mas actividades se ordena sin animar
const Color ESCRITO = {111, 143, 232, 255};
const Color MENORES = {43, 166, 160, 255};
const Color MAYORES = {226, 170, 128, 255};

std::string textoVel(double v) {
    if (v == 0.25) return "0.25x";
    if (v == 0.5) return "0.5x";
    return std::to_string((int)v) + "x";
}
}  // namespace

// ---------------------------------------------------------------------------
// Estado y acciones
// ---------------------------------------------------------------------------
void App::reiniciarOrdenar() {
    anim.preparar(actividades, alg);
    fase = Fase::Inactiva;
    hayMed = false;
    acum = 0;
}

void App::iniciarOrdenamiento(bool saltar) {
    if (actividades.empty()) { aviso("No hay actividades cargadas.", true); return; }
    int n = (int)actividades.size();
    bool animable = n >= 2 && actividades.size() <= MAX_ANIMADAS;

    // 1) TIEMPO REAL: el algoritmo original, medido sobre una copia
    med = medirOrdenamiento(alg, actividades);
    hayMed = true;

    // 2) Traza para la animacion (instrumentada, aparte)
    Traza t;
    if (animable) t = (alg == Algoritmo::Merge) ? trazaMergeSort(actividades) : trazaQuickSort(actividades);
    anim.iniciar(actividades, alg, std::move(t));

    // 3) El ordenamiento de verdad sobre los datos del programa (como el main original)
    if (n >= 2) {
        if (alg == Algoritmo::Merge) mergeSort(actividades, 0, n - 1);
        else quickSort(actividades, 0, n - 1);
    }
    ordenadas = true;
    ultimaAgenda.clear();

    acum = 0;
    tasaBase = std::clamp((double)anim.totalPasos() / 6.0, 8.0, 3000.0);   // ~6 s a velocidad 1x

    if (!animable) {
        anim.preparar(actividades, alg);     // sin traza: se muestra directamente el resultado
        anim.finalizar();
        fase = Fase::Terminada;
        if (n > (int)MAX_ANIMADAS) aviso("Son más de 500 actividades: se ordenó sin animar.");
    } else if (saltar) {
        anim.saltarAlFinal();
        fase = Fase::Terminada;
    } else {
        fase = Fase::Reproduciendo;
    }
}

void App::accionPrincipal() {
    switch (fase) {
        case Fase::Inactiva:      iniciarOrdenamiento(false); break;
        case Fase::Reproduciendo: fase = Fase::Pausada; break;
        case Fase::Pausada:       fase = Fase::Reproduciendo; break;
        case Fase::Terminada:     repetirAnimacion(); break;
    }
}

void App::saltarAnimacion() {
    if (fase == Fase::Inactiva) iniciarOrdenamiento(true);
    else if (fase == Fase::Reproduciendo || fase == Fase::Pausada) {
        anim.saltarAlFinal();
        fase = Fase::Terminada;
    }
}

void App::repetirAnimacion() {
    if (!anim.hayTraza()) return;
    anim.reiniciar();
    acum = 0;
    fase = Fase::Reproduciendo;
}

void App::mezclar() {
    if (actividades.size() < 2) return;
    std::shuffle(actividades.begin(), actividades.end(), rng);
    ordenadas = false;
    ultimaAgenda.clear();
    reiniciarOrdenar();
}

std::string App::descripcionPaso() const {
    std::string base;
    if (fase == Fase::Inactiva) {
        return ordenadas ? "Ya están ordenadas por hora de fin. Puedes ordenarlas de nuevo o usar Mezclar."
                         : "Lista para ordenar " + std::to_string(anim.datos.size()) + " actividades por hora de fin.";
    }
    if (fase == Fase::Terminada) return "Ordenadas por hora de fin.";

    if (anim.alg == Algoritmo::Merge) {
        if (anim.mezclando)
            base = "Mezclando las posiciones " + std::to_string(anim.rIzq + 1) + " a " + std::to_string(anim.rDer + 1) +
                   ": siempre se toma el menor.";
        else base = "Dividiendo en mitades y preparando la siguiente mezcla.";
    } else {
        if (anim.pivote >= 0)
            base = "Partición de las posiciones " + std::to_string(anim.rIzq + 1) + " a " + std::to_string(anim.rDer + 1) +
                   ", pivote con fin " + std::to_string(anim.datos[anim.ids[anim.pivote]].fin) + ".";
        else base = "Eligiendo el siguiente pivote.";
    }
    return fase == Fase::Pausada ? "En pausa. " + base : base;
}

// ---------------------------------------------------------------------------
// Pantalla
// ---------------------------------------------------------------------------
void App::pantallaOrdenar() {
    const bool merge = alg == Algoritmo::Merge;
    encabezado(merge ? "Merge Sort" : "Quick Sort",
               merge ? "Siempre O(n log n)" : "O(n log n) en promedio, O(n²) en el peor caso");

    // ---------- entrada y avance de la animacion ----------
    if (IsKeyPressed(KEY_ESCAPE)) irA(Pantalla::Menu);
    if (!actividades.empty()) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) accionPrincipal();
        if (IsKeyPressed(KEY_S)) saltarAnimacion();
    }
    if (fase == Fase::Reproduciendo) {
        acum += std::min((double)GetFrameTime(), 0.1) * tasaBase * VEL[idxVel];
        size_t pasos = (size_t)acum;
        acum -= pasos;
        anim.avanzar(pasos);
        if (anim.terminado()) fase = Fase::Terminada;
    }

    // ---------- geometria ----------
    const float top = 92, panelW = 290, gap = 20, ctrlH = 48;
    float ctrlY = H - 28 - ctrlH;
    Rectangle lienzo = {M, top, W - 2 * M - panelW - gap, ctrlY - 18 - top};
    Rectangle panel = {lienzo.x + lienzo.width + gap, top, panelW, lienzo.height};

    dibujarLienzo(lienzo);
    dibujarPanelTiempo(panel);

    // ---------- controles ----------
    bool hayDatos = !actividades.empty();
    std::string etiqueta = fase == Fase::Inactiva ? "Ordenar"
                         : fase == Fase::Reproduciendo ? "Pausar"
                         : fase == Fase::Pausada ? "Continuar" : "Repetir animación";
    bool puedePrincipal = hayDatos && !(fase == Fase::Terminada && !anim.hayTraza());
    float x = M;
    if (boton({x, ctrlY, 190, ctrlH}, etiqueta, Boton::Primario, puedePrincipal)) accionPrincipal();
    x += 190 + 12;
    bool puedeSaltar = hayDatos && fase != Fase::Terminada;
    if (boton({x, ctrlY, 210, ctrlH}, "Saltar animación (S)", Boton::Secundario, puedeSaltar)) saltarAnimacion();
    x += 210 + 12;
    bool puedeMezclar = actividades.size() >= 2 && fase != Fase::Reproduciendo;
    if (boton({x, ctrlY, 120, ctrlH}, "Mezclar", Boton::Secundario, puedeMezclar)) mezclar();

    // velocidad
    float vx = W - M - 40;
    if (boton({vx, ctrlY + 4, 40, 40}, "+", Boton::Secundario, idxVel < NUM_VEL - 1)) idxVel++;
    vx -= 66;
    textoCentro(textoVel(VEL[idxVel]), vx + 33, ctrlY + 12, 18, TEXTO, true);
    vx -= 40;
    if (boton({vx, ctrlY + 4, 40, 40}, "-", Boton::Secundario, idxVel > 0)) idxVel--;
    textoDer("Velocidad", vx - 14, ctrlY + 14, 14, TEXTO_SUAVE);
}

void App::dibujarLienzo(Rectangle cv) {
    tarjeta(cv, 14, SUPERFICIE, LINEA);
    const float pad = 24;
    const bool merge = alg == Algoritmo::Merge;
    const int n = (int)anim.ids.size();

    texto(ajustar(descripcionPaso(), cv.width - 2 * pad, 15), cv.x + pad, cv.y + 18, 15, TEXTO_SUAVE);

    // ---------- estado vacio ----------
    if (n == 0) {
        textoCentro("No hay actividades para ordenar", cv.x + cv.width / 2, cv.y + cv.height / 2 - 50, 20, TEXTO, true);
        textoCentro("Carga el archivo o agrega algunas primero.", cv.x + cv.width / 2, cv.y + cv.height / 2 - 18, 15, TEXTO_SUAVE);
        if (boton({cv.x + cv.width / 2 - 100, cv.y + cv.height / 2 + 20, 200, 44}, "Ir a Actividades", Boton::Primario)) irA(Pantalla::Actividades);
        return;
    }

    // ---------- leyenda (se arma primero para saber cuanto espacio ocupa) ----------
    struct Item { Color c; const char* t; bool franja; };
    std::vector<Item> leyenda;
    if (merge) leyenda = {{AMBAR, "Comparando", false}, {ACENTO, "Escrito", false}, {AUX_IZQ, "Mitad izquierda", false},
                          {AUX_DER, "Mitad derecha", false}, {BARRA_MEZCLADA, "Ya mezclado", false}, {VERDE, "Ordenado", false}};
    else       leyenda = {{ROJO, "Pivote", false}, {AMBAR, "Comparando", false}, {ACENTO, "Intercambio", false},
                          {VERDE, "Ya en su lugar", false}, {MENORES, "Menores o iguales al pivote", true}, {MAYORES, "Mayores", true}};
    int filasLey = 1;
    {
        float lx = 0;
        for (const auto& it : leyenda) {
            float w = anchoTexto(it.t, 13) + 24;
            if (lx + w > cv.width - 2 * pad && lx > 0) { lx = 0; filasLey++; }
            lx += w + 10;
        }
    }
    float legTop = cv.y + cv.height - 18 - filasLey * 20;

    // ---------- carriles ----------
    float lanesTop = cv.y + 62;
    float lanesBot = legTop - 14;
    float lanesH = lanesBot - lanesTop;
    float auxTop = lanesTop, auxH = 0, mainTop = lanesTop, mainH = lanesH;
    if (merge) {
        auxH = std::floor(lanesH * 0.34f);
        mainTop = lanesTop + auxH + 14;
        mainH = lanesBot - mainTop;
    }
    const float zonaH = 12;                              // franja bajo la linea de base (Quick)
    float base = mainTop + mainH - zonaH;
    float maxBar = std::max(20.0f, mainH - zonaH - 22);

    // ---------- columnas ----------
    float areaW = cv.width - 2 * pad;
    float slot = std::min(areaW / n, 72.0f);
    float x0 = cv.x + pad + (areaW - slot * n) / 2;
    float gapB = slot >= 8 ? std::clamp(slot * 0.2f, 2.0f, 8.0f) : (slot >= 3 ? 1.0f : 0.0f);
    auto colX = [&](int i) { return (int)std::round(x0 + i * slot + gapB / 2); };
    auto colW = [&](int i) { return std::max(1, (int)std::round(x0 + (i + 1) * slot - gapB / 2) - colX(i)); };
    bool conEtiqueta = slot >= 26;

    int minF = anim.datos[0].fin, maxF = anim.datos[0].fin;
    for (const auto& a : anim.datos) { minF = std::min(minF, a.fin); maxF = std::max(maxF, a.fin); }
    auto frac = [&](int fin) { return maxF > minF ? 0.14f + 0.86f * (fin - minF) / (float)(maxF - minF) : 0.6f; };

    // ---------- fondo del rango activo ----------
    if (anim.hayRango && !anim.completo && anim.rDer >= anim.rIzq) {
        int a = colX(anim.rIzq), b = colX(anim.rDer) + colW(anim.rDer);
        float yTop = merge ? auxTop - 4 : mainTop - 4;
        rectRedondo({(float)a - 6, yTop, (float)(b - a) + 12, base + zonaH + 6 - yTop}, 8, {238, 243, 253, 255});
    }

    // ---------- carril auxiliar (Merge) ----------
    if (merge) {
        float auxBase = auxTop + auxH - 4;
        if (!anim.completo) texto("Arreglo auxiliar", cv.x + pad, auxTop - 2, 13, TEXTO_SUAVE);
        if (anim.mezclando && !anim.aux.empty()) {
            int n1 = anim.mMed - anim.rIzq + 1;
            for (size_t k = 0; k < anim.aux.size(); k++) {
                int pos = anim.rIzq + (int)k;
                int fin = anim.datos[anim.aux[k]].fin;
                float h = std::max(6.0f, frac(fin) * (auxH - 34));
                int bx = colX(pos), bw = colW(pos);
                bool cmp = (int)k == anim.auxI || (int)k == anim.auxJ;
                Color c = cmp ? AMBAR : ((int)k < n1 ? AUX_IZQ : AUX_DER);
                if (anim.usado[k]) barraFantasma(bx, auxBase, bw, h, {200, 210, 226, 255});
                else barra(bx, auxBase, bw, h, c);
                if (conEtiqueta && !anim.usado[k]) textoCentro(std::to_string(fin), bx + bw / 2.0f, auxBase - h - 22, 13, TEXTO_SUAVE);
            }
        } else if (!anim.completo) {
            textoCentro("Aquí se copian las dos mitades antes de mezclarlas", cv.x + cv.width / 2, auxTop + auxH / 2 - 12, 14, {176, 188, 204, 255});
        }
        DrawLine((int)(cv.x + pad), (int)(mainTop - 8), (int)(cv.x + cv.width - pad), (int)(mainTop - 8), {236, 240, 245, 255});
    }

    // ---------- barras principales ----------
    for (int i = 0; i < n; i++) {
        int fin = anim.actividadEn(i).fin;
        float h = std::max(6.0f, frac(fin) * maxBar);
        int bx = colX(i), bw = colW(i);
        bool enRango = anim.hayRango && i >= anim.rIzq && i <= anim.rDer;

        if (anim.completo) { barra(bx, base, bw, h, VERDE); }
        else if (!merge) {
            Color c = BARRA;
            if (i == anim.pivote) c = ROJO;
            else if (anim.marcado[i]) c = VERDE;
            else if (i == anim.comparaA) c = AMBAR;
            else if (i == anim.movA || i == anim.movB) c = ACENTO;
            else if (anim.hayRango && !enRango) c = BARRA_TENUE;
            barra(bx, base, bw, h, c);
        } else {
            if (anim.mezclando && enRango) {
                if (i == anim.escritoK) barra(bx, base, bw, h, ACENTO);
                else if (i <= anim.escritosHasta) barra(bx, base, bw, h, ESCRITO);
                else barraFantasma(bx, base, bw, h, {200, 210, 226, 255});
            } else barra(bx, base, bw, h, anim.marcado[i] ? BARRA_MEZCLADA : BARRA);
        }
        if (conEtiqueta) textoCentro(std::to_string(fin), bx + bw / 2.0f, base - h - 22, 13, TEXTO_SUAVE);
    }

    // ---------- guias de la particion (Quick) ----------
    if (!merge && !anim.completo && anim.pivote >= 0) {
        int pf = anim.actividadEn(anim.pivote).fin;
        float yp = base - std::max(6.0f, frac(pf) * maxBar);
        int a = colX(anim.rIzq), b = colX(anim.rDer) + colW(anim.rDer);
        for (int px = a; px < b; px += 10) DrawRectangle(px, (int)yp, 5, 2, ROJO);   // linea del pivote
        if (anim.frontera >= anim.rIzq) {
            int u0 = colX(anim.rIzq), u1 = colX(anim.frontera) + colW(anim.frontera);
            DrawRectangle(u0, (int)base + 5, u1 - u0, 4, MENORES);
        }
        if (anim.jActual >= 0 && anim.frontera + 1 <= anim.jActual - 1) {
            int u0 = colX(anim.frontera + 1), u1 = colX(anim.jActual - 1) + colW(anim.jActual - 1);
            DrawRectangle(u0, (int)base + 5, u1 - u0, 4, MAYORES);
        }
    }

    // ---------- leyenda ----------
    {
        float lx = cv.x + pad, ly = legTop;
        for (const auto& it : leyenda) {
            float w = anchoTexto(it.t, 13) + 24;
            if (lx + w > cv.x + cv.width - pad) { lx = cv.x + pad; ly += 20; }
            if (it.franja) DrawRectangle((int)lx, (int)ly + 7, 12, 4, it.c);
            else rectRedondo({lx, ly + 3, 12, 12}, 3, it.c);
            texto(it.t, lx + 18, ly - 1, 13, TEXTO_SUAVE);
            lx += w + 10;
        }
    }

    // ---------- cuadro al pasar el mouse ----------
    Vector2 m = GetMousePosition();
    if (m.y >= mainTop && m.y <= base + zonaH && m.x >= x0 && m.x < x0 + slot * n) {
        int i = (int)((m.x - x0) / slot);
        if (i >= 0 && i < n) {
            const Actividad& a = anim.actividadEn(i);
            tooltip({a.nombre, "Inicio " + std::to_string(a.inicio) + ", fin " + std::to_string(a.fin)});
        }
    }
    // al terminar, atajo hacia el siguiente paso
    if (fase == Fase::Terminada)
        if (boton({cv.x + cv.width - pad - 190, cv.y + 8, 190, 36}, "Ver agenda óptima", Boton::Texto)) irA(Pantalla::Agenda);
}

void App::dibujarPanelTiempo(Rectangle p) {
    tarjeta(p, 14, SUPERFICIE, LINEA);
    const float pad = 22;
    float x = p.x + pad, w = p.width - 2 * pad, y = p.y + 20;

    texto("Tiempo real", x, y, 14, TEXTO_SUAVE);
    std::string t = hayMed ? formatearTiempo(med.ns) : "—";
    int tam = 56;
    while (tam > 26 && anchoTexto(t, tam, true) > w) tam -= (tam > 36 ? 20 : 10);
    texto(t, x, y + 22 + (56 - tam) * 0.3f, tam, hayMed ? TEXTO : Color{176, 188, 204, 255}, true);

    std::string cap = !hayMed ? "Se mide al pulsar Ordenar."
        : med.corridas <= 1 ? "Una sola ejecución del algoritmo real."
                            : "Mediana de " + miles(med.corridas) + " ejecuciones del algoritmo real.";
    float hy = parrafo(cap, {x, y + 100, w, 40}, 13, TEXTO_SUAVE);

    float yd = y + 100 + hy + 10;
    DrawLine((int)x, (int)yd, (int)(x + w), (int)yd, LINEA);

    // contadores
    const bool merge = alg == Algoritmo::Merge;
    float yc = yd + 14;
    bool sinConteo = anim.completo && !anim.hayTraza();      // se ordeno sin animar: no hay conteo
    texto("Comparaciones", x, yc, 14, TEXTO_SUAVE);
    textoDer(sinConteo ? "—" : miles(anim.comps), x + w, yc - 3, 20, TEXTO, true);
    texto(merge ? "Escrituras" : "Intercambios", x, yc + 34, 14, TEXTO_SUAVE);
    textoDer(sinConteo ? "—" : miles(anim.movs), x + w, yc + 31, 20, TEXTO, true);

    float yp = yc + 80;
    DrawLine((int)x, (int)yp, (int)(x + w), (int)yp, LINEA);

    // progreso
    size_t total = anim.totalPasos();
    float yg = yp + 14;
    std::string prog = total == 0 ? "Sin animación" : "Paso " + miles((long long)anim.sig) + " de " + miles((long long)total);
    texto(prog, x, yg, 14, TEXTO_SUAVE);
    rectRedondo({x, yg + 30, w, 6}, 3, {232, 237, 244, 255});
    float f = total == 0 ? (anim.completo ? 1.0f : 0.0f) : (float)anim.sig / (float)total;
    if (f > 0) rectRedondo({x, yg + 30, std::max(6.0f, w * f), 6}, 3, ACENTO);

    // nota al pie
    float yn = p.y + p.height - 20 - 58;
    parrafo("La animación va más lenta a propósito. El tiempo real no depende de ella.", {x, yn, w, 58}, 13, TEXTO_SUAVE);
}
