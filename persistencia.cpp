#include "persistencia.h"
#include <fstream>
#include <sstream>

// Quita espacios, tabulaciones y '\r' (archivos creados en Windows) de los extremos.
static std::string recortar(const std::string& s) {
    const char* blancos = " \t\r\n";
    std::size_t ini = s.find_first_not_of(blancos);
    if (ini == std::string::npos) return "";
    std::size_t fin = s.find_last_not_of(blancos);
    return s.substr(ini, fin - ini + 1);
}

std::vector<Actividad> cargarActividades(const std::string& ruta, bool* ok) {
    std::vector<Actividad> actividades;
    std::ifstream archivo(ruta);

    if (ok) *ok = archivo.is_open();
    if (!archivo.is_open()) return actividades;

    std::string linea;
    while (std::getline(archivo, linea)) {
        linea = recortar(linea);
        if (linea.empty()) continue;

        std::stringstream ss(linea);
        std::string nombre, campoInicio, campoFin;

        std::getline(ss, nombre, ',');
        std::getline(ss, campoInicio, ',');
        std::getline(ss, campoFin, ',');

        campoInicio = recortar(campoInicio);
        campoFin = recortar(campoFin);
        if (campoInicio.empty() || campoFin.empty()) continue;

        Actividad a;
        try {
            a.nombre = recortar(nombre);
            a.inicio = std::stoi(campoInicio);
            a.fin = std::stoi(campoFin);
        } catch (...) {
            continue;  // linea con numeros invalidos: se ignora en vez de cerrar el programa
        }
        actividades.push_back(a);
    }
    return actividades;
}

bool guardarAgenda(const std::string& ruta, const std::vector<Actividad>& agenda) {
    std::ofstream archivo(ruta);
    if (!archivo.is_open()) return false;
    for (const auto& a : agenda) {
        archivo << a.nombre << "," << a.inicio << "," << a.fin << "\n";
    }
    return archivo.good();
}
