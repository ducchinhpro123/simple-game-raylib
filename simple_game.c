#include <math.h>
#include <raylib.h>
#include <stdio.h>
// #include <stdlib.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 700
#define BALL_SIZE 40
// #define BALL_VELOCITY 5.0f
#define RECTANGLE_X WINDOW_WIDTH * 0.1
#define RECTANGLE_Y WINDOW_HEIGHT * 0.7
#define RECTANGLE_WIDTH WINDOW_WIDTH * 0.2
#define RECTANGLE_HEIGHT WINDOW_HEIGHT * 0.05

#define RESTITUTION 0.6f
#define GRAVITY 980.0f
#define VELOCITY_THRESOLD 0.5
#define FIXED_TIME_STEP 1.0f / 60.0f
#define VELOCITY_THRESHOLD 5.0f
#define POSITION_THRESHOLD 0.1f

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX3(a, b, c) (MAX(MAX(a, b), c))

#define FPS 60

int is_on_the_ground(float real_ball_y);
void accumulator_loop(float *accumulator, float *ball_velocity, float *ps_y, float *ps_x);
void ball_jumb(float *ball_velocity);

int main()
{
    float ball_velocity = 5.0f;
    /* ps_y and ps_x basically are two points */
    float ps_x = WINDOW_WIDTH / 2.0f;
    float ps_y = WINDOW_HEIGHT / 2.0f;
    float accumulator = 0.0f;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Simple game with raylib");
    SetTargetFPS(FPS);

    while (!WindowShouldClose()) {
        BeginDrawing();

        accumulator_loop(&accumulator, &ball_velocity, &ps_y, &ps_x);

        ClearBackground(BLACK);
        DrawCircle((int)ps_x, (int)ps_y, BALL_SIZE, RED);

        DrawRectangle((int)(RECTANGLE_X), (int)(RECTANGLE_Y), (int)RECTANGLE_WIDTH, (int)RECTANGLE_HEIGHT, YELLOW);

        DrawText(TextFormat("Velocity: %.2f", ball_velocity), 0, 0, 20, YELLOW);

        EndDrawing();
    }
    // Set background color
    CloseWindow();
    return 0;
}

void accumulator_loop(float *accumulator, float *ball_velocity, float *ps_y, float *ps_x)
{
    /*
     *          _
     *   * -> ( * )
     *          -
     *   * means position x or y, so ps_y + BALL_SIZE would be a real ball
     * with vertical only
     * */
    float real_ball_y = *ps_y + BALL_SIZE;
    float real_ball_x = *ps_x + BALL_SIZE;
    float delta_time = GetFrameTime();
    *accumulator += delta_time;

    while (*accumulator >= FIXED_TIME_STEP) {
        *ball_velocity += GRAVITY * FIXED_TIME_STEP;
        *ps_y += *ball_velocity * 0.2f;
        // *ps_y += *ball_velocity * FIXED_TIME_STEP;

        // if the ball hit the bottom of the window
        if (*ps_y + BALL_SIZE > WINDOW_HEIGHT) {
            *ps_y = WINDOW_HEIGHT - BALL_SIZE;  // ensure the ball hits the bottom of the screen
            // ball_velocity *= -RESTITUTION;

            // if velocity is below threshold, stop the ball. Prevent always boucing
            if (fabs(*ball_velocity) > VELOCITY_THRESHOLD) {
                *ball_velocity *= -RESTITUTION;  // apply restitution
            } else {
                *ball_velocity = 0;  // rest on the ground

                if (WINDOW_HEIGHT - (*ps_y + BALL_SIZE) < POSITION_THRESHOLD) {
                    *ps_y = WINDOW_HEIGHT - BALL_SIZE;
                }
            }
        }

        Vector2 ball_position = {*ps_x, *ps_y};
        Rectangle rectangle = {RECTANGLE_X, RECTANGLE_Y, RECTANGLE_WIDTH, RECTANGLE_HEIGHT};

        // check if the ball hits the rectangle
        if (CheckCollisionCircleRec(ball_position, BALL_SIZE, rectangle)) {
            *ps_y = RECTANGLE_Y - BALL_SIZE;  // position the ball on the rectangle
            // *ball_velocity *= -RESTITUTION;   // apply restitution

            // if velocity is below threshold, stop the ball. Prevent always boucing
            if (fabs(*ball_velocity) > VELOCITY_THRESHOLD) {
                // printf("TESTING\n");
                *ball_velocity *= -RESTITUTION;  // apply restitution
            }

            if ((RECTANGLE_Y - real_ball_y) > POSITION_THRESHOLD) {
                // *ball_velocity *= -RESTITUTION;  // apply restitution
                *ps_y = RECTANGLE_Y - BALL_SIZE;
            } else {
                *ball_velocity = 0;

                // if (RECTANGLE_HEIGHT - (*ps_y + BALL_SIZE) < POSITION_THRESHOLD) {
                //     *ps_y = RECTANGLE_HEIGHT - BALL_SIZE;
                // }
            }

            if (IsKeyPressed(KEY_SPACE)) {
                ball_jumb(ball_velocity);
            }
        }

        // if (is_on_the_ground(real_ball_y)) {
        //     ball_velocity = 0;
        //     if (IsKeyPressed(KEY_SPACE)) {
        //         ball_velocity = -20;
        //     }
        // }

        if (IsKeyPressed(KEY_SPACE)) {
            // *ball_velocity = -200;
            ball_jumb(ball_velocity);
        }
        if (IsKeyDown(KEY_RIGHT) && real_ball_x < WINDOW_WIDTH) {
            *ps_x += 10;
        }

        if (IsKeyDown(KEY_LEFT) && real_ball_x > (BALL_SIZE * 2)) {
            *ps_x -= 10;
        }

        *accumulator -= FIXED_TIME_STEP;
    }
}

int is_on_the_ground(float real_ball_y)
{
    if (real_ball_y == WINDOW_HEIGHT) return 1;  // on the ground
    return 0;
}

void ball_jumb(float *ball_velocity)
{
    if (IsKeyPressed(KEY_SPACE)) {
        *ball_velocity = -200;
    }
}
