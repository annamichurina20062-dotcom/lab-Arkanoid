#pragma once
#include "Constants.h"
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include <SFML/Graphics.hpp>

inline sf::Font& blockFont() {
    static sf::Font f;
    static bool loaded = false;
    if (!loaded) {
        f.loadFromFile("C:/Windows/Fonts/arial.ttf");
        loaded = true;
    }
    return f;
}

class Bonus;

// ─── Base Block ──────────────────────────────────────────────────────────────
class Block {
public:
    Vec2 pos;           // top-left
    bool alive = true;

    Block(Vec2 p) : pos(p) {}
    virtual ~Block() = default;

    // Returns bonus (may be null). Decrements hp internally.
    virtual std::unique_ptr<Bonus> hit()  = 0;
    virtual bool                   solid() const { return true; }
    virtual void                   draw(sf::RenderWindow& win) const = 0;

    sf::FloatRect bounds() const {
        return {pos.x, pos.y, (float)BLOCK_W, (float)BLOCK_H};
    }

protected:
    void drawRect(sf::RenderWindow& win, sf::Color fill, sf::Color outline = sf::Color::Black) const {
        sf::RectangleShape r({(float)BLOCK_W, (float)BLOCK_H});
        r.setPosition(pos.x, pos.y);
        r.setFillColor(fill);
        r.setOutlineColor(outline);
        r.setOutlineThickness(1.f);
        win.draw(r);
    }

    // В Block.h замени drawRect на:
void drawRoundedRect(sf::RenderWindow& win, sf::Color fill, float radius = 4.f) const {
    float w = (float)BLOCK_W;
    float h = (float)BLOCK_H;
    float r = radius;

    // Центральный прямоугольник
    sf::RectangleShape center({w - r * 2, h});
    center.setPosition(pos.x + r, pos.y);
    center.setFillColor(fill);
    win.draw(center);

    // Левый и правый столбики
    sf::RectangleShape left({r, h - r * 2});
    left.setPosition(pos.x, pos.y + r);
    left.setFillColor(fill);
    win.draw(left);

    sf::RectangleShape right({r, h - r * 2});
    right.setPosition(pos.x + w - r, pos.y + r);
    right.setFillColor(fill);
    win.draw(right);

    // Четыре угловых кружка
    for (int i = 0; i < 4; ++i) {
        sf::CircleShape corner(r);
        corner.setFillColor(fill);
        corner.setPosition(
            pos.x + (i % 2 == 0 ? 0.f : w - r * 2),
            pos.y + (i < 2      ? 0.f : h - r * 2)
        );
        win.draw(corner);
    }
}

};

// ─── Indestructible ──────────────────────────────────────────────────────────
class IndestructibleBlock final : public Block {
public:
    using Block::Block;
    std::unique_ptr<Bonus> hit() override { return nullptr; }  // never dies
    void draw(sf::RenderWindow& win) const override {
        drawRoundedRect(win, {60, 60, 100});
    }
};

// ─── Normal Block (has hp, optional bonus factory) ───────────────────────────
class NormalBlock : public Block {
public:
    using BonusFactory = std::function<std::unique_ptr<Bonus>(Vec2)>;

    NormalBlock(Vec2 p, int hp, BonusFactory factory = nullptr)
        : Block(p), hp_(hp), factory_(std::move(factory)) {}

    std::unique_ptr<Bonus> hit() override;
    void draw(sf::RenderWindow& win) const override;

    mutable bool flashFrame = false;  // добавь это

private:
    int          hp_;
    BonusFactory factory_;

    sf::Color hpColor() const;
};

// ─── Speed Block (increases ball speed on hit, has hp=1) ────────────────────
class SpeedBlock final : public Block {
public:
    using Block::Block;
    std::unique_ptr<Bonus> hit() override;
    void draw(sf::RenderWindow& win) const override {
        drawRoundedRect(win, {255, 80, 180});
    }
};
