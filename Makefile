CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

OBJS = main.o merge_sort.o quick_sort.o greedy.o persistencia.o

agenda_optima: $(OBJS)
	$(CXX) $(CXXFLAGS) -o agenda_optima $(OBJS)

main.o: main.cpp Actividad.h merge_sort.h quick_sort.h greedy.h persistencia.h
	$(CXX) $(CXXFLAGS) -c main.cpp

merge_sort.o: merge_sort.cpp merge_sort.h Actividad.h
	$(CXX) $(CXXFLAGS) -c merge_sort.cpp

quick_sort.o: quick_sort.cpp quick_sort.h Actividad.h
	$(CXX) $(CXXFLAGS) -c quick_sort.cpp

greedy.o: greedy.cpp greedy.h Actividad.h
	$(CXX) $(CXXFLAGS) -c greedy.cpp

persistencia.o: persistencia.cpp persistencia.h Actividad.h
	$(CXX) $(CXXFLAGS) -c persistencia.cpp

clean:
	rm -f *.o agenda_optima
