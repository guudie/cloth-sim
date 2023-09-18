#define SDL_MAIN_HANDLED
#include <iostream>
#include <fstream>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include <SDL2/SDL.h>
#include "application.h"
#include "ODE_solvers/velocityVerlet.h"

int main(int argv, char** args) {
    int width = 512, height = 512;

    float drag = 0.01f;
    float dragCap = 10.0f;

    application* app = new application();

    velocityVerlet* _integrator = new velocityVerlet([=](float t, glm::vec2 y, glm::vec2 z, glm::vec2 zdash) -> glm::vec2 {
        if(y.y > height - 10 - 3 && abs(z.x) > 0)
            zdash.x = -glm::sign(z).x * 0.1f;
        float zlen = glm::length(z);
        if(zlen > 1e-3) {
            z *= (zlen * zlen * drag > dragCap) ? dragCap / zlen : zlen * drag;
            zdash -= z;
        }
        return zdash;
    });
    app->setupFromPath(width, height, "data.txt", (ODESolver*)_integrator);

    while(app->isRunning()) {
        app->input();

        try {
            app->update();
        } catch (std::string err) {
            std::cout << err << std::endl;
            break;
        }

        app->render();
    }

    app->destroy();
    delete app;
    delete _integrator;

    std::cout << "Quit program" << std::endl;

    return 0;
}