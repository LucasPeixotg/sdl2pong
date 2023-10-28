#include "constants.h"
#include "gametime.h"

// std
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include <math.h>
#include <time.h>

typedef struct player {
    float x;
    float y;
    int vel;
} Player;

extern int GAME_PAUSED;

// global variables
int game_is_running = FALSE;
int game_is_paused = FALSE;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
Player player1;
Player player2;

struct score {
    int player1;
    int player2;
} score;

struct ball {
    float x;
    float y;
    float vx;
    float vy;
    float width;
    float height;
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
            if(event.key.keysym.sym == SDLK_p){
                if(GAME_PAUSED) unpause();
                else pause();
            }
            
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
    ball.width = BALL_RADIUS * 2;
    ball.height = BALL_RADIUS * 2;
    //float angle = ((float) rand() / (float) RAND_MAX) * (PI / 2) + (rand() > RAND_MAX/2 ? (3*PI/4) : (-PI/4));
    ball.vx = BALL_INITIAL_VELOCITY;//cos(angle) * BALL_INITIAL_VELOCITY;
    ball.vy = 0;//sin(angle) * BALL_INITIAL_VELOCITY;
    
    player1.x = 0;
    player1.y = WINDOW_HEIGHT / 2.0f - PAD_HEIGHT/2.0f;
    
    player2.x = WINDOW_WIDTH - PAD_WIDTH;
    player2.y = WINDOW_HEIGHT / 2.0f - PAD_HEIGHT/2.0f;
    
    score.player1 = 0;
    score.player2 = 0;
}

void reset(void) {
    ball.x = WINDOW_WIDTH/2 - 15;
    ball.y = WINDOW_HEIGHT/2 - 7;
    ball.width = 15;
    ball.height = 15;
    float angle = ((float) rand() / (float) RAND_MAX) * (PI / 2) + (rand() > RAND_MAX/2 ? (3*PI/4) : (-PI/4));
    ball.vx = cos(angle) * BALL_INITIAL_VELOCITY;
    ball.vy = sin(angle) * BALL_INITIAL_VELOCITY;
}

float get_ball_velocity(void) {
    return sqrt(pow(ball.vx, 2) + pow(ball.vy, 2));
}

void squish_ball(void) {
    float ball_velocity = get_ball_velocity();
    ball.width = BALL_RADIUS*1.5 + abs((int) (BALL_RADIUS/2 * (ball.vx / ball_velocity))) * (ball_velocity/BALL_MAX_SPEED);
    ball.height =BALL_RADIUS*1.5 + abs((int) (BALL_RADIUS/2 * (ball.vy / ball_velocity))) * (ball_velocity/BALL_MAX_SPEED);
}

void accelerate_ball(int horizontal, int vertical) {
    // TODO:
    //  - add a bit of randomization to the movement
    if(horizontal) {
        if(get_ball_velocity() < BALL_MAX_SPEED) ball.vx *= BALL_ACCELERATION;
    }
    if(vertical) {
        if(get_ball_velocity() < BALL_MAX_SPEED) ball.vy *= BALL_ACCELERATION;
    }

}

void update(void) {
    // TODO:
    //  - make colision detection a priori instead of a posteriori
    //      as it is now and have some problems 

    float delta_time = get_delta_time();

    ball.x += ball.vx * delta_time;
    if(ball.x + ball.width >= WINDOW_WIDTH - PAD_WIDTH) {
        if(ball.y + ball.height >= player2.y && ball.y <= player2.y + PAD_HEIGHT) {
            ball.x = WINDOW_WIDTH - PAD_WIDTH - ball.width;
            ball.vx *= -1;
            
            accelerate_ball(TRUE, FALSE);
        } else {
            score.player1 += 1;
            reset();
        }
    } else if(ball.x <= PAD_WIDTH) {
        if(ball.y + ball.height >= player1.y && ball.y <= player1.y + PAD_HEIGHT) {
            ball.x = PAD_WIDTH;
            ball.vx *= -1;
            
            accelerate_ball(TRUE, FALSE);
        } else {
            score.player2 += 1;
            reset();
        }
    }
    
    ball.y += ball.vy * delta_time;
    if(ball.y + ball.height >= WINDOW_HEIGHT) {
        ball.y = WINDOW_HEIGHT - ball.height;
        ball.vy *= -1;
        
        accelerate_ball(FALSE, TRUE);
    } else if(ball.y <= 0) {
        ball.y = 0;
        ball.vy *= -1;
        
        accelerate_ball(FALSE, TRUE);
    }

    squish_ball();

    player1.y += player1.vel * PAD_VEL * delta_time;
    if(player1.y + PAD_HEIGHT > WINDOW_HEIGHT) player1.y = WINDOW_HEIGHT - PAD_HEIGHT;
    else if(player1.y < 0) player1.y = 0;

    player2.y += player2.vel * PAD_VEL * delta_time;
    if(player2.y + PAD_HEIGHT > WINDOW_HEIGHT) player2.y = WINDOW_HEIGHT - PAD_HEIGHT;
    else if(player2.y < 0) player2.y = 0;
}

void render_player(Player player) {
    SDL_Rect player_rect = {
        (int) player.x,
        (int) player.y,
        PAD_WIDTH,
        PAD_HEIGHT
    };

    SDL_SetRenderDrawColor(renderer, 141, 91, 151, 255);
    SDL_RenderFillRect(renderer, &player_rect);
}

void render_background(void) {
    SDL_SetRenderDrawColor(renderer, 11, 0, 11, 255);
    SDL_RenderClear(renderer);

    SDL_Rect middle_rect = {
        ((int) (WINDOW_WIDTH/2)) - 4,
        0,
        8,
        WINDOW_HEIGHT
    };
    SDL_SetRenderDrawColor(renderer, 40, 28, 42, 255);
    SDL_RenderFillRect(renderer, &middle_rect);
}

void render(void) {
    // TODO:
    //  - particles
    //  - ball trace speed

    render_background();


    SDL_Rect ball_rect = {
        (int) ball.x,
        (int) ball.y,
        (int) ball.width,
        (int) ball.height
    };
    SDL_SetRenderDrawColor(renderer, 130, 84, 139, 255);
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
        if(!GAME_PAUSED) update();
        render();
    }

    destroy_window();
}