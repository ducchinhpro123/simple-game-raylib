#include <math.h>
#include <raylib.h>
#include <stdio.h>

#define W_WIDTH 1000
#define W_HEIGHT 700
#define BALL_SIZE 20
#define RESTITUTION 0.8
#define GRAVITY 980.0f
#define VELOCITY_THRESOLD 0.5
#define FIXED_TIME_STEP 1.0f / 90.0f
#define VELOCITY_THRESHOLD 5.0f
#define POSITION_THRESHOLD 0.1f
#define FPS 60

int main()
{
    float ball_velocity = 0;
    float ball_x = W_WIDTH / 2.0f;
    float ball_y = W_HEIGHT / 2.0f;
    float ball_velocity_x = 0;
    float ball_velocity_y = 0;
    float speed = 5.0f;
    float accumulator = 0.0f;

    InitWindow(W_WIDTH, W_HEIGHT, "Make something fun with raylib");
    SetTargetFPS(FPS);

    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();
        accumulator += delta_time;

        while (accumulator >= FIXED_TIME_STEP) {
            // apply gravity to the ball's vertical velocity
            ball_velocity_y += GRAVITY * FIXED_TIME_STEP;
            ball_y += ball_velocity_y * FIXED_TIME_STEP;

            // if the ball hit the bottom of the window
            if (ball_y + BALL_SIZE > W_HEIGHT) {
                // make sure the ball is not override the border of the screen
                ball_y = W_HEIGHT - BALL_SIZE;
                if (fabs(ball_velocity_y) > VELOCITY_THRESHOLD) {
                    ball_velocity_y *= -RESTITUTION;
                } else {
                    ball_velocity_y = 0;
                    // ensure the ball rest in underground
                    if (W_HEIGHT - (ball_y + BALL_SIZE) < POSITION_THRESHOLD) {
                        ball_y = W_HEIGHT - BALL_SIZE;
                    }
                }
            }

            if (IsKeyPressed(KEY_SPACE)) {
                ball_velocity_y = -600;
            }

            if (IsKeyDown(KEY_RIGHT) && ball_x + BALL_SIZE < W_WIDTH) {
                ball_x += speed;
            }

            if (IsKeyDown(KEY_LEFT) && ball_x - BALL_SIZE > 0) {
                ball_x -= speed;
            }

            accumulator -= FIXED_TIME_STEP;
        }
        BeginDrawing();
        ClearBackground(RED);
        DrawCircle((int)ball_x, (int)ball_y, BALL_SIZE, WHITE);
        DrawText(TextFormat("Velocity: %.2f", ball_velocity), 0, 0, 22, BLACK);
        DrawText(TextFormat("FPS: %d", GetFPS()), 0, 25, 22, BLACK);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
