#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <vector>
#include <random>

using namespace std;
random_device rd;
mt19937 gen(rd());
bernoulli_distribution d(0.5);
int const WIDTH = 800;
int const HIEHT = 800;
int rowNum;
int columNum;
int length;
enum gameState {setUp,prossesing};
class cell{

int x,y,l;
bool isAlive;
    SDL_Rect square;
    public:
    cell(int X,int Y,int L,bool state){
        x =X;
        y =Y;
        l = L;
        isAlive = state;
        square = {x,y,l,l};
    }
    bool getState(){
        return isAlive;
    }
    void switchState(){
        isAlive = !isAlive;
    }
    void drawCell(SDL_Renderer* renderer){
        if (isAlive){
            SDL_SetRenderDrawColor(renderer,255,255,255,255);
        }else{
            SDL_SetRenderDrawColor(renderer,0,0,0,255);
        }
        SDL_RenderFillRect(renderer,&square);
    }
};

vector <vector <cell>>cellGrid;

void CreateTheGrid(vector<vector<cell>>& grid,int Length){
    length = Length;
    columNum = HIEHT/Length;
    rowNum = WIDTH/Length;
    bool state= false;
    for(int i =0;i< columNum;i++ ){
        vector <cell> rows;
        for(int j = 0;j<rowNum;j++){
            state = d(gen);
            rows.emplace_back(j*Length,i*Length,Length,state);
        }
        grid.emplace_back(rows);
    }
}

void drawTheGrid(vector<vector<cell>>& grid,SDL_Renderer* renderer){    
    for(int i = 0;i<grid.size();i++){
        for(int j =0; j<grid[i].size();j++){
            grid[i][j].drawCell(renderer);
        }
    }
}
void updateGrid(vector<vector<cell>>& grid,SDL_Renderer* renderer){
        int cols =grid.size();
        int rows =grid[0].size();
        std::vector<std::vector<bool>> nextState(rows, std::vector<bool>(cols, false));
        int aliveNeighbors = 0;
    for(int i=0;i<cols;i++){
        for(int j=0;j<rows;j++){
            aliveNeighbors=0;
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    // Skip the cell itself
                    if (x == 0 && y == 0) continue;

                    int ni = i + x; // Neighbor row
                    int nj = j + y; // Neighbor column

                    // Check if neighbor is within bounds and is alive
                    if (ni >= 0 && ni < rows && nj >= 0 && nj < cols && grid[ni][nj].getState()) {
                        aliveNeighbors+=1;
                    }
                }
            }

            if (grid[i][j].getState() && (aliveNeighbors < 2 || aliveNeighbors > 3)) {
                nextState[i][j] = false; // Cell dies
            } else if (!grid[i][j].getState() && aliveNeighbors == 3) {
                nextState[i][j] = true; // Cell becomes alive
            } else {
                nextState[i][j] = grid[i][j].getState(); // Cell state stays the same
            }
        }
        
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j].getState() != nextState[i][j]) {
                grid[i][j].switchState(); // Toggle cell state only if changed
            }
            cout<<aliveNeighbors<<endl;
        }
    }
}

class startButton{
    SDL_Rect button;
    SDL_Rect spriteContainer;
    TTF_Font* text;
    SDL_Surface* textButton;
    SDL_Surface* buttonSprite;
    SDL_Texture* textButtonTex;
    SDL_Texture* buttonSpriteTex;
    public:
    startButton(int X,int Y,int W,int H,SDL_Renderer* renderer){
        button = {X,Y,W,H};
        spriteContainer = {X-30,Y-37,W+70,H+70};
        text = TTF_OpenFont("arial.ttf",24);
        textButton = TTF_RenderText_Solid(text,"START",{255,255,255});
        buttonSprite = IMG_Load("rectangle.png");
        buttonSpriteTex = SDL_CreateTextureFromSurface(renderer,buttonSprite);
        textButtonTex = SDL_CreateTextureFromSurface(renderer,textButton);
    }
    void drawButton(SDL_Renderer* renderer){
        SDL_RenderCopy(renderer,buttonSpriteTex,NULL,&spriteContainer);
        SDL_RenderCopy(renderer,textButtonTex,NULL,&button);
    }
    gameState buttonAction(){
        gameState newstate = prossesing;
        return newstate;
    }
    int getX1(){
        return button.x;
    }
    int getY1(){
        return button.y;
    }
    int getX2(){
        return button.w+button.x;
    }
    int getY2(){
        return button.h+button.y;
    }
};
int main(int agrc,char *args[]){
    gameState state = setUp;
    CreateTheGrid(cellGrid,10);
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDL_Window* window = SDL_CreateWindow("Conway's game of life",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WIDTH,HIEHT,SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    SDL_Event event;
    bool running=true;
    startButton start(floor(WIDTH/4),floor(HIEHT*3/4),WIDTH/2,(HIEHT/4)-20,renderer);
    while(running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = false;
            }
            if(state ==setUp && event.type == SDL_MOUSEBUTTONDOWN){
                if(event.button.button == SDL_BUTTON_LEFT){
                        int mouseX,mouseY;
                        SDL_GetMouseState(&mouseX,&mouseY);
                        cellGrid[floor(mouseY/length)][floor(mouseX/length)].switchState();
                        if((mouseX>=start.getX1()&& mouseX<=start.getX2())&&(mouseY>=start.getY1()&&mouseY<=start.getY2())){
                            state = start.buttonAction();
                        }
                }
            }
        }

        SDL_RenderClear(renderer);
        if(state==prossesing){
            updateGrid(cellGrid,renderer);
        }
        drawTheGrid(cellGrid,renderer);
        if (state==setUp){
            start.drawButton(renderer);
        }
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderPresent(renderer);
    }


return 0;
}