#include <iostream>
#include <vector>
#include <chrono>
#include <limits>

#include "Actividad.h"
#include "merge_sort.h"
#include "quick_sort.h"
#include "greedy.h"
#include "persistencia.h"

using namespace std;
using namespace std::chrono;

static void mostrarActividades(const vector<Actividad>& acts) {
    if (acts.empty()) {
        cout << "(no hay actividades)\n";
        return;
    }
    cout << "Nombre\t\tInicio\tFin\n";
    for (const auto& a : acts) {
        cout << a.nombre << "\t\t" << a.inicio << "\t" << a.fin << "\n";
    }
}

static void limpiarEntrada() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static void compararTiempos(const vector<Actividad>& original) {
    if (original.size() < 2) {
        cout << "Se necesitan al menos 2 actividades para comparar.\n";
        return;
    }

    vector<Actividad> copiaMerge = original;
    vector<Actividad> copiaQuick = original;

    auto t1 = high_resolution_clock::now();
    mergeSort(copiaMerge, 0, (int)copiaMerge.size() - 1);
    auto t2 = high_resolution_clock::now();
    long long tiempoMerge = duration_cast<microseconds>(t2 - t1).count();

    auto t3 = high_resolution_clock::now();
    quickSort(copiaQuick, 0, (int)copiaQuick.size() - 1);
    auto t4 = high_resolution_clock::now();
    long long tiempoQuick = duration_cast<microseconds>(t4 - t3).count();

    cout << "\n--- Comparacion sobre " << original.size() << " actividades ---\n";
    cout << "Merge Sort: " << tiempoMerge << " microsegundos\n";
    cout << "Quick Sort: " << tiempoQuick << " microsegundos\n";

    if (tiempoQuick > tiempoMerge * 3 && tiempoMerge > 0) {
        cout << "Quick Sort fue bastante mas lento. Si las actividades ya venian "
             << "ordenadas (o con muchos duplicados en 'fin'), esto coincide con el "
             << "peor caso O(n^2) visto en el informe (Tabla 2).\n";
    } else {
        cout << "Ambos tiempos son comparables: con estos datos no se esta "
             << "disparando el peor caso de Quick Sort.\n";
    }
}

int main() {
    vector<Actividad> actividades;
    vector<Actividad> ultimaAgenda;
    bool ordenadas = false;

    int opcion = -1;
    do {
        cout << "\n===== AGENDA OPTIMA (Merge Sort / Quick Sort + Greedy) =====\n";
        cout << "1. Cargar actividades desde archivo (actividades.txt)\n";
        cout << "2. Agregar actividad manualmente\n";
        cout << "3. Ver actividades cargadas\n";
        cout << "4. Ordenar con Merge Sort\n";
        cout << "5. Ordenar con Quick Sort\n";
        cout << "6. Generar agenda optima (Greedy)\n";
        cout << "7. Guardar la ultima agenda optima en archivo\n";
        cout << "8. Comparar tiempos: Merge Sort vs Quick Sort\n";
        cout << "0. Salir\n";
        cout << "Opcion: ";

        if (!(cin >> opcion)) {
            limpiarEntrada();
            continue;
        }

        if (opcion == 1) {
            actividades = cargarActividades("actividades.txt");
            ordenadas = false;
            cout << actividades.size() << " actividades cargadas.\n";

        } else if (opcion == 2) {
            Actividad a;
            cout << "Nombre (sin espacios, usa '_'): ";
            cin >> a.nombre;
            cout << "Hora de inicio (entero): ";
            cin >> a.inicio;
            cout << "Hora de fin (entero): ";
            cin >> a.fin;
            actividades.push_back(a);
            ordenadas = false;

        } else if (opcion == 3) {
            mostrarActividades(actividades);

        } else if (opcion == 4) {
            if (actividades.empty()) {
                cout << "No hay actividades cargadas.\n";
            } else {
                auto t1 = high_resolution_clock::now();
                mergeSort(actividades, 0, (int)actividades.size() - 1);
                auto t2 = high_resolution_clock::now();
                ordenadas = true;
                cout << "Ordenado con Merge Sort en "
                     << duration_cast<microseconds>(t2 - t1).count()
                     << " microsegundos.\n";
                mostrarActividades(actividades);
            }

        } else if (opcion == 5) {
            if (actividades.empty()) {
                cout << "No hay actividades cargadas.\n";
            } else {
                auto t1 = high_resolution_clock::now();
                quickSort(actividades, 0, (int)actividades.size() - 1);
                auto t2 = high_resolution_clock::now();
                ordenadas = true;
                cout << "Ordenado con Quick Sort en "
                     << duration_cast<microseconds>(t2 - t1).count()
                     << " microsegundos.\n";
                mostrarActividades(actividades);
            }

        } else if (opcion == 6) {
            if (!ordenadas) {
                cout << "Las actividades deben estar ordenadas por hora de fin antes "
                     << "del Greedy (precondicion de la Tabla 1). Ordena primero "
                     << "con la opcion 4 o 5.\n";
            } else {
                ultimaAgenda = seleccionActividades(actividades);
                cout << "\nAgenda optima (" << ultimaAgenda.size()
                     << " actividades sin cruce de " << actividades.size()
                     << " totales):\n";
                mostrarActividades(ultimaAgenda);
            }

        } else if (opcion == 7) {
            if (ultimaAgenda.empty()) {
                cout << "Primero genera una agenda optima (opcion 6).\n";
            } else {
                guardarAgenda("agenda_optima.txt", ultimaAgenda);
            }

        } else if (opcion == 8) {
            compararTiempos(actividades);

        } else if (opcion != 0) {
            cout << "Opcion invalida.\n";
        }

    } while (opcion != 0);

    cout << "Hasta luego.\n";
    return 0;
}
