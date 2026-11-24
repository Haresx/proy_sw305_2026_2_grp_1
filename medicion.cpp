#include "medicion.h"
#include "merge_sort.h"
#include "quick_sort.h"

#include <algorithm>
#include <chrono>
#include <cstdio>

namespace {

using reloj = std::chrono::steady_clock;

void ejecutar(Algoritmo alg, std::vector<Actividad>& v) {
    if (v.size() < 2) return;
    if (alg == Algoritmo::Merge) mergeSort(v, 0, (int)v.size() - 1);
    else quickSort(v, 0, (int)v.size() - 1);
}

// Tiempo promedio (ns) de ordenar 'reps' copias. Las copias se preparan antes
// de arrancar el reloj, asi que solo se mide el ordenamiento.
double tiempoLote(Algoritmo alg, const std::vector<Actividad>& datos, int reps) {
    std::vector<std::vector<Actividad>> copias(reps, datos);
    auto t0 = reloj::now();
    for (auto& c : copias) ejecutar(alg, c);
    auto t1 = reloj::now();
    return std::chrono::duration<double, std::nano>(t1 - t0).count() / reps;
}

}  // namespace

Medicion medirOrdenamiento(Algoritmo alg, const std::vector<Actividad>& datos) {
    const double LOTE_NS = 2e6;   // cada lote debe durar ~2 ms para superar la resolucion del reloj

    double t1 = tiempoLote(alg, datos, 1);          // primera corrida: calibracion
    if (t1 >= 5e7) return {t1, 1};                  // >50 ms: una sola corrida ya es precisa

    double repsD = LOTE_NS / std::max(t1, 20.0);
    int topePorMemoria = (int)std::max<std::size_t>(1, 2000000 / std::max<std::size_t>(1, datos.size()));
    int reps = (int)std::min({repsD, 5000.0, (double)topePorMemoria});
    if (reps < 1) reps = 1;

    int lotes = (t1 >= LOTE_NS) ? 3 : 7;
    std::vector<double> muestras;
    for (int i = 0; i < lotes; i++) muestras.push_back(tiempoLote(alg, datos, reps));
    std::sort(muestras.begin(), muestras.end());
    return {muestras[muestras.size() / 2], reps * lotes};
}

std::string formatearTiempo(double ns) {
    char buf[48];
    if (ns < 1000.0)         std::snprintf(buf, sizeof buf, "%.0f ns", ns);
    else if (ns < 1e6)       std::snprintf(buf, sizeof buf, "%.1f \xC2\xB5" "s", ns / 1e3);
    else if (ns < 1e9)       std::snprintf(buf, sizeof buf, "%.2f ms", ns / 1e6);
    else                     std::snprintf(buf, sizeof buf, "%.2f s", ns / 1e9);
    return buf;
}
