#pragma once
#include "glm/glm.hpp"

class mouse {
private:
    glm::vec2 pos;
    glm::vec2 prevPos;

    float radius;

    bool leftButtonDown;
    bool rightButtonDown;
    bool sideButtonX2Down;

public:
    mouse(float mouseRadius = 20.0f) : pos({0, 0}), prevPos({0, 0}), radius(mouseRadius), leftButtonDown(false), rightButtonDown(false), sideButtonX2Down(false) { }
    ~mouse() = default;

    const glm::vec2& getPos() const;
    const glm::vec2& getPrevPos() const;
    glm::vec2 getDiff() const;
    void updatePos(const glm::vec2& _pos);
    void updatePos(int x, int y);

    float getMouseRadius() const;
    float getMouseRadius2() const;

    bool getLB() const;
    void setLB(bool state);

    bool getRB() const;
    void setRB(bool state);

    bool getSBX2() const;
    void setSBX2(bool state);
};