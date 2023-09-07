.PHONY: all app temp clean subdirs

SUBDIRS = ODE_solvers
ARGS = -O2
GCC = g++

CFLAGS = -O2 -Wall -mconsole -lm `sdl2-config --libs --cflags`

all: subdirs renderer.o mouse.o main.o app
clean:
	-rm *.o *.exe; \
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

subdirs:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

renderer.o: renderer.h renderer.cpp
	$(GCC) $(ARGS) -c renderer.cpp -o renderer.o

mouse.o: mouse.h mouse.cpp
	$(GCC) $(ARGS) -c mouse.cpp -o mouse.o

main.o: main.cpp application.h renderer.h mouse.h ./ODE_solvers/velocityVerlet.h ./ODE_solvers/ODESolver.h
	$(GCC) $(ARGS) -c main.cpp -o main.o

app: main.o renderer.o mouse.o ./ODE_solvers/ode_joined.o
	$(GCC) -o $@ $^ $(CFLAGS)