#include "constants.h"

// std
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define _USE_MATH_DEFINE
#include <math.h>

#include <time.h>

typedef struct player {
    float x;
    float y;
    float height;
    int vel;
} Player;

// global variables
int game_is_running = FALSE;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
Player player1;
Player player2;

struct ball {
    float x;
    float y;
    float velocity;
    float angle;
    float width;
    float height;
    float cos;
    float sin;
} ball;


int initialize_window(void) {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return FALSE;
    }

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );
    
    if(!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return FALSE;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    
    if(!renderer) {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return FALSE;
    }

    return TRUE;
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type) {
        case SDL_QUIT:
            game_is_running = FALSE;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE)
                game_is_running = FALSE;
            
            if(event.key.keysym.sym == SDLK_UP) {
                player2.vel = -1;
            } else if(event.key.keysym.sym == SDLK_DOWN) {
                player2.vel = 1;
            }

            if(event.key.keysym.sym == SDLK_w) {
                player1.vel = -1;
            } else if(event.key.keysym.sym == SDLK_s) {
                player1.vel = 1;
            }
            break;
        case SDL_KEYUP:
            if(event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN) {
                player2.vel = 0;
            }

            if(event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_s) {
                player1.vel = 0;
            }
            break;

    }
}

void setup(void) {
    time_t t1;
    srand((unsigned) time (&t1));

    ball.x = WINDOW_WIDTH/2 - 15;
    ball.y = WINDOW_HEIGHT/2 - 7;
    ball.velocity = 80;
    ball.angle = ((float) rand() / (float) RAND_MAX) * (PI / 2) + (rand() > RAND_MAX/2 ? (3*PI/4) : (-PI/4));
    ball.cos = cos(ball.angle);
    ball.sin = sin(ball.angle);
    ball.width = 15;
    ball.height = 15;
    
    player1.x = 0;
    player1.y = WINDOW_HEIGHT / 2.0f;
    player1.height = 60;
    
    player2.x = WINDOW_WIDTH - 15;
    player2.y = WINDOW_HEIGHT / 2.0f;
    player2.height = 60;
}

void update(void) {
    // TODO:
    //  - ball bounce on walls
    //  - ball bounce on players
    //  - game over

    static int last_frame_time = 0;

    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
    last_frame_time = SDL_GetTicks();

    ball.x += ball.velocity * ball.cos * delta_time;
    ball.y += ball.velocity * ball.sin * delta_time;

    player1.y += player1.vel * PLAYER_VEL * delta_time;
    player2.y += player2.vel * PLAYER_VEL * delta_time;
}

void render_player(Player player) {
    SDL_Rect player_rect = {
        (int) player.x,
        (int) player.y,
        15,
        (int) player.height
    };

    SDL_SetRenderDrawColor(renderer, 200, 200, 50, 255);
    SDL_RenderFillRect(renderer, &player_rect);
}

void render(void) {
    // TODO:
    //  - ball squish
    //  - particles
    //  - ball trace speed

    // render background
    SDL_SetRenderDrawColor(renderer, 80, 15, 50, 255);
    SDL_RenderClear(renderer);

    SDL_Rect middle_rect = {
        ((int) (WINDOW_WIDTH/2)) - 4,
        0,
        8,
        WINDOW_HEIGHT
    };
    SDL_SetRenderDrawColor(renderer, 120, 55, 90, 255);
    SDL_RenderFillRect(renderer, &middle_rect);


    SDL_Rect ball_rect = {
        (int) ball.x,
        (int) ball.y,
        (int) ball.width,
        (int) ball.height
    };

    SDL_SetRenderDrawColor(renderer, 90, 200, 200, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    // render player1
    render_player(player1);
    render_player(player2);

    // buffer swap
    SDL_RenderPresent(renderer);
}

void destroy_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(void) {
    game_is_running = initialize_window();

    setup();

    while(game_is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
}