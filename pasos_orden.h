#ifndef PASOS_ORDEN_H
#define PASOS_ORDEN_H

#include <vector>
#include "Actividad.h"

// Registro paso a paso de lo que hacen Merge Sort y Quick Sort. SOLO sirve para
// la animacion: son copias "instrumentadas" de los mismos algoritmos de
// merge_sort.cpp y quick_sort.cpp (misma logica, mismo resultado). El tiempo que
// muestra el programa NO sale de aqui, sino de medicion.cpp.

enum class TipoPaso : unsigned char {
    // Merge Sort
    MezclaInicio,      // a=izq, b=med, c=der: se copia A[izq..der] al arreglo auxiliar
    MezclaCompara,     // a=indice aux del elemento izquierdo, b=indice aux del derecho
    MezclaEscribe,     // a=posicion destino en A, b=indice aux de donde se toma
    MezclaFin,         // a=izq, b=der
    // Quick Sort
    ParticionInicio,   // a=bajo, b=alto (el pivote es A[alto])
    ParticionCompara,  // a=j, b=alto, c=frontera (ultimo indice <= pivote)
    Intercambio,       // a, b; c=1 si es un intercambio del bucle, 0 si es el final
    EnSuLugar          // a=posicion que ya quedo definitiva
};

struct Paso {
    TipoPaso tipo;
    int a, b, c;
};

struct Traza {
    std::vector<Paso> pasos;
    long long comparaciones = 0;
    long long movimientos = 0;   // Merge: escrituras en A. Quick: intercambios.
};

Traza trazaMergeSort(const std::vector<Actividad>& datos);
Traza trazaQuickSort(const std::vector<Actividad>& datos);

#endif
