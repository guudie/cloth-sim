#include "application.h"
#include "renderer.h"

void application::setup(int clothWidth, int clothHeight, int clothSpacing) {
    _renderer = new renderer();
    // _mouse = new mouse();

    _isRunning = _renderer->setup(512, 512);

    // setup cloth thing here...
    //

    lastUpdateTime = SDL_GetTicks();
}

void application::input() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                _isRunning = false;
                break;
        }
    }
}

void application::update() {
    Uint32 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastUpdateTime) / 1000.0f;

    // update cloth thing here
    //

    lastUpdateTime = currentTime;
}

void application::render() const {
    _renderer->clearScreen(0xFF000816);

    // draw cloth here
    //

    _renderer->render();
}

bool application::isRunning() const {
    return _isRunning;
}

void application::destroy() {
    // delete _mouse;
    delete _renderer;
    // delete _cloth;
}