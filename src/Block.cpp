#include "Block.h"
#include "Bonus.h"

// ─── NormalBlock ─────────────────────────────────────────────────────────────
std::unique_ptr<Bonus> NormalBlock::hit() {
    --hp_;
    flashFrame = true;
    if (hp_ <= 0) {
        alive = false;
        if (factory_) return factory_(pos);
    }
    return nullptr;
}

sf::Color NormalBlock::hpColor() const {
    switch (hp_) {
        case 1:  return {255, 100, 180};  // розовый
        case 2:  return {200, 80,  255};  // фиолетовый
        case 3:  return {100, 140, 255};  // синий
        default: return {80,  200, 255};  // голубой
    }
}


void NormalBlock::draw(sf::RenderWindow& win) const {
    sf::Color c = flashFrame ? sf::Color::White : hpColor();
    flashFrame = false;
    drawRoundedRect(win, c);

    // Убери старый цикл с точками и вставь это:
    sf::Text text(std::to_string(hp_), blockFont(), 13);
    sf::FloatRect tb = text.getLocalBounds();
    text.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    text.setPosition(pos.x + BLOCK_W / 2.f, pos.y + BLOCK_H / 2.f);
    text.setFillColor(sf::Color::White);
    win.draw(text);
}


// ─── SpeedBlock ──────────────────────────────────────────────────────────────
#include "Constants.h"

// SpeedBlock::hit() returns a special sentinel — we handle the speed-up
// effect by returning a BonusSpeedUp centered at the block, but the block
// itself dies immediately.
std::unique_ptr<Bonus> SpeedBlock::hit() {
    alive = false;
    Vec2 center{pos.x + BLOCK_W / 2.f, pos.y + BLOCK_H / 2.f};
    return std::make_unique<BonusSpeedUp>(center);
}
