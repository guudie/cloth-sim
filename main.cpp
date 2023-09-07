#define SDL_MAIN_HANDLED
#include <iostream>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <SDL2/SDL.h>
// #include <SDL2/SDL_timer.h>
#include "renderer.h"
#include "mouse.h"
#include "ODE_solvers/velocityVerlet.h"

struct point {
    glm::vec2 pos;
    glm::vec2 vel;
    glm::vec2 acc;
    bool locked;
};

struct segment {
    point *p_ptr, *q_ptr;
    float len;
};

static void parseData(const std::string& path, std::vector<point*>& points, std::vector<segment*>& sticks) {
    std::fstream fin(path, std::ios::in);
    int r, c;
    fin >> r >> c;
    for(int y = 0; y < r; y++) {
        for(int x = 0; x < c; x++) {
            float cx, cy;
            bool locked;
            fin >> cx >> cy >> locked;
            point* p = new point { glm::vec2(cx, cy), glm::vec2((rand() % 21) * (!locked), 0), glm::vec2(0, 0.5f * (!locked)), locked };
            points.push_back(p);
            if(x > 0) {
                point* tmp = points[y * c + x - 1];
                sticks.push_back(new segment { p, tmp, glm::length(p->pos - tmp->pos) });
            }
            if(y > 0) {
                point* tmp = points[(y - 1) * c + x];
                sticks.push_back(new segment { p, tmp, glm::length(p->pos - tmp->pos) });
            }
        }
    }
}

inline static void handleQuit(bool& running, mouse* _mouse) {
    SDL_Event event;
    int x, y;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE)
                running = false;
            break;
        case SDL_MOUSEMOTION:
            x = event.motion.x;
            y = event.motion.y;
            _mouse->updatePos(x, y);
            break;
        case SDL_MOUSEBUTTONDOWN:
            SDL_GetMouseState(&x, &y);
            _mouse->updatePos(x, y);
            if (!_mouse->getLB() && event.button.button == SDL_BUTTON_LEFT) 
            {
                _mouse->setLB(true);
            }
            if (!_mouse->getRB() && event.button.button == SDL_BUTTON_RIGHT) 
            {
                _mouse->setRB(true);
            }
            break;
        case SDL_MOUSEBUTTONUP: 
            if (_mouse->getLB() && event.button.button == SDL_BUTTON_LEFT)
            {
                _mouse->setLB(false);
            }
            if (_mouse->getRB() && event.button.button == SDL_BUTTON_RIGHT)
            {
                _mouse->setRB(false);
            }
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
        std::swap(p, q);
    }
    
    if(glm::length(q->pos - p->pos) < 1e-3) [[unlikely]] {
        q->pos.x += 1.0f;
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

    mouse* _mouse = new mouse();
    float radiussquared = 400.0f;

    std::vector<point*> points;
    std::vector<segment*> sticks;
    parseData("data.txt", points, sticks);

    float elasticity = 2.0f;
    float drag = 0.01f;
    float dragCap = 10.0f;

    velocityVerlet _integrator = velocityVerlet([=](float t, glm::vec2 y, glm::vec2 z, glm::vec2 zdash) -> glm::vec2 {
        if(y.y > height - 10 - 3 && abs(z.x) > 0)
            zdash.x = -glm::sign(z).x * 0.1f;
        float zlen = glm::length(z);
        if(zlen > 1e-3) {
            z *= (zlen * zlen * drag > dragCap) ? dragCap / zlen : zlen * drag;
            zdash -= z;
        }
        return zdash;
    });

    // point* followMouse = nullptr;

    while(running) {
        handleQuit(running, _mouse);

        _renderer->clearScreen(0xFF000816);

        for(auto& p : points) {
            // if(p == followMouse) {
            //     if(!_mouse->getLB()) {
            //         followMouse = nullptr;
            //         p->locked = false;
            //         p->acc = { 0, 0.5f };
            //     } else {
            //         p->pos = _mouse->getPos();
            //     }
            // }
            if(p->locked)
                continue;
            _integrator.Integrate(p->pos, p->vel, p->acc, 1);
            if(!_mouse->getLB() /* || followMouse != nullptr */)
                continue;
            glm::vec2 tmp = p->pos - _mouse->getPos();
            if(glm::dot(tmp, tmp) < radiussquared) {
                p->pos += _mouse->getDiff();
                p->vel += _mouse->getDiff();
                // p->locked = true;
                // p->vel = { 0, 0 };
                // p->acc = { 0, 0 };
                // followMouse = p;
            }
            // resolveVelocity(p.pos, p.vel, height);
        }

        for(int i = 0; i < 3; i++) {
            for(const auto& stick : sticks) {
                if(stick == nullptr)
                    continue;
                constrainLength(stick->p_ptr, stick->q_ptr, stick->len, elasticity);
            }
            for(auto& p : points)
                resolveOutOfBounds(*p, width, height);
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

        for(auto& stick : sticks) {
            if(stick == nullptr)
                continue;
            if(_mouse->getRB()) [[unlikely]] {
                glm::vec2 tmp = (stick->p_ptr->pos + stick->q_ptr->pos) / 2.0f - _mouse->getPos();
                if(glm::dot(tmp, tmp) < radiussquared / 16.0f) {
                    delete stick;
                    stick = nullptr;
                    continue;
                }
            }
            _renderer->drawLine(stick->p_ptr->pos, stick->q_ptr->pos, 0xFFFFFFFF);
        }

        _renderer->render();

        // SDL_Delay(1000 / 60);
    }

    std::cout << "Quit program" << std::endl;

    for(auto& p : points)
        delete p;
    for(auto& stick : sticks)
        if(stick != nullptr)
            delete stick;
    delete _renderer;
    delete _mouse;

    return 0;
}