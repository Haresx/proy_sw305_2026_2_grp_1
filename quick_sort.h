#ifndef QUICK_SORT_H
#define QUICK_SORT_H

#include <vector>
#include "Actividad.h"

// Ordena A[bajo..alto] de forma no decreciente por el campo 'fin'.
// Particion de Lomuto con pivote = ultimo elemento (igual que el informe,
// seccion 3.1), por lo que hereda el mismo peor caso O(n^2) con datos
// ya ordenados o con muchos duplicados.
void quickSort(std::vector<Actividad>& A, int bajo, int alto);

#endif
