#include <math.h>
#include <raylib.h>
// #include <stdio.h>
#include <stdlib.h>
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
#define NUM_RECTANGLES 5

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX3(a, b, c) (MAX(MAX(a, b), c))
#define ARRAY_LEN(arr) sizeof(arr) / sizeof(arr[0])

#define FPS 60

typedef struct {
    float x;
    float y;
    float velocity;
    float size;
} Ball;

int is_on_the_ground(float real_ball_y);
void accumulator_loop(float *accumulator, Ball *ball);
void ball_jumb(float *ball_velocity);
Rectangle *create_rectangles();

int main()
{
    Ball ball;
    float accumulator = 0.0f;

    ball.x = WINDOW_WIDTH / 2.0f;
    ball.y = WINDOW_HEIGHT / 2.0f;
    ball.size = BALL_SIZE;
    ball.velocity = 5.0f;

    Camera2D camera;
    camera.offset = (Vector2){0, 0};
    camera.target = (Vector2){0, 0};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Simple game with raylib");
    SetTargetFPS(FPS);

    while (!WindowShouldClose()) {
        BeginDrawing();
        BeginMode2D(camera);

        accumulator_loop(&accumulator, &ball);

        ClearBackground(BLACK);
        DrawCircle((int)ball.x, (int)ball.y, ball.size, YELLOW);

        // camera.target = (Vector2){ball.x, ball.y};

        if (ball.x > WINDOW_WIDTH * 0.1) {
            camera.offset.x = -(ball.x - WINDOW_WIDTH * 0.1);
        } else if (ball.x < WINDOW_WIDTH * 0.05) {
            camera.offset.x = -(ball.x - WINDOW_WIDTH * 0.05);
        }
        if (camera.offset.x > 0) {
            camera.offset.x = 0;
        }
        // else {
        //
        //     camera.target.x = 0;
        // }

        DrawText(TextFormat("Velocity: %.2f", ball.velocity), 0, 0, 20, YELLOW);
        // DrawText(TextFormat("ball_x: %.2f", ball_ps_x), 0, 20, 20, YELLOW);
        // DrawText(TextFormat("ball_y: %.2f", ball_ps_y), 0, 40, 20, YELLOW);
        // DrawFPS(10, 10);

        EndMode2D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void accumulator_loop(float *accumulator, Ball *ball)
{
    /*
     *          _
     *   * -> ( * )
     *          -
     *   * means position x or y, so ball_ps_y + BALL_SIZE would be a real ball
     * with vertical only
     * */
    float real_ball_y = ball->y + ball->size;
    float real_ball_x = ball->x + ball->size;
    float delta_time = GetFrameTime();
    *accumulator += delta_time;

    Rectangle *rectangles = create_rectangles();

    while (*accumulator >= FIXED_TIME_STEP) {
        ball->velocity += GRAVITY * FIXED_TIME_STEP;
        ball->y += ball->velocity * 0.2f;
        if (ball->y + ball->size > WINDOW_HEIGHT) {
            ball->y = WINDOW_HEIGHT - ball->size;  // ensure the ball->hits the bottom of the screen
            if (fabs(ball->velocity) > VELOCITY_THRESHOLD) {
                ball->velocity *= -RESTITUTION;  // apply restitution
            } else {
                ball->velocity = 0;  // rest on the ground

                if (WINDOW_HEIGHT - (ball->y + ball->size) < POSITION_THRESHOLD) {
                    ball->y = WINDOW_HEIGHT - ball->size;
                }
            }
        }

        for (int i = 0; i < NUM_RECTANGLES; i++) {
            Vector2 ball_position = {ball->x, ball->y};
            Rectangle rectangle = rectangles[i];

            DrawRectangle(rectangle.x, rectangle.y, RECTANGLE_WIDTH, RECTANGLE_HEIGHT, RED);

            // check if the ball->hits the rectangle
            if (CheckCollisionCircleRec(ball_position, ball->size, rectangle)) {
                ball->y = rectangle.y - ball->size;  // position the ball->on the rectangle
                {
                    if (fabs(ball->velocity) > VELOCITY_THRESHOLD) {
                        ball->velocity *= -RESTITUTION;  // apply restitution
                    }

                    if ((RECTANGLE_Y - real_ball_y) > POSITION_THRESHOLD) {
                        // ball->velocity *= -RESTITUTION;  // apply restitution
                        ball->y = rectangle.y - ball->size;
                    } else {
                        ball->velocity = 0;
                    }
                }
            }
        }

        if (IsKeyPressed(KEY_SPACE)) {
            // *ball_velocity = -200;
            ball_jumb(&ball->velocity);
        }
        if (IsKeyDown(KEY_RIGHT) && real_ball_x < WINDOW_WIDTH) {
            ball->x += 10;
        }

        if (IsKeyDown(KEY_LEFT) && real_ball_x > (BALL_SIZE * 2)) {
            ball->x -= 10;
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
        *ball_velocity = -200.0f;
    }
}

// float random_float(float min, float max)
// {
//     float random = ((float)rand()) / (float)RAND_MAX;
//     float diff = max - min;
//     float r = random * diff;
//     return min + r;
// }

Rectangle *create_rectangles()
{
    Rectangle *rectangles = malloc(NUM_RECTANGLES * sizeof(Rectangle));
    rectangles[0] = (Rectangle){WINDOW_WIDTH * 0.1, WINDOW_HEIGHT * 0.7, WINDOW_WIDTH * 0.2, WINDOW_HEIGHT * 0.05};
    rectangles[1] = (Rectangle){WINDOW_WIDTH * 0.4, WINDOW_HEIGHT * 0.5, WINDOW_WIDTH * 0.2, WINDOW_HEIGHT * 0.05};
    rectangles[2] = (Rectangle){WINDOW_WIDTH * 0.7, WINDOW_HEIGHT * 0.3, WINDOW_WIDTH * 0.2, WINDOW_HEIGHT * 0.05};
    rectangles[3] = (Rectangle){WINDOW_WIDTH * 0.9, WINDOW_HEIGHT * 0.7, WINDOW_WIDTH * 0.2, WINDOW_HEIGHT * 0.05};
    rectangles[4] = (Rectangle){WINDOW_WIDTH * 1.1, WINDOW_HEIGHT * 0.3, WINDOW_WIDTH * 0.2, WINDOW_HEIGHT * 0.05};

    return rectangles;
}
