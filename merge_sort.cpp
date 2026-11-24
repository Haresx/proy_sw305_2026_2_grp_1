#include "merge_sort.h"

static void mezclar(std::vector<Actividad>& A, int izq, int med, int der) {
    int n1 = med - izq + 1;
    int n2 = der - med;

    std::vector<Actividad> L(n1);
    std::vector<Actividad> R(n2);

    for (int i = 0; i < n1; i++) L[i] = A[izq + i];
    for (int j = 0; j < n2; j++) R[j] = A[med + 1 + j];

    int i = 0, j = 0, k = izq;

    while (i < n1 && j < n2) {
        if (L[i].fin <= R[j].fin) {
            A[k] = L[i];
            i++;
        } else {
            A[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) { A[k] = L[i]; i++; k++; }
    while (j < n2) { A[k] = R[j]; j++; k++; }
}

void mergeSort(std::vector<Actividad>& A, int inicio_idx, int fin_idx) {
    if (inicio_idx < fin_idx) {
        int medio = inicio_idx + (fin_idx - inicio_idx) / 2;
        mergeSort(A, inicio_idx, medio);
        mergeSort(A, medio + 1, fin_idx);
        mezclar(A, inicio_idx, medio, fin_idx);
    }
}
