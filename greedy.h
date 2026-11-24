#ifndef GREEDY_H
#define GREEDY_H

#include <vector>
#include "Actividad.h"

// Precondicion: 'ordenadas' esta ordenado de forma no decreciente por 'fin'
// (Tabla 1 del informe: F[0] <= F[1] <= ... <= F[n-1]).
// Devuelve el subconjunto maximo de actividades mutuamente compatibles.
std::vector<Actividad> seleccionActividades(const std::vector<Actividad>& ordenadas);

#endif
