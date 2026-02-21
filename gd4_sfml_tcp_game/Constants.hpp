#pragma once
#define PLAYER_SPEED 100.f
#define TIME_PER_FRAME 1/60.f

#pragma region Window Info
constexpr unsigned int WINDOW_WIDTH = 1280;
constexpr unsigned int WINDOW_HEIGHT = 720;

// found here: https://stackoverflow.com/questions/27123306/is-it-possible-to-use-stdstring-in-a-constant-expression [24/01/2026]
constexpr std::string_view WINDOW_TITLE = "YES!!!! YOU GUESSED IT... TODAY IS THAT DAY!!!";
#pragma endregion

#pragma region Player Start Positions
constexpr int PLAYER_ONE_START_POSITION_X = 17;
constexpr int PLAYER_ONE_START_POSITION_Y = 256;

constexpr int PLAYER_TWO_START_POSITION_X = 606;
constexpr int PLAYER_TWO_START_POSITION_Y = 256;
#pragma endregion

#pragma region Physics
constexpr float GRAVITY = 0.3f;
#pragma endregion