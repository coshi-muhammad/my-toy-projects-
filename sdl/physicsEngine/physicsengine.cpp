#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <cctype>
#include <algorithm>
#include <cmath>
#include <vector>
#include <random>
#include "circle.hpp"
int frameCount = 0;
int lastTime = 0;
int currentTime;
float fps = 0.0;
const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS; 
using namespace std ;

std::random_device seed;  
std::mt19937 randNumgen(seed()); 
vector <circle>circles;
string randColor(){
        int randNum = randNumgen()%4 +1;
        string Color;
        switch (randNum)
        {
        case 1: Color ="red";
            break;
        case 2: Color ="blue";
            break;
        case 3: Color ="green";
            break;
        case 4: Color ="yellow";
            break;        
        }
        return Color;
}
class textBox {
SDL_Rect box;
TTF_Font* text;
SDL_Surface* textInBox;
SDL_Texture* textInBoxtex;
public:
textBox(int x ,int y,int size,string s,SDL_Renderer* renderer){
int length = s.length();
box = {x,y,length*size/2,2*size};
text = TTF_OpenFont("arial.ttf",size);
textInBox = TTF_RenderText_Solid(text,s.c_str(),{255,255,255});
textInBoxtex = SDL_CreateTextureFromSurface(renderer,textInBox);

}
void drawText(SDL_Renderer* renderer){
    SDL_RenderCopy(renderer,textInBoxtex,NULL,&box);
}
};
void drawingFuncion(vector<circle>& circles,SDL_Renderer* renderer){
    for_each(circles.begin(),circles.end(),[&](auto& circle){
        circle.drawCircle(renderer);
    });
}
void ballColision(vector<circle>& circles){
    int r1;
    int r2;
    int x1;
    int x2;
    int y1;
    int y2;
    double vx1;
    double vx2;
    double vy1;
    double vy2;
    double rvx;
    double rvy;
    double rvn;
    double distance;
    double overlap;
    double unitNormalx;
    double unitNormaly;
    for (int i =0 ;i<circles.size();i++){   
        r1 = circles[i].getRadios();
        x1 = circles[i].getPositionX();
        y1 = circles[i].getPositionY();
        vx1 = circles[i].getVelocityX();
        vy1 = circles[i].getVelocityY();

        for (int j =0 ;j<circles.size();j++){
        if(i==j){
            continue;
        }
        r2 = circles[j].getRadios();
        x2 = circles[j].getPositionX();
        y2 = circles[j].getPositionY();
        vx2 = circles[j].getVelocityX();
        vy2 = circles[j].getVelocityY();
        distance = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
        overlap = r1+r2-distance;
        if(overlap>=0){     
        unitNormalx = (x2-x1)/distance;
        unitNormaly = (y2-y1)/distance;
        x1 -= (overlap*unitNormalx)/2;
        y1 -= (overlap*unitNormaly)/2;
        x2 += (overlap*unitNormalx)/2;
        x2 += (overlap*unitNormaly)/2;
        circles[j].setPosition(x2,y2);
        circles[i].setPosition(x1,y1);
        rvx= vx2-vx1;
        rvy= vy2-vy1;
        rvn = rvx*unitNormalx + rvy*unitNormaly;
        if (rvn>=0){
            continue;
        }
        vx1 +=rvn*unitNormalx;
        vy1 +=rvn*unitNormaly;
        vx2 -=rvn*unitNormalx;
        vy2 -=rvn*unitNormaly;
        }
        circles[i].setvelocity(vx1,vy1);
        circles[j].setvelocity(vx2,vy2);
    }
    }
}
int main(int argc,char *args[]){

   SDL_Init(SDL_INIT_EVERYTHING);
   TTF_Init();
   SDL_Window* window = SDL_CreateWindow("physics engine",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HIGHT,SDL_WINDOW_SHOWN);
   SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
   SDL_Event event;
   SDL_Rect upBorder{0,0,WIDTH,3};
   SDL_Rect rightBorder{WIDTH-3,0,3,HIGHT};
   SDL_Rect downBorder{0,HIGHT-3,WIDTH,HIGHT};
   SDL_Rect leftBorder{0,0,3,HIGHT};
   bool runing = true;
   while(runing){
             Uint32 frameStart = SDL_GetTicks();
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                runing = false;
            }
            if (event.type == SDL_KEYDOWN){
                switch (event.key.keysym.sym)
                {
                case SDLK_a:
                circles.push_back(circle(randNumgen()%WIDTH,randNumgen()%HIGHT,randNumgen()%30,randNumgen()%10,randNumgen()%10,randColor(),renderer));
                    
                    break;
                case SDLK_d:
                circles.pop_back();
                break;
                }
            }
            if(event.type == SDL_MOUSEBUTTONDOWN){
                if(event.button.clicks){
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                circles.push_back(circle(mouseX,mouseY,randNumgen()%30 +21,randNumgen()%25,randNumgen()%25,randColor(),renderer));
                }
            }
        }
        textBox numberOfballs(3,3,24,"the number of balls is: " + to_string(circles.size()),renderer);
        textBox framerate(3,53,24,"fps: " + to_string(static_cast<int>(fps)),renderer);
        ballColision(circles);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer,192,192,192,255);
        SDL_RenderFillRect(renderer,&upBorder);
        SDL_RenderFillRect(renderer,&rightBorder);
        SDL_RenderFillRect(renderer,&downBorder);
        SDL_RenderFillRect(renderer,&leftBorder);
 
        drawingFuncion(circles,renderer);
        numberOfballs.drawText(renderer);
        framerate.drawText(renderer);
        SDL_SetRenderDrawColor(renderer,0,0,0,255);

        SDL_RenderPresent(renderer);
        frameCount++;
        currentTime = SDL_GetTicks();
            if (currentTime - lastTime >= 1000) {
            fps = frameCount / ((currentTime - lastTime) / 1000.0f);  // Calculate FPS
            std::cout << "FPS: " << fps << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    // SDL_Delay(500);
   }

    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
