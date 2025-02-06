#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>
#include <deque>
int main(int argc,char *argv[]){
    SDL_Init(SDL_INIT_EVERYTHING);
    auto window = SDL_CreateWindow("snake game",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,800,0);
    auto renderer = SDL_CreateRenderer(window,-1,0);
    bool running = true;
    SDL_Event e;
    enum direction{
        DOWN,
        UP,
        LEFT,
        RIGHT
    };
    int dir;
    SDL_Rect head{0, 0 ,16,16};
    std::deque<SDL_Rect> bodyque;
    int size = 1;
    std::vector<SDL_Rect> apples;
    apples.emplace_back(SDL_Rect{rand()%40*16,rand()%40*16,16,16});
        while (running){
            while(SDL_PollEvent(&e)){
                if(e.type == SDL_QUIT){
                    running = false;
                }
                if(e.type == SDL_KEYDOWN){
                    switch (e.key.keysym.sym)
                    {
                    case SDLK_s :dir = DOWN; 
                        break;

                    case SDLK_w :dir = UP; 
                        break;
                    case SDLK_a :dir = LEFT; 
                        break;
                    case SDLK_d :dir = RIGHT; 
                        break;
                    }
                }
            }
            
            switch (dir)
            {
            case DOWN:head.y +=16; 
                break;
            
            case UP:head.y -=16; 
                break;
            case RIGHT:head.x +=16; 
                break;
            case LEFT:head.x -=16; 
                break;
            }
        

        std::for_each(apples.begin(),apples.end(),[&](auto& apple){
            if (head.x==apple.x && head.y==apple.y){
                size+=1;
                apples.erase(apples.begin());
                apples.emplace_back(SDL_Rect{rand()%40*16,rand()%40*16,16,16});
            }
        });


        std::for_each(bodyque.begin(),bodyque.end(),[&](auto& bodyPart){
            if (head.x==bodyPart.x && head.y==bodyPart.y){
                size=1;
            }
        });
        if(head.x>800){
            head.x = 0;
        }
        if(head.x<0){
            head.x = 800;
        }
        if(head.y>800){
            head.x = 0;
        }
        if(head.y<0){
            head.y = 800;
        }


        bodyque.push_front(head);
        while(bodyque.size()>size){
            bodyque.pop_back();
        }

        
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        std::for_each(bodyque.begin(),bodyque.end(),[&] (auto& bodyPart){
            SDL_RenderFillRect(renderer,&bodyPart);
        });
        SDL_SetRenderDrawColor(renderer,255,0,0,255);
        std::for_each(apples.begin(),apples.end(),[&] (auto& apple){
            SDL_RenderFillRect(renderer,&apple);
        });
        SDL_RenderFillRect(renderer,&head);


        SDL_RenderPresent(renderer);


        SDL_Delay(150);
    }
    return 0 ;
}