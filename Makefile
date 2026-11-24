# Agenda Optima (version grafica con raylib)
#
#   make            compila la aplicacion grafica
#   make run        compila y ejecuta
#   make clean      borra los objetos y el ejecutable
#
# Requiere raylib instalada (ver README.md). Si tu raylib esta en una carpeta
# propia: make RAYLIB=/ruta/a/raylib  (debe tener include/ y lib/)

CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall

# ---- raylib: detectar plataforma --------------------------------------------
ifeq ($(OS),Windows_NT)
    TARGET   = agenda_optima.exe
    LDLIBS   = -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows   # -mwindows: sin ventana de consola
else
    TARGET   = agenda_optima
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        LDLIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    else
        # Linux: pkg-config si esta disponible; si no, las librerias de siempre
        PKG := $(shell pkg-config --exists raylib 2>/dev/null && echo yes)
        ifeq ($(PKG),yes)
            CXXFLAGS += $(shell pkg-config --cflags raylib)
            LDLIBS    = $(shell pkg-config --libs raylib)
        else
            LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
        endif
    endif
endif

ifdef RAYLIB
    CXXFLAGS += -I$(RAYLIB)/include
    LDFLAGS  += -L$(RAYLIB)/lib
endif

OBJS = main.o app.o vista_ordenar.o vista_agenda.o vista_comparar.o ui.o \
       animador.o pasos_orden.o medicion.o \
       merge_sort.o quick_sort.o greedy.o persistencia.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f *.o agenda_optima agenda_optima.exe

# ---- dependencias de cabeceras ---------------------------------------------
main.o:            main.cpp app.h ui.h
app.o:             app.cpp app.h ui.h animador.h medicion.h greedy.h persistencia.h merge_sort.h quick_sort.h Actividad.h
vista_ordenar.o:   vista_ordenar.cpp app.h ui.h animador.h medicion.h merge_sort.h quick_sort.h
vista_agenda.o:    vista_agenda.cpp app.h ui.h greedy.h persistencia.h
vista_comparar.o:  vista_comparar.cpp app.h ui.h medicion.h
ui.o:              ui.cpp ui.h
animador.o:        animador.cpp animador.h pasos_orden.h medicion.h Actividad.h
pasos_orden.o:     pasos_orden.cpp pasos_orden.h Actividad.h
medicion.o:        medicion.cpp medicion.h merge_sort.h quick_sort.h Actividad.h
merge_sort.o:      merge_sort.cpp merge_sort.h Actividad.h
quick_sort.o:      quick_sort.cpp quick_sort.h Actividad.h
greedy.o:          greedy.cpp greedy.h Actividad.h
persistencia.o:    persistencia.cpp persistencia.h Actividad.h

.PHONY: run clean
