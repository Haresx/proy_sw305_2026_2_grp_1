#ifndef ANIMADOR_H
#define ANIMADOR_H

#include <vector>
#include "Actividad.h"
#include "medicion.h"
#include "pasos_orden.h"

// Estado visual de la animacion: reproduce una Traza paso a paso. No mide nada
// ni ordena nada por si mismo; solo muestra lo que paso.
struct Animador {
    Algoritmo alg = Algoritmo::Merge;
    std::vector<Actividad> datos;     // actividades en el orden inicial (id = indice)
    Traza traza;
    size_t sig = 0;                   // siguiente paso por aplicar
    bool completo = false;

    std::vector<int> ids;             // ids[pos] = actividad que hay ahora en la posicion pos
    std::vector<char> marcado;        // Merge: ya mezclado al menos una vez. Quick: ya en su lugar.
    long long comps = 0, movs = 0;

    // rango de trabajo actual
    bool hayRango = false;
    int rIzq = 0, rDer = -1;

    // resaltados del paso actual
    int pivote = -1, comparaA = -1, comparaB = -1, movA = -1, movB = -1;
    int frontera = -1, jActual = -1;

    // Merge: arreglo auxiliar
    bool mezclando = false;
    int mMed = 0;
    std::vector<int> aux;
    std::vector<char> usado;
    int auxI = -1, auxJ = -1, escritoK = -1;
    int escritosHasta = -1;           // ultima posicion escrita en la mezcla en curso

    // Muestra 'datos' sin pasos (estado de espera).
    void preparar(const std::vector<Actividad>& d, Algoritmo a);
    // Carga datos + traza, listo para reproducir.
    void iniciar(const std::vector<Actividad>& d, Algoritmo a, Traza t);
    // Vuelve al estado inicial (desordenado) para repetir la animacion.
    void reiniciar();
    void avanzar(size_t n);
    void saltarAlFinal();
    // Marca todo como terminado sin traza (cuando no se anima).
    void finalizar();

    bool hayTraza() const { return !traza.pasos.empty(); }
    bool terminado() const { return completo; }
    size_t totalPasos() const { return traza.pasos.size(); }
    const Actividad& actividadEn(int pos) const { return datos[ids[pos]]; }

private:
    void aplicar(const Paso& p);
    void limpiarEstado();
};

#endif
