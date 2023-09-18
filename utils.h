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

void parseData(const std::string& path, std::vector<point*>& points, std::vector<segment*>& sticks);

void resolveOutOfBounds(point& p, int w, int h);

void resolveVelocity(const glm::vec2& p, glm::vec2& v, const int& height);