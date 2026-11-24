#include "greedy.h"

std::vector<Actividad> seleccionActividades(const std::vector<Actividad>& ordenadas) {
    std::vector<Actividad> seleccionadas;
    if (ordenadas.empty()) return seleccionadas;

    seleccionadas.push_back(ordenadas[0]);
    int ultima_fin = ordenadas[0].fin;

    for (std::size_t i = 1; i < ordenadas.size(); i++) {
        if (ordenadas[i].inicio >= ultima_fin) {
            seleccionadas.push_back(ordenadas[i]);
            ultima_fin = ordenadas[i].fin;
        }
    }
    return seleccionadas;
}
