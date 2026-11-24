#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include <vector>
#include <string>
#include "Actividad.h"

// Lee actividades desde un archivo CSV simple: nombre,inicio,fin (una por linea).
std::vector<Actividad> cargarActividades(const std::string& ruta);

// Guarda una agenda (por ejemplo, la agenda optima) en el mismo formato.
void guardarAgenda(const std::string& ruta, const std::vector<Actividad>& agenda);

#endif
