#define SDL_MAIN_HANDLED
#include <iostream>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <SDL2/SDL.h>
// #include <SDL2/SDL_timer.h>
#include "renderer.h"
#include "ODE_solvers/velocityVerlet.h"

struct point {
    glm::vec2 pos;
    glm::vec2 vel;
    glm::vec2 acc;
    bool locked;
};

struct segment {
    int p, q;
    float len;
};

static void parseData(const std::string& path, std::vector<point>& points, std::vector<segment>& sticks) {
    std::fstream fin(path, std::ios::in);
    int r, c;
    fin >> r >> c;
    for(int y = 0; y < r; y++) {
        for(int x = 0; x < c; x++) {
            glm::vec2 p;
            bool locked;
            fin >> p.x >> p.y >> locked;
            points.push_back({ p, glm::vec2((rand() % 11) * (!locked), 0), glm::vec2(0, 0.1f * (!locked)), locked });
            if(x > 0) {
                glm::vec2 tmp = points[y * c + x - 1].pos;
                sticks.push_back({ y * c + x, y * c + x - 1, glm::length(p - tmp) });
            }
            if(y > 0) {
                glm::vec2 tmp = points[(y - 1) * c + x].pos;
                sticks.push_back({ y * c + x, (y - 1) * c + x, glm::length(p - tmp) });
            }
        }
    }
}

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

inline static void resolveOutOfBounds(point& p, int w, int h) {
    if(p.pos.x > w) {
        p.pos.x = w;
        p.vel.x *= -0.8f;
    }
    if(p.pos.x < 0){
        p.pos.x = 0;
        p.vel.x *= -0.8f;
    }
    if(p.pos.y > h - 10) {
        p.pos.y = h - 10;
        p.vel.y *= -0.8f;
    }
    if(p.pos.y < 0) {
        p.pos.y = 0;
        p.vel.y *= -0.8f;
    }
}

inline static void resolveVelocity(const glm::vec2& p, glm::vec2& v, const int& height) {
    if(abs(v.x) < 0.25f)
        v.x = 0;
    if(abs(v.y) < 0.25f && p.y > height - 10 - 3)
        v.y = 0;
}

inline static void constrainLength(point* p, point* q, const float& len, const float& elasticity) {
    if(p->locked && q->locked)
        return;
    if(q->locked) {
        point* tmp = p;
        p = q;
        q = tmp;
    }
    
    glm::vec2 seg = q->pos - p->pos;
    float actuallen = glm::length(seg);
    if(actuallen > len + elasticity) {
        seg *= (len + elasticity) / actuallen - 1.0f;
        // actuallen = len + elasticity;
    } else if(actuallen < len - elasticity) {
        seg *= (len - elasticity) / actuallen - 1.0f;
        // actuallen = len - elasticity;
    } else {
        // seg *= len / actuallen - 1.0f;
        return;
    }
    // now q needs to go along with seg in order to get the correct position
    if(!p->locked)
        seg *= 0.5f;

    q->pos += seg;
    q->vel += seg;

    if(!p->locked) {
        p->pos -= seg;
        p->vel -= seg;
    }
}

int main(int argv, char** args) {
    int width = 512, height = 512;
    renderer* _renderer = new renderer();
    bool running = _renderer->setup(width, height);

    std::vector<point> points;
    std::vector<segment> sticks;
    parseData("data.txt", points, sticks);

    float elasticity = 4.0f;
    float drag = 0.01f;

    velocityVerlet _integrator = velocityVerlet([=](float t, glm::vec2 y, glm::vec2 z, glm::vec2 zdash) -> glm::vec2 {
        if(y.y > height - 10 - 3 && abs(z.x) > 0)
            zdash.x = -glm::sign(z).x * 0.1f;
        float zlen = glm::length(z);
        zdash -= zlen > 1e-3 ? z * zlen * drag : glm::vec2(0, 0);
        return zdash;
    });

    while(running) {
        handleQuit(running);

        _renderer->clearScreen(0xFF000816);

        for(auto& p : points) {
            _integrator.Integrate(p.pos, p.vel, p.acc, 1);
            // resolveVelocity(p.pos, p.vel, height);
        }

        for(int i = 0; i < 3; i++) {
            for(const auto& stick : sticks) {
                point& p = points[stick.p];
                point& q = points[stick.q];
                constrainLength(&p, &q, stick.len, elasticity);
            }
            for(auto& p : points)
                resolveOutOfBounds(p, width, height);
        }

        // for(const auto& stick : sticks) {
        //     point& p = points[stick.p];
        //     point& q = points[stick.q];
        //     for(int i = 0; i < 3; i++) {
        //         constrainLength(&p, &q, stick.len, elasticity);
        //         resolveOutOfBounds(p, width, height);
        //         resolveOutOfBounds(q, width, height);
        //     }
        // }

        for(const auto& stick : sticks)
            _renderer->drawLine(points[stick.p].pos, points[stick.q].pos, 0xFFFFFFFF);

        _renderer->render();

        // SDL_Delay(1000 / 60);
    }

    std::cout << "Quit program" << std::endl;

    delete _renderer;

    return 0;
}