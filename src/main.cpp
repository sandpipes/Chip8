#include <iostream>
#include <SDL2/SDL.h>
#include "chip8.h"

const int WIDTH = 640;
const int HEIGHT = 320;

int DrawRectangle(SDL_Renderer *ren, SDL_Rect *rect, bool white){
    if(white)
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    else
        return 0;
    return SDL_RenderFillRect(ren, rect);
}

int InitializeSDL(SDL_Window *&window, SDL_Renderer *&renderer){
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cout << "SDL_Init Error:" << SDL_GetError() << std::endl;
        return 1;
    }

    window = SDL_CreateWindow("Chip8 Emulator", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    if(!window){
        std::cout << "CreateWindow Error:" << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1,  0);

    if(!renderer){
        SDL_DestroyWindow(window);
        std::cout << "CreateRenderer Error:" << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    return 0;
}

void handleKeyInput(SDL_Event *e, Chip8 *chip8, bool pressed){
    switch(e->key.keysym.sym){
        case SDLK_1:
            chip8->keyPressed(0, pressed);
            break;
        case SDLK_2:
            chip8->keyPressed(1, pressed);
            break;
        case SDLK_3:
            chip8->keyPressed(2, pressed);
            break;
        case SDLK_4:
            chip8->keyPressed(3, pressed);
            break;
        case SDLK_q:
            chip8->keyPressed(4, pressed);
            break;
        case SDLK_w:
            chip8->keyPressed(5, pressed);
            break;
        case SDLK_e:
            chip8->keyPressed(6, pressed);
            break;
        case SDLK_r:
            chip8->keyPressed(7, pressed);
            break;
        case SDLK_a:
            chip8->keyPressed(8, pressed);
            break;
        case SDLK_s:
            chip8->keyPressed(9, pressed);
            break;
        case SDLK_d:
            chip8->keyPressed(10, pressed);
            break;
        case SDLK_f:
            chip8->keyPressed(11, pressed);
            break;
        case SDLK_z:
            chip8->keyPressed(12, pressed);
            break;
        case SDLK_x:
            chip8->keyPressed(13, pressed);
            break;
        case SDLK_c:
            chip8->keyPressed(14, pressed);
            break;
        case SDLK_v:
            chip8->keyPressed(15, pressed);
            break;
    }
}

int main(int argc, char *argv[]){

    if(argc < 2){
        printf("usage: %s <game>\n", argv[0]);
        exit(1);
    }

    SDL_Window *window;
    SDL_Renderer *renderer;

    Chip8 chip8;

    if(InitializeSDL(window, renderer) != 0)
        return 1;

    if(chip8.LoadGame(argv[1]) != 0)
        exit(1);

    bool loop = true;
    while(loop){
        chip8.emulateCycle();

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            bool keypressed = false;
            switch(event.type){
                case SDL_QUIT:
                    loop = false;
                    break;
                case SDL_KEYDOWN:
                    keypressed = true;
                case SDL_KEYUP:
                    handleKeyInput(&event, &chip8, keypressed);
                    break;
                }
            }

        if(chip8.shouldDraw()){
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            Uint8 *p = chip8.getPixel();

            for(int x = 0; x < 64; x++){
                for(int y = 0; y < 32; y++){
                    if(p[x + (y * 64)]){
                        SDL_Rect t {x*10, y*10, 10, 10};
                        DrawRectangle(renderer, &t, p[x + (y * 64)]);
                    } else
                        continue;
                }
            }
                    
            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}