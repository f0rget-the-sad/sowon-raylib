#include <raylib.h>
#include <raymath.h>


#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define FPS 60
#define DELTA_TIME (1.0f / FPS)
#define CHAR_WIDTH (300 / 2)
#define CHAR_HEIGHT (380 / 2)
#define CHARS_COUNT 8
#define TEXT_WIDTH (CHAR_WIDTH * CHARS_COUNT)
#define TEXT_HEIGHT (CHAR_HEIGHT)
#define WIGGLE_COUNT 3
#define WIGGLE_DURATION (0.40f / WIGGLE_COUNT)
#define COLON_INDEX 10


Texture2D digits = {0};

Vector2
initial_pen(float user_scale, Vector2 *effective_digit)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    float fit_scale;

    float text_aspect_ratio = (float) TEXT_WIDTH / (float) TEXT_HEIGHT;
    float window_aspect_ratio = (float) w / (float) h;
    if(text_aspect_ratio > window_aspect_ratio) {
        fit_scale = (float) w / (float) TEXT_WIDTH;
    } else {
        fit_scale = (float) h / (float) TEXT_HEIGHT;
    }

    effective_digit->x = floorf((float) CHAR_WIDTH * user_scale * fit_scale);
    effective_digit->y = floorf((float) CHAR_HEIGHT * user_scale * fit_scale);

    return (Vector2) {
        .x = w / 2 - effective_digit->x * CHARS_COUNT / 2,
        .y = h / 2 - effective_digit->y / 2,
    };
}

void render_digit_at(int digit_index, int wiggle_index,
                     Vector2 *pen, Vector2 effective_digit)
{
    wiggle_index %= WIGGLE_COUNT;

    Rectangle src_rect = {
        digit_index * CHAR_WIDTH,
        wiggle_index * CHAR_HEIGHT,
        CHAR_WIDTH,
        CHAR_HEIGHT
    };
    Rectangle dst_rect = {
        pen->x,
        pen->y,
        effective_digit.x,
        effective_digit.y
    };
    //DrawTexture(digits, pen->x, pen->y, GREEN);
    // DrawTextureRec(digits, src_rect, *pen, WHITE);
    DrawTexturePro(digits, src_rect, dst_rect, Vector2Zero(), 0, GREEN);

    pen->x += effective_digit.x;
}

void GameFrame() {
    static float wiggle_cooldown = WIGGLE_DURATION;
    static float displayed_time = 0.0f;
    static float user_scale = 1.0f;
    static int wiggle_index = 0;

    BeginDrawing();
    {
        ClearBackground((Color) {0x18, 0x18, 0x18, 0xff});

        Vector2 effective_digit;
        Vector2 pen = initial_pen(user_scale, &effective_digit);

        const long t = (long) ceilf(fmaxf(displayed_time, 0.0f));

        const int hours = t / 60 / 60;
        render_digit_at(hours / 10,  wiggle_index,     &pen, effective_digit);
        render_digit_at(hours % 10,  wiggle_index + 1, &pen, effective_digit);
        render_digit_at(COLON_INDEX, wiggle_index,     &pen, effective_digit);

        const int mins = t / 60 % 60;
        render_digit_at(mins / 10,   wiggle_index + 2, &pen, effective_digit);
        render_digit_at(mins % 10,   wiggle_index + 3, &pen, effective_digit);
        render_digit_at(COLON_INDEX, wiggle_index + 1, &pen, effective_digit);

        const int secs = t % 60;
        render_digit_at(secs / 10,   wiggle_index + 4, &pen, effective_digit);
        render_digit_at(secs % 10,   wiggle_index + 5, &pen, effective_digit);

        displayed_time += DELTA_TIME;

        if (wiggle_cooldown <= 0.0f) {
            wiggle_index++;
            wiggle_cooldown = WIGGLE_DURATION;
        }
        wiggle_cooldown -= DELTA_TIME;
    }
    EndDrawing();
}

int main(void)
{
    InitWindow(TEXT_WIDTH, TEXT_HEIGHT, "sowon");
    SetTargetFPS(FPS);

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    digits = LoadTexture("resources/digits.png");

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(GameFrame, 0, 1);
#else

    while (!WindowShouldClose())
    {
        GameFrame();
    }
#endif

    UnloadTexture(digits);

    CloseWindow();
    return 0;
}
