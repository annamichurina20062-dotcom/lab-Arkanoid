#pragma once

constexpr int WINDOW_WIDTH  = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr int FPS           = 60;

constexpr int BLOCK_COLS    = 12;
constexpr int BLOCK_ROWS    = 6;
constexpr int BLOCK_W       = 58;
constexpr int BLOCK_H       = 22;
constexpr int BLOCK_PAD     = 4;
constexpr int BLOCK_OFFSET_X = 28;
constexpr int BLOCK_OFFSET_Y = 55;

constexpr float PADDLE_W        = 100.f;
constexpr float PADDLE_H        = 14.f;
constexpr float PADDLE_Y        = 560.f;
constexpr float PADDLE_SPEED    = 420.f;
constexpr float PADDLE_MIN_W    = 40.f;
constexpr float PADDLE_MAX_W    = 200.f;

constexpr float BALL_RADIUS     = 8.f;
constexpr float BALL_SPEED_INIT = 280.f;
constexpr float BALL_SPEED_MAX  = 560.f;
constexpr float BALL_SPEED_MIN  = 180.f;

constexpr float BONUS_FALL_SPEED = 130.f;
constexpr float BONUS_W          = 28.f;
constexpr float BONUS_H          = 14.f;

constexpr int SCORE_PER_HIT    =  1;
constexpr int SCORE_MISS       = -3;
constexpr int MAX_DEFEATS      = 3;
