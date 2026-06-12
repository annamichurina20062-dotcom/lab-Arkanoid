#include "Ball.h"
#include <algorithm>

Ball::Ball(Vec2 startPos, Vec2 direction, float spd)
    : pos(startPos), speed(spd)
{
    applyDirection(direction);
}

void Ball::applyDirection(Vec2 dir) {
    vel = dir.normalized() * speed;
}

void Ball::setSpeed(float s) {
    speed = std::clamp(s, BALL_SPEED_MIN, BALL_SPEED_MAX);
    vel   = vel.normalized() * speed;
}

void Ball::update(float dt) {
    if (!sticky) pos += vel * dt;
}

sf::FloatRect Ball::bounds() const {
    return {pos.x - BALL_RADIUS, pos.y - BALL_RADIUS,
            BALL_RADIUS * 2.f, BALL_RADIUS * 2.f};
}

void Ball::draw(sf::RenderWindow& win) const {
    sf::CircleShape c(BALL_RADIUS);
    c.setOrigin(BALL_RADIUS, BALL_RADIUS);
    c.setPosition(pos.x, pos.y);
    c.setFillColor({220, 180, 255});
    win.draw(c);
}
