#ifndef MEDICION_H
#define MEDICION_H

#include <string>
#include <vector>
#include "Actividad.h"

enum class Algoritmo { Merge, Quick };

struct Medicion {
    double ns;      // tiempo real de UN ordenamiento, en nanosegundos
    int corridas;   // cuantas ejecuciones se usaron para obtenerlo
};

// Mide el tiempo REAL de mergeSort() / quickSort() (los de merge_sort.cpp y
// quick_sort.cpp, sin instrumentar) sobre una copia de 'datos'.
// Con pocos datos un solo ordenamiento dura menos que la resolucion del reloj,
// asi que se ejecuta en lotes y se devuelve la mediana por ordenamiento.
Medicion medirOrdenamiento(Algoritmo alg, const std::vector<Actividad>& datos);

// "830 ns", "15.3 µs", "2.41 ms", "1.20 s"
std::string formatearTiempo(double ns);

#endif
