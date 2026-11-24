#include "persistencia.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<Actividad> cargarActividades(const std::string& ruta) {
    std::vector<Actividad> actividades;
    std::ifstream archivo(ruta);

    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo: " << ruta << "\n";
        return actividades;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;

        std::stringstream ss(linea);
        std::string nombre, campoInicio, campoFin;

        std::getline(ss, nombre, ',');
        std::getline(ss, campoInicio, ',');
        std::getline(ss, campoFin, ',');

        if (campoInicio.empty() || campoFin.empty()) continue;

        Actividad a;
        a.nombre = nombre;
        a.inicio = std::stoi(campoInicio);
        a.fin = std::stoi(campoFin);
        actividades.push_back(a);
    }
    return actividades;
}

void guardarAgenda(const std::string& ruta, const std::vector<Actividad>& agenda) {
    std::ofstream archivo(ruta);
    for (const auto& a : agenda) {
        archivo << a.nombre << "," << a.inicio << "," << a.fin << "\n";
    }
    std::cout << "Agenda guardada en " << ruta << "\n";
}
