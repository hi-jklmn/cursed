#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define N 20
#define COLOR_OFFSET 50
#define PAIR_OFFSET 2

typedef struct {
    int x, y;
} Vec2i;

typedef struct {
    int color_pair;
    char character;
} Pixel;

typedef struct Screen {
    void *screen_data;
    void (*screen_init)(struct Screen* this, Vec2i dimensions);
    void (*screen_render)(struct Screen* this, Pixel* buffer, Vec2i dimensions);
} Screen;

typedef struct {
    Vec2i dimensions;
    char *pattern;
    char *buffer;
} matrix_data;

void matrix_init(Screen* this, Vec2i dimensions) {
    this->screen_data = malloc(sizeof(matrix_data));
    matrix_data* data = this->screen_data;
    data->dimensions = dimensions;
    data->pattern = malloc(dimensions.x * dimensions.y);
    for (int i = 0; i < dimensions.y; i++) {
        for (int j = 0; j < dimensions.x; j++) {
            data->pattern[i * dimensions.x + j] = '!' + rand() % ('~' - '!');
        }
    }
    data->buffer = malloc(dimensions.x * dimensions.y);
    memset(data->buffer, 0, dimensions.x + dimensions.y);
}

void matrix_render(Screen* this, Pixel* buffer, Vec2i dimensions) {
    matrix_data* data = this->screen_data;
    if (memcmp(&dimensions, &data->dimensions, sizeof(Vec2i))) {
        matrix_init(this, dimensions);
        data = this->screen_data;
    }
    for (int x = 0; x < dimensions.x; x++) {
        if (rand() % 20 == 0) {
            data->buffer[x] = N;
        }
    }
    for (int y = dimensions.y-1; y >= 0; y--) {
        for (int x = 0; x < dimensions.x; x++) {
            if (data->buffer[y * dimensions.x + x] = 
                    data->buffer[y * dimensions.x + x] > 0 
                    ? data->buffer[y * dimensions.x + x] - 1 
                    : 0) {
                if (y < dimensions.y - 1) {
                    data->buffer[(y+1) * dimensions.x + x] = 
                        data->buffer[y * dimensions.x + x] + 1;
                } 
            }
        }
    }

    for (int y = 0; y < dimensions.y; y++) {
        for (int x = 0; x < dimensions.x; x++) {
            if (data->buffer[y * dimensions.x + x] == N) {
                buffer[y * dimensions.x + x] = (Pixel) {
                    .color_pair = 1,
                    .character = '!' + rand() % 93,
                };
            } else if (data->buffer[y * dimensions.x + x] > 0) {
                buffer[y * dimensions.x + x] = (Pixel) {
                    .color_pair = PAIR_OFFSET + data->buffer[y * dimensions.x + x],
                    .character = data->pattern[y * dimensions.x + x],
                };
            } else {
                 buffer[y * dimensions.x + x] = (Pixel) {
                    .color_pair = 0,
                    .character = ' ',
                };
            }
        }
    }
}

int main() {
    initscr();
    cbreak();
    noecho();

    start_color();

    curs_set(0);

    timeout(40);

    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    for (int i = 0; i <= N; i++) {
        init_color(COLOR_OFFSET + i, i * 500 / N, 500 + i * 500 / N, i * 500 / N);
        init_pair(PAIR_OFFSET + i, COLOR_OFFSET + i, COLOR_BLACK);
    }

    Vec2i dimensions = (Vec2i) { .x = COLS, .y = LINES };

    Screen matrix = (Screen) {
        .screen_init = &matrix_init,
        .screen_render = &matrix_render,
    };

    matrix.screen_init(&matrix, dimensions);

    for (;;) {
        Pixel buffer[dimensions.x * dimensions.y];

        matrix.screen_render(&matrix, buffer, dimensions);

        for (int y = 0; y < dimensions.y; y++) {
            for (int x = 0; x < dimensions.x; x++) {
                attron(COLOR_PAIR(buffer[y * dimensions.x + x].color_pair));
                mvaddch(y, x, buffer[y * dimensions.x + x].character);
            }
        }

        refresh();

        if(getch() != ERR) break;
    }
    printf("Program finished correctly.");

    endwin();

    return 0;
}
