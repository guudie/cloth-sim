#include "cloth.h"
#include "renderer.h"
#include "ODE_solvers/ODESolver.h"
#include "mouse.h"
#include <fstream>
#include <math.h>

static void constrainLength(point* p, point* q, const float& len, const float& elasticity) {
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

cloth::cloth(ODESolver* integrator) {
    assert(integrator != nullptr);
    _integrator = integrator;
}

cloth::~cloth() {
    for(auto& p : points)
        if(p != nullptr)
            delete p;
    
    for(auto& stick : sticks)
        if(stick != nullptr)
            delete stick;
}

void cloth::init() {
    return;
}

void cloth::initFromFile(const std::string& path) {
    std::fstream fin(path, std::ios::in);
    int r, c;
    fin >> r >> c;
    points.reserve(r * c);
    for(int y = 0; y < r; y++) {
        for(int x = 0; x < c; x++) {
            float cx, cy;
            bool locked;
            fin >> cx >> cy >> locked;
            point* p = new point { glm::vec2(cx, cy), glm::vec2((rand() % 21) * (!locked), 0), glm::vec2(0, 0.5f * (!locked)), locked };
            points.emplace_back(p);
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

void cloth::update(mouse* _mouse, int width, int height, float dt) {
    for(auto& p : points) {
        if(p == _followMouse) {
            if(!_mouse->getLB()) {
                _followMouse = nullptr;
                p->locked = false;
                p->vel = _mouse->getDiff();
                p->acc = { 0, 0.5f };
            } else {
                p->pos = _mouse->getPos();
            }
        }

        if(p->locked)
            continue;
        
        _integrator->integrate(p->pos, p->vel, p->acc, dt);
        // resolveVelocity(p->pos, p->vel, height);

        glm::vec2 tmp = p->pos - _mouse->getPos();
        if(glm::dot(tmp, tmp) < _mouse->getMouseRadius2() && _followMouse == nullptr) {
            if(_mouse->getLB()) {
                p->locked = true;
                p->vel = { 0, 0 };
                p->acc = { 0, 0 };
                _followMouse = p;
            } else if(_mouse->getSBX2()) {
                p->pos += _mouse->getDiff();
                p->vel += _mouse->getDiff();
            }
        }
    }

    if(_mouse->getLB() && _followMouse == nullptr) {
        _mouse->setLB(false);
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

    for(auto& stick : sticks) {
        if(stick == nullptr)
            continue;
        if(_mouse->getRB()) [[unlikely]] {
            glm::vec2 tmp = (stick->p_ptr->pos + stick->q_ptr->pos) / 2.0f - _mouse->getPos();
            if(glm::dot(tmp, tmp) < _mouse->getMouseRadius2() / 16.0f) {
                delete stick;
                stick = nullptr;
                continue;
            }
        }
        if(isnan(stick->p_ptr->pos.x) || isnan(stick->p_ptr->pos.y) || isnan(stick->q_ptr->pos.x) || isnan(stick->q_ptr->pos.y)) {
            throw "Error: Nan encountered in position";
        }
    }
}

void cloth::drawAllPoints(renderer* _renderer) {

}

void cloth::drawAllSticks(renderer* _renderer) {
    for(auto& stick : sticks)
        if(stick != nullptr)
            _renderer->drawLine(stick->p_ptr->pos, stick->q_ptr->pos, 0xFFFFFFFF);
}