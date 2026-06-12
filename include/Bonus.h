#pragma once
#include "Vec2.h"
#include "Constants.h"
#include <SFML/Graphics.hpp>

class Paddle;
class Ball;
class GameState;

// ─── Base Bonus ──────────────────────────────────────────────────────────────
class Bonus {
public:
    Vec2 pos;
    bool active = true;

    explicit Bonus(Vec2 spawnPos) : pos(spawnPos) {}
    virtual ~Bonus() = default;

    virtual void activate(GameState& gs) = 0;
    virtual sf::Color color() const = 0;

    void update(float dt) { pos.y += BONUS_FALL_SPEED * dt; }

    sf::FloatRect bounds() const {
        return {pos.x - BONUS_W / 2.f, pos.y - BONUS_H / 2.f, BONUS_W, BONUS_H};
    }

    void draw(sf::RenderWindow& win) const {
    sf::ConvexShape shape(4);
    shape.setPoint(0, {0.f,        -BONUS_H});   // верх
    shape.setPoint(1, {BONUS_W/2.f, 0.f});       // право
    shape.setPoint(2, {0.f,         BONUS_H});   // низ
    shape.setPoint(3, {-BONUS_W/2.f, 0.f});      // лево
    shape.setOrigin(0.f, 0.f);
    shape.setPosition(pos.x, pos.y);
    shape.setFillColor(color());
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(1.f);
    win.draw(shape);
}
};

// ─── Concrete Bonuses ────────────────────────────────────────────────────────
class BonusPaddleGrow final : public Bonus {
public:
    using Bonus::Bonus;
    void activate(GameState& gs) override;
    sf::Color color() const override { return {180, 255, 220}; }
};

class BonusPaddleShrink final : public Bonus {
public:
    using Bonus::Bonus;
    void activate(GameState& gs) override;
    sf::Color color() const override { return {255, 100, 180}; }
};

class BonusSpeedUp final : public Bonus {
public:
    using Bonus::Bonus;
    void activate(GameState& gs) override;
    sf::Color color() const override { return {255, 80, 180}; }
};

class BonusSpeedDown final : public Bonus {
public:
    using Bonus::Bonus;
    void activate(GameState& gs) override;
    sf::Color color() const override { return {100, 180, 255}; }
};

class BonusSticky final : public Bonus {
public:
    using Bonus::Bonus;
    void activate(GameState& gs) override;
    sf::Color color() const override { return {200, 100, 255}; }
};

class BonusExtraBall final : public Bonus {
public:
    using Bonus::Bonus;
    void activate(GameState& gs) override;
    sf::Color color() const override { return {255, 220, 255}; }
};
