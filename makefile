.PHONY: all app temp clean subdirs

EXT =
WINOPT = 
DEBUGCONSOLE = true
ifeq ($(OS), Windows_NT)
	EXT = .exe
	ifeq ($(DEBUGCONSOLE), true)
		WINOPT = -mconsole
	endif
endif
SUBDIRS = ODE_solvers
ARGS = -O2
GCC = g++

LIBS = `sdl2-config --libs --cflags`
STATICLINK = false
ifeq ($(STATICLINK), true)
	LIBS = `sdl2-config --static-libs --cflags` --static
endif
CFLAGS = $(WINOPT) -O2 -Wall -lm $(LIBS)

all: subdirs utils.o renderer.o mouse.o cloth.o application.o main.o app$(EXT)
clean:
	-rm *.o *.exe; \
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

subdirs:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

utils.o: utils.h utils.cpp
	$(GCC) $(ARGS) -c utils.cpp -o utils.o

renderer.o: renderer.h renderer.cpp
	$(GCC) $(ARGS) -c renderer.cpp -o renderer.o

mouse.o: mouse.h mouse.cpp
	$(GCC) $(ARGS) -c mouse.cpp -o mouse.o

cloth.o: cloth.h cloth.cpp renderer.h mouse.h utils.h ./ODE_solvers/ODESolver.h
	$(GCC) $(ARGS) -c cloth.cpp -o cloth.o

application.o: application.h application.cpp cloth.h renderer.h mouse.h ./ODE_solvers/ODESolver.h
	$(GCC) $(ARGS) -c application.cpp -o application.o

main.o: main.cpp application.h ./ODE_solvers/velocityVerlet.h ./ODE_solvers/ODESolver.h
	$(GCC) $(ARGS) -c main.cpp -o main.o

app$(EXT): main.o utils.o renderer.o mouse.o cloth.o application.o ./ODE_solvers/ode_joined.o
	$(GCC) -o $@ $^ $(CFLAGS)