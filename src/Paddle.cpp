#include "Paddle.h"
#include <algorithm>

Paddle::Paddle() : x((WINDOW_WIDTH - PADDLE_W) / 2.f) {}

void Paddle::moveLeft(float dt) {
    x -= PADDLE_SPEED * dt;
    clamp();
}

void Paddle::moveRight(float dt) {
    x += PADDLE_SPEED * dt;
    clamp();
}

void Paddle::clamp() {
    x = std::clamp(x, 0.f, (float)WINDOW_WIDTH - width);
}

void Paddle::resize(float delta) {
    float cx = centerX();
    width = std::clamp(width + delta, PADDLE_MIN_W, PADDLE_MAX_W);
    x = cx - width / 2.f;
    clamp();
}

sf::FloatRect Paddle::bounds() const {
    return {x, PADDLE_Y, width, PADDLE_H};
}

void Paddle::draw(sf::RenderWindow& win) const {
    sf::RectangleShape shape({width, PADDLE_H});
    shape.setPosition(x, PADDLE_Y);
    shape.setFillColor({180, 100, 255});   // фиолетовый
    shape.setOutlineColor({220, 180, 255});
    shape.setOutlineThickness(1.f);
    win.draw(shape);
}
