#include "quick_sort.h"
#include <utility>

static int particionar(std::vector<Actividad>& A, int bajo, int alto) {
    int pivote = A[alto].fin;
    int i = bajo - 1;

    for (int j = bajo; j < alto; j++) {
        if (A[j].fin <= pivote) {
            i++;
            std::swap(A[i], A[j]);
        }
    }
    std::swap(A[i + 1], A[alto]);
    return i + 1;
}

void quickSort(std::vector<Actividad>& A, int bajo, int alto) {
    if (bajo < alto) {
        int indice_pivote = particionar(A, bajo, alto);
        quickSort(A, bajo, indice_pivote - 1);
        quickSort(A, indice_pivote + 1, alto);
    }
}
