#pragma once
#include <iostream>
#include <vector>

struct point;
struct segment;

class renderer;
class ODESolver;
class mouse;

class cloth {
private:
    std::vector<point*> points;
    std::vector<segment*> sticks;

    ODESolver* _integrator;

    point* _followMouse = nullptr;

    float elasticity = 2.0f;

public:
    cloth(ODESolver* integrator);
    ~cloth();

    void init();
    void initFromFile(const std::string& path);
    void update(mouse* _mouse, int width, int height, float dt = 1);
    void drawAllPoints(renderer* _renderer);
    void drawAllSticks(renderer* _renderer);
};