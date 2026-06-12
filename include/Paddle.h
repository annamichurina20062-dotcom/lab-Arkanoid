#pragma once
#include "Constants.h"
#include <SFML/Graphics.hpp>

class Paddle {
public:
    float x;
    float width = PADDLE_W;

    Paddle();

    void moveLeft (float dt);
    void moveRight(float dt);
    void clamp();

    void resize(float delta);
    void draw(sf::RenderWindow& win) const;

    sf::FloatRect bounds() const;
    float centerX() const { return x + width / 2.f; }
};
