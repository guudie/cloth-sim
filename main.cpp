#define SDL_MAIN_HANDLED
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
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
    if(abs(v.x) < 0.3f)
        v.x = 0;
    if(abs(v.y) < 0.3f && p.y > 512 - 10 - 3)
        v.y = 0;
}

inline static void constrainLength(glm::vec2& p, glm::vec2& q, glm::vec2& vp, glm::vec2& vq, const float& len) {
    glm::vec2 seg = q - p;
    seg = seg * (len / glm::length(seg) - 1.0f);
    // now q needs to go along with seg in order to get the correct position
    seg *= 0.5f;
    q += seg;
    vq += seg;
    p -= seg;
    vp -= seg;
}

int main(int argv, char** args) {
    renderer* _renderer = new renderer();
    bool running = _renderer->setup(512, 512);
    glm::vec2 p(100, 100);
    glm::vec2 q(150, 150);
    glm::vec2 r(150, 100);
    const float lpq = glm::length(q - p);
    const float lqr = glm::length(r - q);
    const float lpr = glm::length(r - p);

    glm::vec2 accp(0, 0.25f);
    glm::vec2 velp(5, -5);

    glm::vec2 accq(0, 0.25f);
    glm::vec2 velq(-5, 5);

    glm::vec2 accr(0, 0.25f);
    glm::vec2 velr(5, 5);

    velocityVerlet _integrator = velocityVerlet([](float t, glm::vec2 y, glm::vec2 z, glm::vec2 zdash) -> glm::vec2 {
        if(y.y > 512 - 10 - 3 && abs(z.x) > 0)
            zdash.x = -glm::sign(z).x * 0.1f;
        return zdash;
    });

    while(running) {
        handleQuit(running);

        _renderer->clearScreen(0xFF000816);

        _integrator.Integrate(p, velp, accp, 1);
        // resolveVelocity(p, velp);

        _integrator.Integrate(q, velq, accq, 1);
        // resolveVelocity(q, velq);

        _integrator.Integrate(r, velr, accr, 1);
        // resolveVelocity(r, velr);

        for(int i = 0; i < 3; i++) {
            constrainLength(p, q, velp, velq, lpq);
            constrainLength(q, r, velq, velr, lqr);
            constrainLength(p, r, velp, velr, lpr);

            resolveOutOfBounds(p, velp, 512, 512);
            resolveOutOfBounds(q, velq, 512, 512);
            resolveOutOfBounds(r, velr, 512, 512);
        }

        _renderer->drawLine(p, q, 0xFFFFFFFF);
        _renderer->drawLine(q, r, 0xFFFFFFFF);
        _renderer->drawLine(p, r, 0xFFFFFFFF);
        _renderer->render();

        // SDL_Delay(1000 / 60);
    }

    std::cout << "Quit program" << std::endl;

    delete _renderer;

    return 0;
}