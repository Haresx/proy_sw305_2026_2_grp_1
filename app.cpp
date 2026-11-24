#include "app.h"

#include <algorithm>
#include <cmath>

#include "greedy.h"
#include "merge_sort.h"
#include "persistencia.h"
#include "quick_sort.h"
#include "raylib.h"
#include "ui.h"

using namespace ui;

namespace {
const float M = 36;                                  // margen de las pantallas
Color blanco() { return {255, 255, 255, 255}; }
}

App::App() : rng(std::random_device{}()) {}

// ---------------------------------------------------------------------------
// Ciclo principal
// ---------------------------------------------------------------------------
void App::frame() {
    W = GetScreenWidth();
    H = GetScreenHeight();
    ui::inicioFrame();

    switch (pantalla) {
        case Pantalla::Menu:        pantallaMenu(); break;
        case Pantalla::Actividades: pantallaActividades(); break;
        case Pantalla::Ordenar:     pantallaOrdenar(); break;
        case Pantalla::Agenda:      pantallaAgenda(); break;
        case Pantalla::Comparar:    pantallaComparar(); break;
    }
    dibujarAviso();
    ui::finFrame();

    if (hayCambio) {              // el cambio se aplica al final para no procesar un clic dos veces
        hayCambio = false;
        pantalla = destino;
        entrar();
    }
}

void App::irA(Pantalla p) { destino = p; hayCambio = true; }

void App::irAOrdenar(Algoritmo a) { alg = a; irA(Pantalla::Ordenar); }

void App::entrar() {
    switch (pantalla) {
        case Pantalla::Actividades: foco = 0; scrollAct = 0; break;
        case Pantalla::Ordenar:     reiniciarOrdenar(); break;
        case Pantalla::Agenda:      scrollAgenda = 0; generarAgenda(); break;
        case Pantalla::Comparar:    medirComparacion(); break;
        default: break;
    }
}

void App::aviso(const std::string& txt, bool error) {
    avisoTxt = txt;
    avisoError = error;
    avisoHasta = GetTime() + 3.2;
}

void App::dibujarAviso() {
    if (avisoTxt.empty() || GetTime() > avisoHasta) return;
    float w = anchoTexto(avisoTxt, 15, true) + 48;
    Rectangle r = {(W - w) / 2, 18, w, 44};
    rectRedondo(r, 22, avisoError ? Color{190, 52, 58, 255} : TEXTO);
    textoCentro(avisoTxt, r.x + r.width / 2, r.y + 10, 15, blanco(), true);
}

void App::encabezado(const std::string& titulo, const std::string& detalle) {
    Rectangle atras = {M - 12, 24, 100, 40};
    bool hover = sobre(atras);
    float tw = anchoTexto("Menú", 16, true);
    if (boton(atras, "Menú", Boton::Texto)) irA(Pantalla::Menu);
    chevronIzq(atras.x + atras.width / 2 - tw / 2 - 14, atras.y + 20, 6, hover ? ACENTO : TEXTO);

    texto(titulo, atras.x + atras.width + 18, 22, 26, TEXTO, true);
    if (!detalle.empty()) textoDer(detalle, W - M, 32, 14, TEXTO_SUAVE);
}

// ---------------------------------------------------------------------------
// Menu
// ---------------------------------------------------------------------------
void App::pantallaMenu() {
    struct Fila { const char* nombre; const char* desc; const char* tecla; };
    const Fila filas[] = {
        {"Actividades",      "Cargar desde archivo, agregar o ver la lista.",        "1"},
        {"Merge Sort",       "Ordena viendo cómo se dividen y se mezclan los grupos.", "2"},
        {"Quick Sort",       "Ordena viendo cómo el pivote reparte los elementos.",   "3"},
        {"Agenda óptima",    "Elige el máximo de actividades que no se cruzan.",      "4"},
        {"Comparar tiempos", "Mide Merge Sort y Quick Sort con tus datos.",           "5"},
        {"Salir",            "",                                                       "0"},
    };
    const int N = 6;
    const float filaH = 62;
    float ancho = std::min(580.0f, W - 2 * M);
    float x = (W - ancho) / 2;
    float alto = 150 + N * filaH + 56;
    float y = std::max(20.0f, (H - alto) / 2);

    texto("Agenda óptima", x, y, 56, TEXTO, true);
    texto("Ordena tus actividades y quédate con las que no se cruzan.", x + 2, y + 92, 16, TEXTO_SUAVE);

    float yf = y + 150;
    int elegido = -1;
    for (int i = 0; i < N; i++) {
        Rectangle r = {x - 12, yf + i * filaH, ancho + 24, filaH};
        bool hover = sobre(r);
        if (hover) {
            tarjeta(r, 12, SUPERFICIE, ACENTO);
            DrawRectangle((int)r.x + 1, (int)(r.y + 14), 3, (int)(r.height - 28), ACENTO);
        } else if (i < N - 1) {
            DrawLine((int)x, (int)(r.y + r.height), (int)(x + ancho), (int)(r.y + r.height), LINEA);
        }
        if (hover) manoActiva();
        const Fila& f = filas[i];
        bool tieneDesc = f.desc[0] != 0;
        texto(f.nombre, x + 10, r.y + (tieneDesc ? 8 : 17), 20, hover ? ACENTO : TEXTO, true);
        if (tieneDesc) texto(f.desc, x + 10, r.y + 34, 14, TEXTO_SUAVE);

        Rectangle tecla = {x + ancho - 22, r.y + (filaH - 26) / 2, 26, 26};
        tarjeta(tecla, 6, FONDO, LINEA);
        textoCentro(f.tecla, tecla.x + 13, tecla.y + 2, 14, TEXTO_SUAVE, true);

        if (clic(r)) elegido = i;
    }
    if (IsKeyPressed(KEY_ONE)) elegido = 0;
    if (IsKeyPressed(KEY_TWO)) elegido = 1;
    if (IsKeyPressed(KEY_THREE)) elegido = 2;
    if (IsKeyPressed(KEY_FOUR)) elegido = 3;
    if (IsKeyPressed(KEY_FIVE)) elegido = 4;
    if (IsKeyPressed(KEY_ZERO)) elegido = 5;

    switch (elegido) {
        case 0: irA(Pantalla::Actividades); break;
        case 1: irAOrdenar(Algoritmo::Merge); break;
        case 2: irAOrdenar(Algoritmo::Quick); break;
        case 3: irA(Pantalla::Agenda); break;
        case 4: irA(Pantalla::Comparar); break;
        case 5: salir = true; break;
        default: break;
    }

    // estado de los datos
    float ys = yf + N * filaH + 22;
    std::string estado;
    Color punto = BARRA;
    if (actividades.empty()) estado = "Aún no hay actividades. Empieza por Actividades.";
    else {
        estado = std::to_string(actividades.size()) + (actividades.size() == 1 ? " actividad cargada, " : " actividades cargadas, ");
        estado += ordenadas ? "ordenadas por hora de fin." : "sin ordenar.";
        if (ordenadas) punto = VERDE;
    }
    DrawCircle((int)x + 4, (int)ys + 12, 5, punto);
    texto(estado, x + 20, ys, 15, TEXTO_SUAVE);
}

// ---------------------------------------------------------------------------
// Actividades
// ---------------------------------------------------------------------------
void App::cargarDesdeArchivo() {
    bool ok = false;
    std::vector<Actividad> v = cargarActividades("actividades.txt", &ok);
    if (!ok) { aviso("No se pudo abrir actividades.txt. Debe estar junto al programa.", true); return; }
    if (v.empty()) { aviso("actividades.txt no tiene líneas válidas (nombre,inicio,fin).", true); return; }
    actividades = v;
    ultimaAgenda.clear();
    ordenadas = false;
    scrollAct = 0;
    aviso(std::to_string(v.size()) + " actividades cargadas.");
}

void App::generarAleatorias(int cuantas) {
    actividades.clear();
    for (int i = 0; i < cuantas; i++) {
        int ini = (int)(rng() % 20);
        int dur = 1 + (int)(rng() % 4);
        actividades.push_back({"Act_" + std::to_string(i + 1), ini, ini + dur});
    }
    ultimaAgenda.clear();
    ordenadas = false;
    scrollAct = 0;
    aviso(std::to_string(cuantas) + " actividades aleatorias generadas.");
}

void App::agregarManual() {
    if (cNombre.empty()) { aviso("Escribe un nombre para la actividad.", true); foco = 1; return; }
    if (cInicio.empty() || cFin.empty()) { aviso("Completa la hora de inicio y la de fin.", true); foco = cInicio.empty() ? 2 : 3; return; }
    int ini = std::stoi(cInicio), fin = std::stoi(cFin);
    if (fin <= ini) { aviso("La hora de fin debe ser mayor que la de inicio.", true); foco = 3; return; }
    actividades.push_back({cNombre, ini, fin});
    ordenadas = false;
    aviso("«" + cNombre + "» agregada.");
    cNombre.clear(); cInicio.clear(); cFin.clear();
    foco = 1;
    scrollAct = 1e9f;   // baja hasta la ultima fila
}

void App::pantallaActividades() {
    encabezado("Actividades");
    if (IsKeyPressed(KEY_ESCAPE)) { if (foco) foco = 0; else irA(Pantalla::Menu); }

    float top = 100;
    float gap = 20;
    float altoCont = H - top - M;
    float wIzq = std::floor((W - 2 * M - gap) * 0.56f);
    Rectangle tabla = {M, top, wIzq, altoCont};
    Rectangle panel = {M + wIzq + gap, top, W - 2 * M - wIzq - gap, std::min(altoCont, 470.0f)};

    // ---------- tabla ----------
    tarjeta(tabla, 14, SUPERFICIE, LINEA);
    std::string resumen = std::to_string(actividades.size()) + (actividades.size() == 1 ? " actividad" : " actividades");
    if (!actividades.empty()) resumen += ordenadas ? ", ordenadas por hora de fin" : ", sin ordenar";
    texto(resumen, tabla.x + 22, tabla.y + 16, 15, TEXTO_SUAVE);

    float yEnc = tabla.y + 56;
    float colFin = tabla.x + tabla.width - 28, colIni = colFin - 90;
    texto("Nombre", tabla.x + 22, yEnc, 13, TEXTO_SUAVE);
    textoDer("Inicio", colIni, yEnc, 13, TEXTO_SUAVE);
    textoDer("Fin", colFin, yEnc, 13, TEXTO_SUAVE);
    DrawLine((int)tabla.x + 14, (int)yEnc + 26, (int)(tabla.x + tabla.width - 14), (int)yEnc + 26, LINEA);

    float filaH = 34;
    Rectangle zona = {tabla.x + 1, yEnc + 28, tabla.width - 2, tabla.height - (yEnc + 28 - tabla.y) - 12};
    float contenido = actividades.size() * filaH;
    float maxScroll = std::max(0.0f, contenido - zona.height);
    if (sobre(zona)) scrollAct -= GetMouseWheelMove() * filaH * 2;
    scrollAct = std::clamp(scrollAct, 0.0f, maxScroll);

    if (actividades.empty()) {
        textoCentro("No hay actividades", zona.x + zona.width / 2, zona.y + zona.height / 2 - 30, 18, TEXTO, true);
        textoCentro("Carga el archivo o agrega una a mano.", zona.x + zona.width / 2, zona.y + zona.height / 2 - 2, 14, TEXTO_SUAVE);
    }
    BeginScissorMode((int)zona.x, (int)zona.y, (int)zona.width, (int)zona.height);
    for (size_t i = 0; i < actividades.size(); i++) {
        float yy = zona.y + i * filaH - scrollAct;
        if (yy + filaH < zona.y || yy > zona.y + zona.height) continue;
        const Actividad& a = actividades[i];
        texto(ajustar(a.nombre, colIni - tabla.x - 120, 15), tabla.x + 22, yy + 6, 15, TEXTO);
        textoDer(std::to_string(a.inicio), colIni, yy + 6, 15, TEXTO);
        textoDer(std::to_string(a.fin), colFin, yy + 6, 15, TEXTO);
        DrawLine((int)tabla.x + 14, (int)(yy + filaH - 1), (int)(tabla.x + tabla.width - 14), (int)(yy + filaH - 1), {236, 240, 245, 255});
    }
    EndScissorMode();
    if (maxScroll > 0) {                     // barra de desplazamiento
        float th = std::max(30.0f, zona.height * zona.height / contenido);
        float ty = zona.y + (zona.height - th) * (scrollAct / maxScroll);
        rectRedondo({tabla.x + tabla.width - 10, ty, 4, th}, 2, {196, 206, 220, 255});
    }

    // ---------- panel de acciones ----------
    tarjeta(panel, 14, SUPERFICIE, LINEA);
    float px = panel.x + 24, pw = panel.width - 48, y = panel.y + 22;

    texto("Desde archivo", px, y, 16, TEXTO, true);
    if (boton({px, y + 36, pw, 44}, "Cargar actividades.txt", Boton::Primario)) cargarDesdeArchivo();
    if (boton({px, y + 88, pw, 44}, "Generar 30 aleatorias", Boton::Secundario)) generarAleatorias(30);
    texto("Reemplaza la lista actual.", px, y + 140, 13, TEXTO_SUAVE);

    DrawLine((int)px, (int)(y + 178), (int)(px + pw), (int)(y + 178), LINEA);

    texto("Agregar a mano", px, y + 196, 16, TEXTO, true);
    float yc = y + 262;
    bool clicCampo = false;
    if (campo({px, yc, pw, 42}, "Nombre (sin espacios)", cNombre, foco == 1, "Reunion_equipo")) { foco = 1; clicCampo = true; }
    float mitad = (pw - 12) / 2;
    if (campo({px, yc + 80, mitad, 42}, "Hora de inicio", cInicio, foco == 2, "9")) { foco = 2; clicCampo = true; }
    if (campo({px + mitad + 12, yc + 80, mitad, 42}, "Hora de fin", cFin, foco == 3, "10")) { foco = 3; clicCampo = true; }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !clicCampo && !sobre(panel)) foco = 0;

    bool enviar = boton({px, yc + 142, pw, 44}, "Agregar actividad", Boton::Primario);

    // teclado
    if (foco == 1) editar(cNombre, 24, false);
    else if (foco == 2) editar(cInicio, 4, true);
    else if (foco == 3) editar(cFin, 4, true);
    else while (GetCharPressed() > 0) {}
    if (IsKeyPressed(KEY_TAB)) foco = foco % 3 + 1;
    if (foco && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))) enviar = true;
    if (enviar) agregarManual();

    if (scrollAct > maxScroll + 1) scrollAct = maxScroll;
}
