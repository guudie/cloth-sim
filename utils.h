#pragma once
#include "glm/glm.hpp"
#include <fstream>
#include <vector>

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

void parseData(const std::string& path, std::vector<point*>& points, std::vector<segment*>& sticks) {
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

inline void resolveOutOfBounds(point& p, int w, int h) {
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

inline void resolveVelocity(const glm::vec2& p, glm::vec2& v, const int& height) {
    if(abs(v.x) < 0.25f)
        v.x = 0;
    if(abs(v.y) < 0.25f && p.y > height - 10 - 3)
        v.y = 0;
    float len = glm::length(v);
    v *= len > 100.0f ? 100.0f / len : 1;
}