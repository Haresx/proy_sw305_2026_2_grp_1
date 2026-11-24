// Pantalla "Comparar tiempos": Merge Sort vs Quick Sort sobre los datos actuales.

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "app.h"
#include "raylib.h"
#include "ui.h"

using namespace ui;

namespace {
const float M = 36;
const Color COLOR_QUICK = {232, 135, 30, 255};
}

void App::medirComparacion() {
    compListo = false;
    if (actividades.size() < 2) return;
    // misma comparacion del menu original (opcion 8): ambos sobre los datos tal como estan ahora
    compMerge = medirOrdenamiento(Algoritmo::Merge, actividades);
    compQuick = medirOrdenamiento(Algoritmo::Quick, actividades);
    compListo = true;
}

void App::pantallaComparar() {
    encabezado("Comparar tiempos", "Tiempos reales de cada algoritmo");
    if (IsKeyPressed(KEY_ESCAPE)) irA(Pantalla::Menu);

    float w = std::min(760.0f, W - 2 * M);
    float x = (W - w) / 2;

    if (actividades.size() < 2) {
        float cw = std::min(560.0f, W - 2 * M), ch = 220;
        Rectangle c = {(W - cw) / 2, std::max(110.0f, (H - ch) / 2 - 10), cw, ch};
        tarjeta(c, 14, SUPERFICIE, LINEA);
        texto("Faltan actividades", c.x + 32, c.y + 30, 22, TEXTO, true);
        parrafo("Se necesitan al menos 2 actividades para comparar.", {c.x + 32, c.y + 74, c.width - 64, 40}, 16, TEXTO_SUAVE);
        if (boton({c.x + 32, c.y + 138, 200, 48}, "Ir a Actividades", Boton::Primario)) irA(Pantalla::Actividades);
        return;
    }

    double m = compMerge.ns, q = compQuick.ns;
    double mayor = std::max(m, q);

    std::string datos = "Medido sobre " + std::to_string(actividades.size()) + " actividades, tal como están ahora";
    datos += ordenadas ? " (ya ordenadas por hora de fin)." : " (sin ordenar).";
    texto(ajustar(datos, w, 15), x, 96, 15, TEXTO_SUAVE);

    // ---------- barras ----------
    Rectangle card = {x, 136, w, 196};
    tarjeta(card, 14, SUPERFICIE, LINEA);
    float lblW = 120, tiempoW = 120, pad = 28;
    float bx = card.x + pad + lblW, bwMax = card.width - 2 * pad - lblW - tiempoW;

    struct Fila { const char* nombre; double ns; Color c; };
    Fila filas[2] = {{"Merge Sort", m, ACENTO}, {"Quick Sort", q, COLOR_QUICK}};
    for (int i = 0; i < 2; i++) {
        float y = card.y + 34 + i * 72;
        texto(filas[i].nombre, card.x + pad, y + 6, 17, TEXTO, true);
        rectRedondo({bx, y, bwMax, 36}, 8, {240, 243, 247, 255});
        float f = mayor > 0 ? (float)(filas[i].ns / mayor) : 0;
        rectRedondo({bx, y, std::max(8.0f, bwMax * f), 36}, 8, filas[i].c);
        textoDer(formatearTiempo(filas[i].ns), card.x + card.width - pad, y + 5, 20, TEXTO, true);
    }

    // ---------- lectura de los resultados ----------
    float y = card.y + card.height + 24;
    bool quickMuchoMasLento = q > m * 3 && m > 0;
    if (m > 0 && q > 0) {
        char buf[96];
        double r = q / m;
        if (r >= 1.0) std::snprintf(buf, sizeof buf, "Quick Sort tardó %.1f veces lo que Merge Sort.", r);
        else          std::snprintf(buf, sizeof buf, "Merge Sort tardó %.1f veces lo que Quick Sort.", 1.0 / r);
        texto(buf, x, y, 20, TEXTO, true);
        y += 40;
    }
    std::string veredicto = quickMuchoMasLento
        ? "Quick Sort fue bastante más lento. Si las actividades ya venían ordenadas (o con muchos duplicados en «fin»), "
          "esto coincide con el peor caso O(n²) visto en el informe (Tabla 2)."
        : "Ambos tiempos son comparables: con estos datos no se está disparando el peor caso de Quick Sort.";
    float h = parrafo(veredicto, {x, y, w, 80}, 16, TEXTO_SUAVE);
    y += h + 10;
    if (ordenadas) {
        h = parrafo("Las actividades ya están ordenadas por hora de fin, el caso más desfavorable para este Quick Sort (pivote = último elemento). "
                    "Con muchas actividades la diferencia se nota.", {x, y, w, 80}, 14, TEXTO_SUAVE);
        y += h + 10;
    }

    if (boton({x, std::min(y + 14, (float)H - M - 48), 190, 48}, "Medir de nuevo", Boton::Secundario)) medirComparacion();
}
