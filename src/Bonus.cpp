#include "Bonus.h"
#include "GameState.h"
#include <algorithm>

void BonusPaddleGrow::activate(GameState& gs) {
    gs.paddle.resize(+30.f);
}

void BonusPaddleShrink::activate(GameState& gs) {
    gs.paddle.resize(-25.f);
}

void BonusSpeedUp::activate(GameState& gs) {
    for (auto& b : gs.balls)
        b.setSpeed(b.speed + 50.f);
}

void BonusSpeedDown::activate(GameState& gs) {
    for (auto& b : gs.balls)
        b.setSpeed(b.speed - 50.f);
}

void BonusSticky::activate(GameState& gs) {
    for (auto& b : gs.balls)
        b.sticky = true;
}

void BonusExtraBall::activate(GameState& gs) {
    if (gs.balls.empty()) return;
    const Ball& ref = gs.balls.front();
    // Mirror the first ball's direction on X
    Vec2 dir{-ref.vel.x, ref.vel.y};
    gs.balls.emplace_back(ref.pos, dir.normalized(), ref.speed);
}
