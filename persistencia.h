#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include <vector>
#include <string>
#include "Actividad.h"

// Lee actividades desde un archivo CSV simple: nombre,inicio,fin (una por linea).
// Las lineas mal formadas se ignoran. Si 'ok' no es nulo, indica si el archivo
// se pudo abrir.
std::vector<Actividad> cargarActividades(const std::string& ruta, bool* ok = nullptr);

// Guarda una agenda (por ejemplo, la agenda optima) en el mismo formato.
// Devuelve true si se escribio correctamente.
bool guardarAgenda(const std::string& ruta, const std::vector<Actividad>& agenda);

#endif
