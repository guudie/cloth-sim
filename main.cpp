#define SDL_MAIN_HANDLED
#include <iostream>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
// #include <SDL2/SDL_timer.h>
#include "renderer.h"
#include "ODE_solvers/velocityVerlet.h"

inline static void handleQuit(bool& running) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                running = false;
                break;
        }
    }
}

inline static void resolveOutOfBounds(glm::vec2& p, glm::vec2& v, int w, int h) {
    if(p.x > w) {
        p.x = w;
        v.x *= -0.85f;
    }
    if(p.x < 0){
        p.x = 0;
        v.x *= -0.85f;
    }
    if(p.y > h - 10) {
        p.y = h - 10;
        v.y *= -0.85f;
    }
    if(p.y < 0)
        p.y = 0;
}

inline static void resolveVelocity(const glm::vec2& p, glm::vec2& v) {
    if(abs(v.x) < 1)
        v.x = 0;
    if(abs(v.y) < 1 && p.y > 512 - 10 - 3)
        v.y = 0;
}

inline static void constrainLength(glm::vec2& p, glm::vec2& q, const float& len) {
    glm::vec2 seg = q - p;
    seg = seg * (len / seg.length() - 1.0f);
    // now q needs to go along with seg in order to get the correct position
    q += 0.5f * seg;
    p -= 0.5f * seg;
}

int main(int argv, char** args) {
    renderer* _renderer = new renderer();
    bool running = _renderer->setup(512, 512);
    glm::vec2 p(100, 100);
    glm::vec2 q(150, 150);
    const float len = glm::length(q - p);

    glm::vec2 accp(0, 0.25f);
    glm::vec2 velp(0, 0);

    glm::vec2 accq(0, 0.25f);
    glm::vec2 velq(0, 0);

    velocityVerlet _integrator = velocityVerlet([](float t, glm::vec2 y, glm::vec2 z, glm::vec2 zdash) -> glm::vec2 {
        if(y.y > 512 - 10 - 3 && abs(z.x) > 0)
            zdash.x = -glm::sign(z).x * 0.1f;
        return zdash;
    });

    while(running) {
        handleQuit(running);

        _renderer->clearScreen(0xFF000816);

        _integrator.Integrate(p, velp, accp, 1);
        resolveOutOfBounds(p, velp, 512, 512);
        resolveVelocity(p, velp);

        _integrator.Integrate(q, velq, accq, 1);
        resolveOutOfBounds(q, velq, 512, 512);
        resolveVelocity(q, velq);

        constrainLength(p, q, len);

        _renderer->drawLine(p, q, 0xFFFFFFFF);
        _renderer->render();

        // SDL_Delay(1000 / 60);
    }

    std::cout << "Quit program" << std::endl;

    delete _renderer;

    return 0;
}