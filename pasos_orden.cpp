#include "pasos_orden.h"
#include <numeric>
#include <utility>

namespace {

// Ambos algoritmos trabajan sobre "ids" (posiciones en 'datos') y comparan
// datos[id].fin, igual que los originales comparan A[i].fin.

struct GrabadorMerge {
    const std::vector<Actividad>& d;
    std::vector<int> A;
    Traza t;

    explicit GrabadorMerge(const std::vector<Actividad>& datos) : d(datos), A(datos.size()) {
        std::iota(A.begin(), A.end(), 0);
    }

    int clave(int id) const { return d[id].fin; }
    void paso(TipoPaso tp, int a, int b, int c = 0) { t.pasos.push_back({tp, a, b, c}); }

    void mezclar(int izq, int med, int der) {
        int n1 = med - izq + 1;
        int n2 = der - med;
        std::vector<int> L(A.begin() + izq, A.begin() + med + 1);
        std::vector<int> R(A.begin() + med + 1, A.begin() + der + 1);
        paso(TipoPaso::MezclaInicio, izq, med, der);

        int i = 0, j = 0, k = izq;
        while (i < n1 && j < n2) {
            t.comparaciones++;
            paso(TipoPaso::MezclaCompara, i, n1 + j);
            if (clave(L[i]) <= clave(R[j])) {
                A[k] = L[i];
                paso(TipoPaso::MezclaEscribe, k, i);
                i++;
            } else {
                A[k] = R[j];
                paso(TipoPaso::MezclaEscribe, k, n1 + j);
                j++;
            }
            t.movimientos++;
            k++;
        }
        while (i < n1) { A[k] = L[i]; paso(TipoPaso::MezclaEscribe, k, i); t.movimientos++; i++; k++; }
        while (j < n2) { A[k] = R[j]; paso(TipoPaso::MezclaEscribe, k, n1 + j); t.movimientos++; j++; k++; }
        paso(TipoPaso::MezclaFin, izq, der);
    }

    void ordenar(int ini, int fin) {
        if (ini < fin) {
            int medio = ini + (fin - ini) / 2;
            ordenar(ini, medio);
            ordenar(medio + 1, fin);
            mezclar(ini, medio, fin);
        }
    }
};

struct GrabadorQuick {
    const std::vector<Actividad>& d;
    std::vector<int> A;
    Traza t;

    explicit GrabadorQuick(const std::vector<Actividad>& datos) : d(datos), A(datos.size()) {
        std::iota(A.begin(), A.end(), 0);
    }

    int clave(int id) const { return d[id].fin; }
    void paso(TipoPaso tp, int a, int b, int c = 0) { t.pasos.push_back({tp, a, b, c}); }

    int particionar(int bajo, int alto) {
        int pivote = clave(A[alto]);
        int i = bajo - 1;
        paso(TipoPaso::ParticionInicio, bajo, alto);

        for (int j = bajo; j < alto; j++) {
            t.comparaciones++;
            paso(TipoPaso::ParticionCompara, j, alto, i);
            if (clave(A[j]) <= pivote) {
                i++;
                std::swap(A[i], A[j]);
                t.movimientos++;
                paso(TipoPaso::Intercambio, i, j, 1);
            }
        }
        std::swap(A[i + 1], A[alto]);
        t.movimientos++;
        paso(TipoPaso::Intercambio, i + 1, alto, 0);
        paso(TipoPaso::EnSuLugar, i + 1, 0);
        return i + 1;
    }

    void ordenar(int bajo, int alto) {
        if (bajo < alto) {
            int p = particionar(bajo, alto);
            ordenar(bajo, p - 1);
            ordenar(p + 1, alto);
        } else if (bajo == alto) {
            paso(TipoPaso::EnSuLugar, bajo, 0);   // un solo elemento: ya esta en su sitio
        }
    }
};

}  // namespace

Traza trazaMergeSort(const std::vector<Actividad>& datos) {
    GrabadorMerge g(datos);
    if (!datos.empty()) g.ordenar(0, (int)datos.size() - 1);
    return std::move(g.t);
}

Traza trazaQuickSort(const std::vector<Actividad>& datos) {
    GrabadorQuick g(datos);
    if (!datos.empty()) g.ordenar(0, (int)datos.size() - 1);
    return std::move(g.t);
}
