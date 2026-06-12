#include "Game.h"
#include "GameState.h"
#include "Constants.h"
#include <random>
#include <algorithm>
#include <sstream>
#include <iostream>

namespace {
    std::mt19937& rng() {
        static std::mt19937 g(std::random_device{}());
        return g;
    }

    int randInt(int lo, int hi) {
        return std::uniform_int_distribution<int>(lo, hi)(rng());
    }

    // Returns overlap depth on each axis (negative = no overlap)
    bool rectOverlap(const sf::FloatRect& a, const sf::FloatRect& b,
                     float& dx, float& dy)
    {
        dx = std::min(a.left + a.width,  b.left + b.width)  - std::max(a.left, b.left);
        dy = std::min(a.top  + a.height, b.top  + b.height) - std::max(a.top,  b.top);
        return dx > 0.f && dy > 0.f;
    }
}

// ─── Construction ────────────────────────────────────────────────────────────
Game::Game()
    : window_(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Arkanoid",
              sf::Style::Titlebar | sf::Style::Close)
{
    window_.setFramerateLimit(FPS);

    const char* candidates[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        "/usr/share/fonts/TTF/DejaVuSans-Bold.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "C:/Windows/Fonts/arial.ttf",
        "arial.ttf"
    };
    bool loaded = false;
    for (auto* path : candidates) {
        if (font_.loadFromFile(path)) {
            loaded = true;
            break;
        }
    }
    if (!loaded) {
        // Запусти из терминала — увидишь это сообщение
        fprintf(stderr, "ERROR: no font loaded!\n");
    }

    buildLevel();
}




// ─── Level builder ───────────────────────────────────────────────────────────
void Game::buildLevel() {
    balls_.clear();
    blocks_.clear();
    bonuses_.clear();

    // Spawn ball above paddle center
    Vec2 ballStart{(float)WINDOW_WIDTH / 2.f, PADDLE_Y - BALL_RADIUS - 2.f};
    balls_.emplace_back(ballStart, Vec2{0.4f, -1.f});

    // Bonus factories keyed by random choice
    auto makeFactory = [](int choice) -> NormalBlock::BonusFactory {
        switch (choice) {
            case 0: return [](Vec2 p){ return std::make_unique<BonusPaddleGrow>(p);   };
            case 1: return [](Vec2 p){ return std::make_unique<BonusPaddleShrink>(p); };
            case 2: return [](Vec2 p){ return std::make_unique<BonusSpeedDown>(p);    };
            case 3: return [](Vec2 p){ return std::make_unique<BonusSticky>(p);       };
            case 4: return [](Vec2 p){ return std::make_unique<BonusExtraBall>(p);    };
            default: return nullptr;
        }
    };

    for (int row = 0; row < BLOCK_ROWS; ++row) {
        for (int col = 0; col < BLOCK_COLS; ++col) {
            float bx = BLOCK_OFFSET_X + col * (BLOCK_W + BLOCK_PAD);
            float by = BLOCK_OFFSET_Y + row * (BLOCK_H + BLOCK_PAD);
            Vec2  p{bx, by};

            int roll = randInt(0, 9);
            if (roll == 0) {
                blocks_.push_back(std::make_unique<IndestructibleBlock>(p));
            } else if (roll == 1) {
                blocks_.push_back(std::make_unique<SpeedBlock>(p));
            } else {
                int hp = randInt(1, 4);
                NormalBlock::BonusFactory factory = nullptr;
                if (randInt(0, 3) == 0)           // 25% chance of hidden bonus
                    factory = makeFactory(randInt(0, 4));
                blocks_.push_back(std::make_unique<NormalBlock>(p, hp, std::move(factory)));
            }
        }
    }
}

// ─── Main loop ───────────────────────────────────────────────────────────────
void Game::run() {
    sf::Clock clock;
    while (window_.isOpen() && running_) {
        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 0.05f);   // cap spike frames

        handleEvents();
        if (!paused_) update(dt);
        draw();
    }
}

// ─── Events ──────────────────────────────────────────────────────────────────
void Game::handleEvents() {
    sf::Event e{};
    while (window_.pollEvent(e)) {
        if (e.type == sf::Event::Closed)
            window_.close();

        if (e.type == sf::Event::KeyPressed) {
            if (e.key.code == sf::Keyboard::Escape) running_ = false;
            if (e.key.code == sf::Keyboard::P)      paused_  = !paused_;

            // Release sticky ball with Space
            if (e.key.code == sf::Keyboard::Space) {
                for (auto& b : balls_) {
                    if (b.sticky) {
                        b.sticky = false;
                        b.vel = Vec2{0.4f, -1.f}.normalized() * b.speed;
                    }
                }
            }

            // Restart with R when defeated
            if (e.key.code == sf::Keyboard::R && defeats_ >= MAX_DEFEATS) {
                score_   = 0;
                defeats_ = 0;
                paddle_  = Paddle{};
                buildLevel();
            }
        }
    }

    if (!paused_ && defeats_ < MAX_DEFEATS) {
        float dt = 1.f / FPS;   // smooth paddle via held key
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  paddle_.moveLeft(dt);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) paddle_.moveRight(dt);
    }
}

// ─── Update ──────────────────────────────────────────────────────────────────
void Game::update(float dt) {
    if (defeats_ >= MAX_DEFEATS) return;

    updateBalls(dt);
    updateBonuses(dt);

    // Sticky balls follow paddle
    for (auto& b : balls_) {
        if (b.sticky)
            b.pos.x = paddle_.centerX();
    }

    // Remove dead blocks
    blocks_.erase(
        std::remove_if(blocks_.begin(), blocks_.end(),
                       [](const auto& b){ return !b->alive; }),
        blocks_.end());

    if (allBlocksCleared()) buildLevel();
}

void Game::updateBalls(float dt) {
    for (auto& ball : balls_) {
        ball.update(dt);
        resolveBallVsWalls(ball);
        resolveBallVsPaddle(ball);
        score_ += resolveBallVsBlocks(ball);

        if (ball.pos.y > WINDOW_HEIGHT + BALL_RADIUS) {
            ball.active = false;
        }
    }

    resolveBallVsBalls();

    balls_.erase(
        std::remove_if(balls_.begin(), balls_.end(),
                       [](const Ball& b){ return !b.active; }),
        balls_.end());

    if (balls_.empty()) onMiss();
}

void Game::updateBonuses(float dt) {
    for (auto& bon : bonuses_) {
        if (!bon->active) continue;
        bon->update(dt);

        if (bon->bounds().intersects(paddle_.bounds())) {
            applyBonus(*bon);
            bon->active = false;
        }

        if (bon->pos.y > WINDOW_HEIGHT) bon->active = false;
    }

    bonuses_.erase(
        std::remove_if(bonuses_.begin(), bonuses_.end(),
                       [](const auto& b){ return !b->active; }),
        bonuses_.end());
}

// ─── Collision helpers ───────────────────────────────────────────────────────
void Game::resolveBallVsWalls(Ball& ball) {
    if (ball.pos.x - BALL_RADIUS < 0) {
        ball.pos.x = BALL_RADIUS;
        ball.reflectX();
    } else if (ball.pos.x + BALL_RADIUS > WINDOW_WIDTH) {
        ball.pos.x = WINDOW_WIDTH - BALL_RADIUS;
        ball.reflectX();
    }
    if (ball.pos.y - BALL_RADIUS < 0) {
        ball.pos.y = BALL_RADIUS;
        ball.reflectY();
    }
}

void Game::resolveBallVsPaddle(Ball& ball) {
    if (ball.sticky) return;
    auto pb = paddle_.bounds();
    auto bb = ball.bounds();
    float dx, dy;
    if (!rectOverlap(pb, bb, dx, dy)) return;
    if (ball.vel.y < 0) return;     // moving upward → already bouncing away

    // Angle based on hit offset
    float offset  = (ball.pos.x - paddle_.centerX()) / (paddle_.width / 2.f);
    float angle   = offset * 60.f * (3.14159f / 180.f);
    Vec2  dir{ std::sin(angle), -std::cos(angle) };
    ball.vel = dir.normalized() * ball.speed;

    ball.pos.y = PADDLE_Y - BALL_RADIUS - 1.f;
}

int Game::resolveBallVsBlocks(Ball& ball) {
    int gained = 0;
    sf::FloatRect bb = ball.bounds();

    for (auto& block : blocks_) {
        if (!block->alive) continue;
        float dx, dy;
        if (!rectOverlap(block->bounds(), bb, dx, dy)) continue;

        // Reflect along axis of least penetration
        if (dx < dy) ball.reflectX();
        else         ball.reflectY();

        auto bonus = block->hit();
        gained += SCORE_PER_HIT;

        if (bonus) {
            Vec2 spawnPos{block->bounds().left + BLOCK_W / 2.f,
                          block->bounds().top  + BLOCK_H / 2.f};
            bonus->pos = spawnPos;
            bonuses_.push_back(std::move(bonus));
        }
        break;   // one block per frame per ball
    }
    return gained;
}

void Game::resolveBallVsBalls() {
    for (std::size_t i = 0; i < balls_.size(); ++i) {
        for (std::size_t j = i + 1; j < balls_.size(); ++j) {
            Vec2 diff{balls_[j].pos.x - balls_[i].pos.x,
                      balls_[j].pos.y - balls_[i].pos.y};
            float dist = diff.length();
            if (dist < BALL_RADIUS * 2.f && dist > 0.f) {
                // Elastic collision: swap velocity components along collision axis
                Vec2 n = diff.normalized();
                float d1 = balls_[i].vel.x * n.x + balls_[i].vel.y * n.y;
                float d2 = balls_[j].vel.x * n.x + balls_[j].vel.y * n.y;
                balls_[i].vel.x += (d2 - d1) * n.x;
                balls_[i].vel.y += (d2 - d1) * n.y;
                balls_[j].vel.x += (d1 - d2) * n.x;
                balls_[j].vel.y += (d1 - d2) * n.y;
                balls_[i].setSpeed(balls_[i].speed);
                balls_[j].setSpeed(balls_[j].speed);
            }
        }
    }
}

// ─── Bonus dispatch ──────────────────────────────────────────────────────────
void Game::applyBonus(Bonus& b) {
    GameState gs{paddle_, balls_, score_};
    b.activate(gs);
}

// ─── Miss / defeat ───────────────────────────────────────────────────────────
void Game::onMiss() {
    score_ += SCORE_MISS;
    ++defeats_;
    paddle_.resize(-15.f);

    if (defeats_ < MAX_DEFEATS) {
        // Respawn a single ball
        Vec2 start{paddle_.centerX(), PADDLE_Y - BALL_RADIUS - 2.f};
        balls_.emplace_back(start, Vec2{0.4f, -1.f});
    }
}

// ─── Draw ────────────────────────────────────────────────────────────────────
void Game::draw() {
    window_.clear({15, 10, 30});

    for (const auto& bl : blocks_)  bl->draw(window_);
    for (const auto& bo : bonuses_) bo->draw(window_);
    for (const auto& b  : balls_)   b.draw(window_);
    paddle_.draw(window_);

    drawHUD();
    window_.display();
}

void Game::drawHUD() {
    // Градиентная полоса сверху
    sf::VertexArray bar(sf::Quads, 4);
    bar[0] = sf::Vertex({0.f,                    0.f},  {40,  10, 60});
    bar[1] = sf::Vertex({(float)WINDOW_WIDTH,    0.f},  {10,  20, 80});
    bar[2] = sf::Vertex({(float)WINDOW_WIDTH,   35.f},  {20,  10, 50});
    bar[3] = sf::Vertex({0.f,                   35.f},  {30,   5, 40});
    window_.draw(bar);

    // Тонкая светящаяся линия под полосой
    sf::RectangleShape line({(float)WINDOW_WIDTH, 1.5f});
    line.setPosition(0.f, 35.f);
    line.setFillColor({180, 100, 255, 200});
    window_.draw(line);

    auto makeText = [&](const std::string& s, float x, float y, unsigned sz = 16) {
        sf::Text t(s, font_, sz);
        t.setPosition(x, y);
        t.setFillColor(sf::Color::White);
        window_.draw(t);
    };

    // Счёт
    makeText("SCORE", 10.f, 2.f, 11);
    makeText(std::to_string(score_), 10.f, 13.f, 18);

    // Жизни
    std::string missStr = "MISSES";
    makeText(missStr, WINDOW_WIDTH - 100.f, 2.f, 11);
    makeText(std::to_string(defeats_) + " / " + std::to_string(MAX_DEFEATS),
             WINDOW_WIDTH - 100.f, 13.f, 18);

    // Паузa
    if (paused_) {
        sf::RectangleShape overlay({(float)WINDOW_WIDTH, (float)WINDOW_HEIGHT});
        overlay.setFillColor({0, 0, 0, 120});
        window_.draw(overlay);
        makeText("P A U S E D", WINDOW_WIDTH / 2.f - 55.f, WINDOW_HEIGHT / 2.f, 26);
    }

    // Гейм овер
    if (defeats_ >= MAX_DEFEATS) {
    sf::RectangleShape overlay({(float)WINDOW_WIDTH, (float)WINDOW_HEIGHT});
    overlay.setFillColor({0, 0, 0, 150});
    window_.draw(overlay);

    sf::Text t1("G A M E  O V E R", font_, 30);
    t1.setFillColor(sf::Color::White);
    sf::FloatRect r1 = t1.getLocalBounds();
    t1.setOrigin(r1.left + r1.width / 2.f, r1.top + r1.height / 2.f);
    t1.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 20.f);
    window_.draw(t1);

    sf::Text t2("Press R to restart", font_, 18);
    t2.setFillColor(sf::Color::White);
    sf::FloatRect r2 = t2.getLocalBounds();
    t2.setOrigin(r2.left + r2.width / 2.f, r2.top + r2.height / 2.f);
    t2.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f + 20.f);
    window_.draw(t2);
}
}

bool Game::allBlocksCleared() const {
    for (const auto& b : blocks_)
        if (b->alive && dynamic_cast<IndestructibleBlock*>(b.get()) == nullptr)
            return false;
    return true;
}
