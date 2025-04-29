#include <iostream>
#include <raylib.h>

using namespace std;

Color col_RaisinBlack = Color{55, 50, 62, 255};
Color col_ImperialRed = Color{247, 23, 53, 255};
Color col_HoneyDew = Color{223, 248, 235, 255};
Color col_CaribeanCurrent = Color{15, 113, 115, 255};
Color col_Amber = Color{243, 183, 0, 255};
Color col_OxfordBlue = Color{0, 0, 34, 225};

int playerScore = 0;
int cpuScore = 0;
bool isPaused = false;

class Screen
{
public:
    static constexpr int WIDTH = 1280;
    static constexpr int HEIGHT = 800;
};

class Ball
{
public:
    float RADIUS;
    float speed_x;
    float speed_y;
    float pos_x;
    float pos_y;

    void ResetBall()
    {
        pos_x = Screen::WIDTH / 2;
        pos_y = Screen::HEIGHT / 2;
        int direction_choice[2] = {-1, 1};
        speed_x *= direction_choice[GetRandomValue(0, 1)];
        speed_y *= direction_choice[GetRandomValue(0, 1)];
    }

    void Draw()
    {
        DrawCircle(pos_x, pos_y, RADIUS, WHITE);
    }
    void Update()
    {
        pos_x += speed_x;
        pos_y += speed_y;

        if (pos_y + RADIUS >= Screen::HEIGHT || pos_y <= RADIUS)
        {
            speed_y = -1 * speed_y;
        }
        if (pos_x + RADIUS >= Screen::WIDTH)
        {
            playerScore++;
            ResetBall();
        }
        if (pos_x <= RADIUS)
        {
            cpuScore++;
            ResetBall();
        }
    }

    Ball(float radius, float speed_x, float speed_y, float pos_x, float pos_y)
        : RADIUS(radius), speed_x(speed_x), speed_y(speed_y), pos_x(pos_x), pos_y(pos_y) {}
};

class Paddle
{
protected:
    void RestrictPos()
    {
        if (pos_y <= 0)
        {
            pos_y = 0;
        }
        if (pos_y + HEIGHT >= Screen::HEIGHT)
        {
            pos_y = Screen::HEIGHT - HEIGHT;
        }
    }

public:
    float WIDTH;
    float HEIGHT;
    float pos_x;
    float pos_y;
    float speed;

    void Draw()
    {
        DrawRectangle(pos_x, pos_y, WIDTH, HEIGHT, col_CaribeanCurrent);
    }
    void Update()
    {
        if (IsKeyDown(KEY_DOWN))
        {
            pos_y += speed;
        }
        if (IsKeyDown(KEY_UP))
        {
            pos_y -= speed;
        }
        RestrictPos();
    }
    Paddle(float width, float height, float pos_x, float pos_y, float speed)
        : WIDTH(width), HEIGHT(height), pos_x(pos_x), pos_y(pos_y), speed(speed) {};
};

class CpuPaddle : public Paddle
{
public:
    CpuPaddle(float width, float height, float pos_x, float pos_y, float speed)
        : Paddle(width, height, pos_x, pos_y, speed) {}
    void Draw()
    {
        DrawRectangle(pos_x, pos_y, WIDTH, HEIGHT, col_ImperialRed);
    }
    void Update(int ball_y)
    {
        if (pos_y + HEIGHT / 2 > ball_y)
        {
            pos_y -= speed;
        }
        if (pos_y + HEIGHT / 2 < ball_y)
        {
            pos_y += speed;
        }
        RestrictPos();
    }
};

void DrawUI()
{
    DrawText(TextFormat("Player %i", playerScore), 20, 20, 30, col_HoneyDew);
    DrawText(TextFormat("CPU %i", cpuScore), Screen::WIDTH -20 - MeasureText(TextFormat("CPU %i", cpuScore),30), 20, 30, col_HoneyDew);
    DrawLine(Screen::WIDTH / 2, 0, Screen::WIDTH / 2, Screen::HEIGHT, Color{255, 255, 255, 100});
    DrawCircleLines(Screen::WIDTH / 2, Screen::HEIGHT / 2, 100, Color{255, 255, 255, 100});
    DrawText("Ping Pong Game", Screen::WIDTH / 2 - MeasureText("Ping Pong Game", 25) / 2, 20, 25, col_Amber);
}


void DrawPlayPauseButton()
{
    int buttonWidth = 100;
    int buttonHeight = 40;
    int buttonX = (Screen::WIDTH / 2) - (buttonWidth / 2); 
    int buttonY = 60; 

    Rectangle button = {buttonX, buttonY, buttonWidth, buttonHeight};
    Color buttonColor = isPaused ? col_ImperialRed : col_CaribeanCurrent;
    DrawRectangleRec(button, buttonColor);
    DrawText(isPaused ? "Play" : "Pause", button.x + 10, button.y + 10, 20, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(GetMousePosition(), button))
    {
        isPaused = !isPaused;
    }
}

int main()
{
    InitWindow(Screen::WIDTH, Screen::HEIGHT, "Ping Pong");
    InitAudioDevice();
    SetTargetFPS(60);

    Ball ball(20, 7, 7, Screen::WIDTH / 2, Screen::HEIGHT / 2);
    Paddle player_paddle(20, 130, 20, Screen::HEIGHT / 2, 6);
    CpuPaddle cpu_paddle(20, 130, Screen::WIDTH - 40, Screen::HEIGHT / 2, 5);
    Sound hitSound = LoadSound("hit.wav");

    while (WindowShouldClose() == false)
    {
        BeginDrawing();
        ClearBackground(col_OxfordBlue);

        DrawUI();
        DrawPlayPauseButton();

        if (!isPaused)
        {
            player_paddle.Update();
            ball.Update();
            cpu_paddle.Update(ball.pos_y);

            if (CheckCollisionCircleRec(Vector2{ball.pos_x, ball.pos_y}, ball.RADIUS, {player_paddle.pos_x, player_paddle.pos_y, player_paddle.WIDTH, player_paddle.HEIGHT}))
            {
                ball.speed_x *= -1;
                PlaySound(hitSound);
            }

            if (CheckCollisionCircleRec(Vector2{ball.pos_x, ball.pos_y}, ball.RADIUS, {cpu_paddle.pos_x, cpu_paddle.pos_y, cpu_paddle.WIDTH, cpu_paddle.HEIGHT}))
            {
                ball.speed_x *= -1;
                PlaySound(hitSound);
            }
        }

        player_paddle.Draw();
        ball.Draw();
        cpu_paddle.Draw();

        EndDrawing();
    }

    CloseWindow();
}