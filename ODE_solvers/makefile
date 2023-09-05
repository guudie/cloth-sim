.PHONY: all clean

ARGS = -O2
GCC = g++

all: ODESolver.o velocityVerlet.o verlet.o
clean:
	-rm *.o

ODESolver.o: ODESolver.h ODESolver.cpp
	$(GCC) $(ARGS) -c ODESolver.cpp -o ODESolver.o

velocityVerlet.o: ODESolver.o velocityVerlet.h velocityVerlet.cpp
	$(GCC) $(ARGS) -c velocityVerlet.cpp -o velocityVerlet.o

verlet.o: ODESolver.o verlet.h verlet.cpp
	$(GCC) $(ARGS) -c verlet.cpp -o verlet.o