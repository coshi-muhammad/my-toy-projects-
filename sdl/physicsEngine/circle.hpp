#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <cctype>
#include <algorithm>
#include <cmath>
#include <unordered_map>
int const HIGHT = 800;
int const WIDTH = 1000;
float dampaning = 1;
float friction = 0;
using namespace std ;
unordered_map<string, SDL_Texture*> textureCache;
class circle {
    float centerX;
    float centerY;
    float radios;
    float veloX;
    float veloY;
    float accelerationX = 0;
    float accelerationY = 1;
    string color;
    string sprite;
     SDL_Rect container;
    SDL_Texture* tex;
    public:
    circle (float x , float y , float r ,float vx , float vy , string c,SDL_Renderer* renderer){
        centerX = x;
        centerY = y;
        veloX = vx;
        veloY = vy;
        radios = r;
        color = c;
        tex = getTexture(renderer, color);
        
    }
    void drawCircle(SDL_Renderer* renderer){
        SDL_RenderCopy(renderer,tex,nullptr,&container);
        collision();
        updatePosition();
    }
    int getPositionX(){
        return centerX;
    }
    int getPositionY(){
        return centerY;
    }
    float getVelocityX(){
        return veloX;
    }
        float getVelocityY(){
        return veloY;
    }
    int getRadios(){
        return radios;
    }
    void setPosition(int x ,int y){
        centerX = x;
        centerY = y;
    }
    void setvelocity(int vx ,int vy){
        veloX = vx;
        veloY = vy;
    }
private:
    void updatePosition(){
        veloX += accelerationX;
        veloY += accelerationY;
        centerX += veloX;
        centerY += veloY;
                container = {
                        static_cast<int> (centerX-radios),
                        static_cast<int> (centerY-radios),
                        static_cast<int>( 2*radios),
                        static_cast<int> ( 2*radios)};
    }
    void collision(){
    if (centerX - radios <= 0) {
        centerX = radios;
        veloX = -veloX * dampaning;
        applyFriction();
    } else if (centerX + radios >= WIDTH) {
        centerX = WIDTH - radios;
        veloX = -veloX * dampaning;
        applyFriction();
    }

    if (centerY - radios <= 0) {
        centerY = radios; 
        veloY = -veloY * dampaning;
        applyFriction();
    } else if (centerY + radios >= HIGHT) {
        centerY = HIGHT - radios;
        veloY = -veloY * dampaning;
        applyFriction();
    }
    }
    void applyFriction() {
    // Apply friction to the X velocity
    if (veloX != 0) {
        veloX -= (veloX > 0 ? friction : -friction);
    }

    // Apply friction to the Y velocity
    if (veloY != 0) {
        veloY -= (veloY > 0 ? friction : -friction);
    }

    // Stop the object if velocity is very low
    if (fabs(veloX) < 0.1) {
        veloX = 0;
    }
    if (fabs(veloY) < 0.1) {
        veloY = 0;
    }
}
SDL_Texture* getTexture(SDL_Renderer* renderer, const string& color) {
    if (textureCache.find(color) == textureCache.end()) {
        SDL_Surface* surface = IMG_Load(("circle_" + color + ".png").c_str());
        if (!surface) {
            cerr << "Failed to load surface for color " << color << endl;
            return nullptr;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture) {
            cerr << "Failed to create texture for color " << color << endl;
            return nullptr;
        }
        textureCache[color] = texture;
    }
    return textureCache[color];
}
};