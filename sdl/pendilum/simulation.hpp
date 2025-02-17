#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
struct vec2{
    double x;
    double y;
    vec2 operator+(const vec2& other) const {
        return {x + other.x, y + other.y};
    }

    vec2 operator-(const vec2& other) const {
        return {x - other.x, y - other.y};
    }

    vec2 operator*(double scalar) const {
        return {x * scalar, y * scalar};
    }
};
double gravity=1;
double dt = 0.02;
class Body{
    protected:
    vec2 position;
    vec2 prevPosition;
    vec2 nextPosition;
    vec2 velocity;
    vec2 accelaration;
    vec2 nextAccelaration;
    int width;
    int height;
    public:
    Body( int W, int H, vec2 P,vec2 V,vec2 A){
        position = P;
        velocity = V;
        accelaration = A;
        width = W;
        height = H;
    }
    void setPosition(vec2 P){
        position = P;
    }
    void setVelocity(vec2 V){
        velocity = V;
    }
    void setAccelaration(vec2 A){
        accelaration = A;
    }
    vec2 getPosition(){
        return position;
    }
    vec2 getVelosity(){
        return velocity;
    }
    vec2 getAcceleration(){
        return accelaration;
    }
    virtual void drawBody(SDL_Renderer* renderer) =0;
    virtual void updatePosition(){
        prevPosition = position - velocity;
        nextPosition = position*2 - prevPosition + accelaration*0.01;
        prevPosition = position;
        position = nextPosition;
    };
};
class Line:public Body{
    vec2 position1;
    vec2 position2;
    double angle;
    double anglerVelocity = 0;
    double anglerAcceleration = 0;
    public:
    Line(int W, int H, vec2 P,vec2 V,vec2 A,double An):Body(W,H,P,V,A){
        position1 = position;
        position2 = {(position.x)+width,position.y};
        angle = An;
    }
    void drawBody(SDL_Renderer* renderer){
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderDrawLine(renderer,position1.x,position1.y,position2.x,position2.y);
    }
    void findAnglerAcceleration(){
        anglerAcceleration = -(gravity/width)*(std::sin(angle));
    }
    void updateAngle(){
        anglerVelocity += anglerAcceleration*dt;
        angle += anglerVelocity*dt;
    }
    vec2 findAceleration(){
        vec2 A = {-width*anglerAcceleration*(std::cos(angle)),-width*anglerAcceleration*(std::sin(angle))};
        return A;
    }
    void updatePosition(){
        findAnglerAcceleration();
        updateAngle();
        position2.x = position1.x + width * std::sin(angle);
        position2.y = position1.y + width * std::cos(angle);
    };
    vec2 getPosition2(){
        return position2;
    }
    void setPosition1(vec2 P){
        position1 = P;
    }
};
class Ball:public Body{
    vec2 center;
    int radios;
    SDL_Surface* S;
    SDL_Texture* T;
    SDL_Rect container;
    public:
    Ball(int R, vec2 P,vec2 V,vec2 A,SDL_Renderer* renderer):Body(2*R,2*R,P,V,A){
        radios = R;
        center = {position.x+R,position.y+R};
        S = IMG_Load("circle_red.png");
        T = SDL_CreateTextureFromSurface(renderer,S);
        container = {static_cast<int>(center.x-R),static_cast<int>(center.y-R),width,height};
    }
    void drawBody(SDL_Renderer* renderer){
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderCopy(renderer,T,nullptr,&container);
    }
    void setCenterPosition(vec2 np){
        center = np;
    }
    void updatePosition(){
        container.x = center.x-radios;
        container.y = center.y-radios;
    }
};
vec2 initPosition = {400,300};
vec2 initVelocity = {0,0};
vec2 initAcceleration = {0,0};
Line String1(100,20,initPosition,initVelocity,initAcceleration,M_PI/2);
class Simulation{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    bool runing =true;
    public:
    Simulation(std::string simulationTitle,const int width,const int height){
        SDL_Init(SDL_INIT_EVERYTHING);
        TTF_Init();
        IMG_Init(IMG_INIT_PNG);
        window = SDL_CreateWindow(simulationTitle.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    }
    void runSimulation(){
    Ball ball1(20,initPosition,initVelocity,initAcceleration,renderer);
        while(runing){
            while (SDL_PollEvent(&event))
            {
                if(event.type == SDL_QUIT){
                    runing = false;
                }
            }
            SDL_RenderClear(renderer);
            String1.updatePosition();
            String1.drawBody(renderer);
            ball1.setCenterPosition(String1.getPosition2());
            ball1.updatePosition();
            ball1.drawBody(renderer);
            SDL_SetRenderDrawColor(renderer,0,0,0,255);
            SDL_RenderPresent(renderer);
            
        }
    }
    void destructSimulation(){
        IMG_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};