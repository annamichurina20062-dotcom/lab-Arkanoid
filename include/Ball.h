#pragma once
#include "Vec2.h"
#include "Constants.h"
#include <SFML/Graphics.hpp>

class Ball {
public:
    Vec2  pos;
    Vec2  vel;
    float speed   = BALL_SPEED_INIT;
    bool  sticky  = false;   // attached to paddle
    bool  active  = true;

    Ball(Vec2 startPos, Vec2 direction, float spd = BALL_SPEED_INIT);

    void update(float dt);
    void draw(sf::RenderWindow& win) const;

    sf::FloatRect bounds() const;

    void reflectX() { vel.x = -vel.x; }
    void reflectY() { vel.y = -vel.y; }

    void setSpeed(float s);
    void applyDirection(Vec2 dir);
};
