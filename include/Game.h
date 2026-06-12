#pragma once
#include "Ball.h"
#include "Paddle.h"
#include "Block.h"
#include "Bonus.h"
#include "Constants.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow window_;
    sf::Font         font_;

    Paddle                              paddle_;
    std::vector<Ball>                   balls_;
    std::vector<std::unique_ptr<Block>> blocks_;
    std::vector<std::unique_ptr<Bonus>> bonuses_;

    int  score_   = 0;
    int  defeats_ = 0;
    bool running_ = true;
    bool paused_  = false;

    void buildLevel();
    void handleEvents();
    void update(float dt);
    void draw();

    void updateBalls(float dt);
    void updateBonuses(float dt);

    void resolveBallVsWalls(Ball& ball);
    void resolveBallVsPaddle(Ball& ball);
    void resolveBallVsBalls();
    int  resolveBallVsBlocks(Ball& ball);   // returns score gained

    void spawnBonus(Vec2 pos, std::unique_ptr<Bonus> b);
    void applyBonus(Bonus& b);

    void onMiss();
    void drawHUD();
    bool allBlocksCleared() const;
};
