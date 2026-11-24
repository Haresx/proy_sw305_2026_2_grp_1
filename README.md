# Agenda Óptima — Merge Sort / Quick Sort + Greedy (versión gráfica)

Aplicación en C++ que lleva a la práctica el informe *"Merge Sort, Quick Sort y
Greedy: selección de actividades"*. Ordena actividades por hora de fin (con Merge
Sort o Quick Sort, a elección) y luego aplica el algoritmo voraz para quedarse con
el máximo de actividades que **no se cruzan**.

Ahora corre en una **ventana propia** (sin consola), con un menú minimalista y una
**animación paso a paso** de cada algoritmo, mostrando el **tiempo real** de ejecución.

## Instalar raylib (una sola vez)

La interfaz usa [raylib](https://www.raylib.com/).

- **Windows:** instala el paquete oficial desde la página de raylib (incluye el
  compilador w64devkit), o con MSYS2: `pacman -S mingw-w64-ucrt-x86_64-raylib`.
- **macOS:** `brew install raylib`
- **Linux:** instala `libraylib-dev` si tu distribución lo trae; si no, compílala
  desde su repositorio (instrucciones en la web de raylib).

## Compilar y ejecutar

```bash
make          # compila
make run      # compila y abre la aplicación
```

Si raylib está en una carpeta propia: `make RAYLIB=/ruta/a/raylib` (con `include/` y `lib/`).

Ejecuta el programa **desde esta carpeta**: busca `actividades.txt` y la carpeta
`assets/` (tipografía) junto al ejecutable, y guarda `agenda_optima.txt` ahí mismo.

## Cómo se usa

| Pantalla | Qué hace | Atajo |
|---|---|---|
| **Actividades** | Cargar `actividades.txt`, agregar a mano, ver la lista (con scroll). También genera 30 actividades aleatorias para probar. | `1` |
| **Merge Sort** | Ordena animando división, arreglo auxiliar y mezcla. | `2` |
| **Quick Sort** | Ordena animando pivote, comparaciones e intercambios. | `3` |
| **Agenda óptima** | Greedy sobre la lista ordenada: línea de tiempo, agenda final y guardado en `agenda_optima.txt`. | `4` |
| **Comparar tiempos** | Mide Merge Sort y Quick Sort sobre los datos actuales. | `5` |

En las pantallas de ordenamiento:

- **Ordenar** (`Espacio`) inicia; luego **Pausar / Continuar**.
- **Saltar animación** (`S`) muestra el resultado final al instante.
- **Velocidad** `-` / `+` (de 0.25x a 8x). **Mezclar** desordena la lista para repetir la prueba.
- Pasa el mouse sobre una barra para ver el nombre y el intervalo de la actividad.
- `Esc` vuelve al menú.

### El tiempo mostrado es real, la animación no

Al pulsar *Ordenar*, el tiempo se mide ejecutando **tus** `mergeSort` / `quickSort`
originales (`merge_sort.cpp`, `quick_sort.cpp`, sin modificar). Como con pocas
actividades un solo ordenamiento dura menos que la resolución del reloj, se
ejecuta en lotes y se muestra la **mediana por ordenamiento** (ns, µs, ms o s,
según corresponda). La animación se genera aparte, con copias instrumentadas de
los mismos algoritmos (`pasos_orden.cpp`); por eso pausar, acelerar o saltar
no altera el tiempo. Con más de 500 actividades se ordena sin animar.

Para ver el peor caso de Quick Sort (Tabla 2 del informe): ordena una vez y vuelve
a entrar a *Quick Sort* sin mezclar, así recibe datos ya ordenados.

## Estructura del proyecto

| Archivo | Responsabilidad |
|---|---|
| `Actividad.h`, `merge_sort.*`, `quick_sort.*`, `greedy.*` | **Sin cambios** respecto a la versión de consola. |
| `persistencia.*` | Cargar/guardar. Ahora no imprime en consola: devuelve si funcionó, e ignora líneas inválidas en vez de cerrarse. |
| `medicion.*` | Tiempo real de los algoritmos y formato de unidades. |
| `pasos_orden.*` | Grabación paso a paso (solo para la animación) + conteo de comparaciones y movimientos. |
| `animador.*` | Reproduce la grabación: estado visual, pausa y salto al final. |
| `ui.*` | Tema, tipografía, botones y campos de texto sobre raylib. |
| `app.*`, `vista_*.cpp` | Pantallas: menú, actividades, ordenar, agenda óptima y comparar. |
| `main.cpp` | Ventana y bucle principal. |
| `assets/` | Tipografía Poppins (licencia SIL OFL incluida). |

## Formato de `actividades.txt`

Una actividad por línea: `nombre,inicio,fin`. Sin espacios en el nombre (usa `_`);
inicio y fin son enteros (p. ej. horas en formato 24 h).

```
Reunion_equipo,9,10
Clase_algoritmos,9,11
```

## Posibles extensiones

- Pivote aleatorio o mediana de tres en Quick Sort.
- Elegir cuántas actividades aleatorias generar y generar casos ordenados / inversos / con duplicados.
- Animar también la selección del Greedy.
