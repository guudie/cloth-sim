#pragma once
#include <SDL2/SDL.h>

class renderer;

class application {
private:
    renderer* _renderer = nullptr;
    // mouse* _mouse = nullptr;
    // cloth* _cloth = nullptr;

    bool _isRunning = false;

    Uint32 lastUpdateTime;

public:
    application() = default;
    ~application() = default;

    bool isRunning() const;

    void setup(int clothWidth, int clothHeight, int clothSpacing);
    void input();
    void update();
    void render() const;
    void destroy();
};