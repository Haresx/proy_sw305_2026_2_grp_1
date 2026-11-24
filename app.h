#ifndef APP_H
#define APP_H

#include <random>
#include <string>
#include <vector>

#include "Actividad.h"
#include "animador.h"
#include "medicion.h"
#include "raylib.h"

// Aplicacion grafica. Conserva los mismos datos y operaciones que el menu de
// consola original (cargar, agregar, ver, ordenar con Merge/Quick, greedy,
// guardar, comparar), repartidos en pantallas.
class App {
public:
    App();
    void frame();            // procesa la entrada y dibuja un cuadro
    bool salir = false;

private:
    enum class Pantalla { Menu, Actividades, Ordenar, Agenda, Comparar };
    enum class Fase { Inactiva, Reproduciendo, Pausada, Terminada };

    // ---- datos del programa (iguales a los del main original) ----
    std::vector<Actividad> actividades;
    std::vector<Actividad> ultimaAgenda;
    bool ordenadas = false;
    std::mt19937 rng;

    // ---- navegacion / avisos ----
    Pantalla pantalla = Pantalla::Menu;
    Pantalla destino = Pantalla::Menu;
    bool hayCambio = false;
    int W = 0, H = 0;
    std::string avisoTxt;
    bool avisoError = false;
    double avisoHasta = 0;

    void irA(Pantalla p);
    void irAOrdenar(Algoritmo a);
    void entrar();
    void aviso(const std::string& txt, bool error = false);
    void dibujarAviso();
    void encabezado(const std::string& titulo, const std::string& detalle = "");

    // ---- Menu ----
    void pantallaMenu();

    // ---- Actividades ----
    std::string cNombre, cInicio, cFin;
    int foco = 0;                 // 0 ninguno, 1 nombre, 2 inicio, 3 fin
    float scrollAct = 0;
    void pantallaActividades();
    void cargarDesdeArchivo();
    void generarAleatorias(int cuantas);
    void agregarManual();

    // ---- Ordenar (Merge Sort / Quick Sort) ----
    Algoritmo alg = Algoritmo::Merge;
    Animador anim;
    Fase fase = Fase::Inactiva;
    Medicion med = {0, 0};
    bool hayMed = false;
    double acum = 0, tasaBase = 10;
    int idxVel = 2;
    void pantallaOrdenar();
    void reiniciarOrdenar();
    void iniciarOrdenamiento(bool saltar);
    void accionPrincipal();
    void saltarAnimacion();
    void repetirAnimacion();
    void mezclar();
    std::string descripcionPaso() const;
    void dibujarLienzo(Rectangle cv);
    void dibujarPanelTiempo(Rectangle p);

    // ---- Agenda optima (Greedy) ----
    float scrollAgenda = 0;
    std::vector<char> agSel;      // por actividad (ya ordenada): la eligio el greedy
    std::vector<int> agCruza;     // si se descarto: indice de la elegida con la que se cruza
    void pantallaAgenda();
    void generarAgenda();
    void guardarAgendaActual();

    // ---- Comparar tiempos ----
    bool compListo = false;
    Medicion compMerge = {0, 0}, compQuick = {0, 0};
    void pantallaComparar();
    void medirComparacion();
};

#endif
