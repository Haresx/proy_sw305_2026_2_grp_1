#ifndef MERGE_SORT_H
#define MERGE_SORT_H

#include <vector>
#include "Actividad.h"

// Ordena A[inicio_idx..fin_idx] de forma no decreciente por el campo 'fin'.
// Misma especificacion AS&ST del informe (seccion 3.1 / Tabla 1).
void mergeSort(std::vector<Actividad>& A, int inicio_idx, int fin_idx);

#endif
