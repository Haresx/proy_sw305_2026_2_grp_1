#ifndef ACTIVIDAD_H
#define ACTIVIDAD_H

#include <string>

// Misma estructura usada en el informe (Tabla 1): cada actividad
// tiene un nombre y un intervalo [inicio, fin).
struct Actividad {
    std::string nombre;
    int inicio;
    int fin;
};

#endif
