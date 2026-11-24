#include "animador.h"
#include <numeric>
#include <utility>

void Animador::limpiarEstado() {
    size_t n = datos.size();
    ids.assign(n, 0);
    std::iota(ids.begin(), ids.end(), 0);
    marcado.assign(n, 0);
    sig = 0;
    comps = movs = 0;
    completo = false;
    hayRango = false;
    rIzq = 0; rDer = -1;
    pivote = comparaA = comparaB = movA = movB = frontera = jActual = -1;
    mezclando = false;
    aux.clear(); usado.clear();
    auxI = auxJ = escritoK = -1;
    escritosHasta = -1;
}

void Animador::preparar(const std::vector<Actividad>& d, Algoritmo a) {
    alg = a;
    datos = d;
    traza = Traza();
    limpiarEstado();
}

void Animador::iniciar(const std::vector<Actividad>& d, Algoritmo a, Traza t) {
    alg = a;
    datos = d;
    traza = std::move(t);
    limpiarEstado();
    if (traza.pasos.empty()) finalizar();
}

void Animador::reiniciar() {
    limpiarEstado();
    if (traza.pasos.empty()) finalizar();
}

void Animador::finalizar() {
    completo = true;
    hayRango = false;
    mezclando = false;
    aux.clear(); usado.clear();
    pivote = comparaA = comparaB = movA = movB = frontera = jActual = -1;
    auxI = auxJ = escritoK = -1;
    escritosHasta = -1;
    marcado.assign(datos.size(), 1);
}

void Animador::aplicar(const Paso& p) {
    // los resaltados "de un solo paso" se borran en cada tick
    comparaA = comparaB = movA = movB = -1;
    auxI = auxJ = escritoK = -1;

    switch (p.tipo) {
    case TipoPaso::MezclaInicio:
        hayRango = true; rIzq = p.a; mMed = p.b; rDer = p.c;
        mezclando = true;
        aux.assign(ids.begin() + p.a, ids.begin() + p.c + 1);
        usado.assign(aux.size(), 0);
        escritosHasta = p.a - 1;
        break;
    case TipoPaso::MezclaCompara:
        auxI = p.a; auxJ = p.b;
        comps++;
        break;
    case TipoPaso::MezclaEscribe:
        ids[p.a] = aux[p.b];
        usado[p.b] = 1;
        escritoK = p.a;
        escritosHasta = p.a;
        movs++;
        break;
    case TipoPaso::MezclaFin:
        for (int i = p.a; i <= p.b; i++) marcado[i] = 1;
        mezclando = false;
        aux.clear(); usado.clear();
        escritosHasta = -1;
        break;

    case TipoPaso::ParticionInicio:
        hayRango = true; rIzq = p.a; rDer = p.b;
        pivote = p.b;
        frontera = p.a - 1;
        jActual = -1;
        break;
    case TipoPaso::ParticionCompara:
        comparaA = p.a; comparaB = p.b;
        jActual = p.a;
        frontera = p.c;
        comps++;
        break;
    case TipoPaso::Intercambio:
        std::swap(ids[p.a], ids[p.b]);
        movA = p.a; movB = p.b;
        if (p.c == 1) frontera = p.a;
        movs++;
        break;
    case TipoPaso::EnSuLugar:
        marcado[p.a] = 1;
        if (pivote >= 0) { pivote = -1; frontera = -1; jActual = -1; }
        break;
    }
}

void Animador::avanzar(size_t n) {
    while (n-- > 0 && sig < traza.pasos.size()) aplicar(traza.pasos[sig++]);
    if (sig >= traza.pasos.size() && !completo) finalizar();
}

void Animador::saltarAlFinal() {
    while (sig < traza.pasos.size()) aplicar(traza.pasos[sig++]);
    finalizar();
}
