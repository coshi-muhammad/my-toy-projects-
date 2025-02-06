#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
const int WIDTH = 800;
const int HEIGHT = 800; 
class Screen
{
    SDL_Event e;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<SDL_FPoint> points;
public:
    Screen(){
        SDL_Init(SDL_INIT_VIDEO);
        SDL_CreateWindowAndRenderer(WIDTH,HEIGHT,0,&window,&renderer);
        SDL_RenderSetScale(renderer,1,1);
    }
    void pixelPlacer(float x, float y){
        points.push_back(SDL_FPoint{x,y});
    }
    void pixelclear(){
        points.clear();
    }
    void showPixels(){
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        for(auto& point : points){
            SDL_RenderDrawPointF(renderer,point.x,point.y);
        }
        SDL_RenderPresent(renderer);
    }
    void events(){
        while(SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                SDL_Quit();
                exit(0);
            }
        }
    };
};