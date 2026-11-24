#include "ui.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <vector>

namespace ui {

static void dibujarTooltip(const std::vector<std::string>& lineas);
static std::vector<std::string> tooltipPendiente;

// ---------------------------------------------------------------------------
// Tipografia: una fuente cargada a cada tamaño exacto (queda nitida), en dos
// pesos. Si no se encuentran los .ttf se usa la fuente por defecto de raylib.
// ---------------------------------------------------------------------------
namespace {

// raylib interpreta 'fontSize' como la altura de linea (ascendente + descendente), no
// como el tamaño "em". En Poppins esa altura es 1.4 em; se convierte para que los
// tamaños usados en el diseño (14, 16, 20...) correspondan al tamaño visual normal.
const float K_LINEA = 1.4f;
struct Fnt { Font f; int px; };
std::map<int, Fnt> fuentesRegular, fuentesNegrita;
std::string rutaRegular, rutaNegrita;
bool usarDefecto = false;
const int TAMANOS[] = {12, 13, 14, 15, 16, 18, 20, 22, 26, 30, 36, 56, 64};

int tamanoCercano(int t) {
    int mejor = TAMANOS[0];
    for (int s : TAMANOS) if (std::abs(s - t) < std::abs(mejor - t)) mejor = s;
    return mejor;
}

std::string buscar(const std::string& archivo) {
    std::string a = std::string(GetApplicationDirectory()) + "assets/" + archivo;
    if (FileExists(a.c_str())) return a;
    std::string b = "assets/" + archivo;
    if (FileExists(b.c_str())) return b;
    return "";
}

Font cargar(const std::string& ruta, int tam) {
    // Latin-1 completo: acentos, ñ, ¿, ¡, µ, «», ² ...
    static std::vector<int> puntos;
    if (puntos.empty()) {
        for (int c = 32; c <= 126; c++) puntos.push_back(c);
        for (int c = 160; c <= 255; c++) puntos.push_back(c);
    }
    Font f = LoadFontEx(ruta.c_str(), tam, puntos.data(), (int)puntos.size());
    SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
    return f;
}

Fnt& fuente(int tam, bool negrita) {
    auto& mapa = negrita ? fuentesNegrita : fuentesRegular;
    int t = tamanoCercano(tam);
    auto it = mapa.find(t);
    if (it == mapa.end()) {
        Fnt f;
        if (usarDefecto) { f.f = GetFontDefault(); f.px = t; }
        else { f.px = (int)std::lround(t * K_LINEA); f.f = cargar(negrita ? rutaNegrita : rutaRegular, f.px); }
        it = mapa.emplace(t, f).first;
    }
    return it->second;
}

// UTF-8 de un codepoint
void agregarUtf8(std::string& s, int cp) {
    if (cp < 0x80) s += (char)cp;
    else { s += (char)(0xC0 | (cp >> 6)); s += (char)(0x80 | (cp & 0x3F)); }
}

bool cursorMano = false;
bool cursorTexto = false;

}  // namespace

void iniciarFuentes() {
    rutaRegular = buscar("Poppins-Regular.ttf");
    rutaNegrita = buscar("Poppins-Medium.ttf");
    usarDefecto = rutaRegular.empty() || rutaNegrita.empty();
    if (!usarDefecto) for (int t : TAMANOS) { fuente(t, false); fuente(t, true); }
}

void cerrarFuentes() {
    if (!usarDefecto) {
        for (auto& kv : fuentesRegular) UnloadFont(kv.second.f);
        for (auto& kv : fuentesNegrita) UnloadFont(kv.second.f);
    }
    fuentesRegular.clear(); fuentesNegrita.clear();
}

float anchoTexto(const std::string& t, int tam, bool negrita) {
    Fnt& f = fuente(tam, negrita);
    return MeasureTextEx(f.f, t.c_str(), (float)f.px, 0).x;
}

void texto(const std::string& t, float x, float y, int tam, Color c, bool negrita) {
    Fnt& f = fuente(tam, negrita);
    DrawTextEx(f.f, t.c_str(), {std::round(x), std::round(y)}, (float)f.px, 0, c);
}

void textoDer(const std::string& t, float xDer, float y, int tam, Color c, bool negrita) {
    texto(t, xDer - anchoTexto(t, tam, negrita), y, tam, c, negrita);
}

void textoCentro(const std::string& t, float xCentro, float y, int tam, Color c, bool negrita) {
    texto(t, xCentro - anchoTexto(t, tam, negrita) / 2, y, tam, c, negrita);
}

float parrafo(const std::string& t, Rectangle r, int tam, Color c, bool negrita) {
    float lh = tam * 1.45f, y = 0;
    std::string linea, palabra;
    auto volcar = [&]() {
        if (!linea.empty()) { texto(linea, r.x, r.y + y, tam, c, negrita); y += lh; linea.clear(); }
    };
    size_t i = 0;
    while (i <= t.size()) {
        if (i == t.size() || t[i] == ' ' || t[i] == '\n') {
            if (!palabra.empty()) {
                std::string prueba = linea.empty() ? palabra : linea + " " + palabra;
                if (!linea.empty() && anchoTexto(prueba, tam, negrita) > r.width) { volcar(); linea = palabra; }
                else linea = prueba;
                palabra.clear();
            }
            if (i < t.size() && t[i] == '\n') volcar();
        } else palabra += t[i];
        i++;
    }
    volcar();
    return y;
}

std::string ajustar(const std::string& t, float ancho, int tam, bool negrita) {
    if (anchoTexto(t, tam, negrita) <= ancho) return t;
    std::string s = t;
    while (!s.empty() && anchoTexto(s + "...", tam, negrita) > ancho) s.pop_back();
    return s + "...";
}

// ---------------------------------------------------------------------------
void rectRedondo(Rectangle r, float radio, Color c) {
    float m = std::min(r.width, r.height);
    if (m <= 0) return;
    float roundness = std::min(1.0f, radio * 2.0f / m);
    DrawRectangleRounded(r, roundness, 10, c);
}

void tarjeta(Rectangle r, float radio, Color relleno, Color borde) {
    rectRedondo(r, radio, borde);
    rectRedondo({r.x + 1, r.y + 1, r.width - 2, r.height - 2}, std::max(0.0f, radio - 1), relleno);
}

void chevronIzq(float cx, float cy, float tam, Color c) {
    DrawLineEx({cx + tam * 0.4f, cy - tam}, {cx - tam * 0.4f, cy}, 2.2f, c);
    DrawLineEx({cx - tam * 0.4f, cy}, {cx + tam * 0.4f, cy + tam}, 2.2f, c);
}

// ---------------------------------------------------------------------------
void inicioFrame() { cursorMano = false; cursorTexto = false; }

void finFrame() {
    dibujarTooltip(tooltipPendiente);
    tooltipPendiente.clear();
    SetMouseCursor(cursorMano ? MOUSE_CURSOR_POINTING_HAND : (cursorTexto ? MOUSE_CURSOR_IBEAM : MOUSE_CURSOR_DEFAULT));
}

bool sobre(Rectangle r) { return CheckCollisionPointRec(GetMousePosition(), r); }

void manoActiva() { cursorMano = true; }

bool clic(Rectangle r) { return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && sobre(r); }

bool boton(Rectangle r, const std::string& etiqueta, Boton estilo, bool activo) {
    bool hover = activo && sobre(r);
    if (hover) cursorMano = true;

    Color texto_c = TEXTO;
    if (estilo == Boton::Primario) {
        Color fondo = !activo ? Color{190, 200, 222, 255} : (hover ? ACENTO_HOVER : ACENTO);
        rectRedondo(r, 10, fondo);
        texto_c = {255, 255, 255, 255};
    } else if (estilo == Boton::Secundario) {
        tarjeta(r, 10, hover ? ACENTO_TENUE : SUPERFICIE, hover ? ACENTO : LINEA);
        texto_c = activo ? TEXTO : Color{160, 172, 186, 255};
    } else {
        if (hover) rectRedondo(r, 10, ACENTO_TENUE);
        texto_c = activo ? (hover ? ACENTO : TEXTO) : Color{160, 172, 186, 255};
    }
    const int tam = 16;
    float ty = r.y + (r.height - tam * 1.5f) / 2 + 1;
    textoCentro(etiqueta, r.x + r.width / 2, ty, tam, texto_c, true);
    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

bool campo(Rectangle r, const std::string& etiqueta, const std::string& valor,
           bool enfocado, const std::string& pista) {
    texto(etiqueta, r.x, r.y - 22, 14, TEXTO_SUAVE);
    bool hover = sobre(r);
    if (hover) cursorTexto = true;
    tarjeta(r, 8, SUPERFICIE, enfocado ? ACENTO : (hover ? Color{176, 188, 204, 255} : LINEA));
    if (enfocado) rectRedondo({r.x - 1, r.y + r.height - 3, r.width + 2, 3}, 1.5f, ACENTO);

    float ty = r.y + (r.height - 16 * 1.5f) / 2 + 1;
    if (valor.empty() && !enfocado) texto(pista, r.x + 12, ty, 16, {176, 188, 204, 255});
    else texto(valor, r.x + 12, ty, 16, TEXTO);
    if (enfocado && ((int)(GetTime() * 2) % 2 == 0)) {
        float cx = r.x + 12 + anchoTexto(valor, 16) + 1;
        DrawRectangle((int)cx, (int)(r.y + 9), 2, (int)(r.height - 18), ACENTO);
    }
    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void editar(std::string& s, size_t maxLen, bool soloDigitos) {
    for (int cp = GetCharPressed(); cp > 0; cp = GetCharPressed()) {
        if (cp == ',') continue;                               // separador del archivo CSV
        if (soloDigitos) { if (cp < '0' || cp > '9') continue; }
        else {
            if (cp == ' ') cp = '_';                           // el archivo no admite espacios en el nombre
            if (!((cp >= 33 && cp <= 126) || (cp >= 160 && cp <= 255))) continue;
        }
        // contar caracteres (no bytes) para respetar maxLen
        size_t chars = 0;
        for (unsigned char ch : s) if ((ch & 0xC0) != 0x80) chars++;
        if (chars < maxLen) agregarUtf8(s, cp);
    }
    // borrar con repeticion al mantener presionado
    static double siguiente = 0;
    auto borrarUno = [&]() {
        while (!s.empty() && ((unsigned char)s.back() & 0xC0) == 0x80) s.pop_back();
        if (!s.empty()) s.pop_back();
    };
    if (IsKeyPressed(KEY_BACKSPACE)) { borrarUno(); siguiente = GetTime() + 0.4; }
    else if (IsKeyDown(KEY_BACKSPACE) && GetTime() >= siguiente) { borrarUno(); siguiente = GetTime() + 0.035; }
}

// ---------------------------------------------------------------------------
static void dibujarTooltip(const std::vector<std::string>& lineas) {
    if (lineas.empty()) return;
    float ancho = 0;
    for (size_t i = 0; i < lineas.size(); i++) ancho = std::max(ancho, anchoTexto(lineas[i], i == 0 ? 15 : 13, i == 0));
    float w = ancho + 28;
    float h = 16 + 24 + (lineas.size() - 1) * 21 + 4;
    Vector2 m = GetMousePosition();
    float x = m.x + 16, y = m.y - h - 10;
    if (x + w > GetScreenWidth() - 8) x = m.x - w - 12;
    if (y < 8) y = m.y + 20;
    rectRedondo({x, y, w, h}, 8, TEXTO);
    for (size_t i = 0; i < lineas.size(); i++) {
        if (i == 0) texto(lineas[i], x + 14, y + 10, 15, {255, 255, 255, 255}, true);
        else        texto(lineas[i], x + 14, y + 10 + 24 + (i - 1) * 21, 13, {196, 206, 220, 255});
    }
}

// Se guarda y se dibuja en finFrame(), para quedar siempre por encima de todo.
void tooltip(const std::vector<std::string>& lineas) { tooltipPendiente = lineas; }

std::string miles(long long n) {
    std::string s = std::to_string(n), r;
    int cuenta = 0;
    for (int i = (int)s.size() - 1; i >= 0; i--) {
        r.insert(r.begin(), s[i]);
        if (++cuenta % 3 == 0 && i > 0) r.insert(r.begin(), ' ');
    }
    return r;
}

void barra(int x, float yBase, int w, float h, Color c) {
    if (h < 1) h = 1;
    float r = std::min(4.0f, w / 2.0f);
    rectRedondo({(float)x, yBase - h, (float)w, h}, r, c);
    float hb = std::min(r + 1, h);                      // base recta
    DrawRectangle(x, (int)(yBase - hb), w, (int)std::ceil(hb), c);
}

void barraFantasma(int x, float yBase, int w, float h, Color c) {
    barra(x, yBase, w, h, c);
    if (w > 4 && h > 4) barra(x + 2, yBase - 2, w - 4, h - 4, SUPERFICIE);
}

}  // namespace ui
