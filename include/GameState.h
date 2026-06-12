#pragma once
#include "Ball.h"
#include "Paddle.h"
#include <vector>
#include <memory>

struct GameState {
    Paddle&                            paddle;
    std::vector<Ball>&                 balls;
    int&                               score;
};
