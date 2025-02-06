#include <raylib.h>
constexpr int WIDTH = 1000;
constexpr int HIEGHT= 800;
int player_score = 0;
int cpu_score =0;
Color green ={38,185,154,255};
Color darkGreen = {20,160,133,255};
Color lightgreen = {129,204,184,255};
Color yellow = {243,213,91,255};



class Ball{
    public:
    Ball(){
        center_x = WIDTH/2;
        center_y = HIEGHT/2;
        radius = 20;
        speed_x = 7;
        speed_y =-10;
        ball_color = WHITE;
    }
    void drawBall(){
        updatePosition();
        DrawCircle(center_x,center_y,radius,yellow);
        
    }
    void updatePosition(){
       center_x +=speed_x; 
       center_y +=speed_y; 
       //collision with the walls 
       if(center_x+radius>=WIDTH){
        player_score++;
        resetBall();
       }
       if(center_x-radius<=0){
        cpu_score++;
        resetBall();
       }
       if(center_y+radius>=HIEGHT){
        center_y = HIEGHT-radius;
        speed_y = -speed_y;
       }
       if(center_y-radius<=0){
        center_y = radius;
        speed_y = -speed_y;
       }
    }
    void resetBall(){
        center_x = WIDTH/2;
        center_y = HIEGHT/2;
        int speed_constant[2] = {-1,1};
        speed_x *= speed_constant[ GetRandomValue(0,1)];
        speed_y *= speed_constant[ GetRandomValue(0,1)];
    }

    float center_x;
    float center_y;
    float radius;
    float speed_x;
    float speed_y;
    Color ball_color;

};
class Paddle{
    public:
    Paddle(int X,int Y){
        x = X;
        y = Y;
        w = 26;
        h = 120;
        paddleColor = {255,255,255,255};
        stepSize = 5;

    }
    void drawPaddle(){
        Rectangle rect = {x-w/2,y-h/2,w,h};
        DrawRectangleRounded(rect,0.8,0,paddleColor);
    }
    virtual void updatePaddle(int nY){

        if(IsKeyDown(KEY_W)){
            y -=stepSize;
        }
        if(IsKeyDown(KEY_S)){
            y+=stepSize;
        }
        capMovment();
   }
    void capMovment(){
         if(y-h/2<0){
            y=h/2;
        }
        if(y+h/2>HIEGHT){
            y = HIEGHT-h/2;
        }
        
    }
    float x,y,w,h,stepSize;
    Color paddleColor;
};
class Rival: public Paddle{
    public:
    Rival(int X,int Y): Paddle(X,Y){

    }
    void updatePaddle(int nY){
        int dy = nY - y;
        if(dy>0){
            y+=stepSize;
        }
        if(dy<0){
            y-=stepSize;
        }
       capMovment(); 
 
    }
};



int main() 
{
    InitWindow(WIDTH,HIEGHT,"pong");
    SetTargetFPS(60);
    Paddle player(35,HIEGHT/2);
    Rival cpu(WIDTH-35,HIEGHT/2);
    Ball ball;
    while(!WindowShouldClose()){
        float x = (player.x-player.w/2);
        float y = (player.y-player.h/2);
        float cx = (cpu.x-cpu.w/2);
        float cy = (cpu.y-cpu.h/2);
        player.updatePaddle(0);
        cpu.updatePaddle(ball.center_y);
        if(CheckCollisionCircleRec({ball.center_x,ball.center_y},
                                ball.radius,Rectangle{x,y,player.w,player.h})){
            ball.speed_x = -ball.speed_x;
        }
        if(CheckCollisionCircleRec({ball.center_x,ball.center_y},
                                ball.radius,Rectangle{cx,cy,cpu.w,cpu.h})){
            ball.speed_x = -ball.speed_x;
        }
        BeginDrawing();
        ClearBackground(darkGreen);
        DrawRectangle(WIDTH/2,0,WIDTH/2,HIEGHT,green);
        DrawCircle(WIDTH/2,HIEGHT/2,150,lightgreen);
        player.drawPaddle();
        cpu.drawPaddle();
        ball.drawBall();
        DrawLine(WIDTH/2,0,WIDTH/2,HIEGHT,WHITE);
        
        DrawText(TextFormat("%i",player_score),WIDTH/4,20,80,WHITE);
        DrawText(TextFormat("%i",cpu_score),WIDTH*3/4,20,80,WHITE);
        EndDrawing();  
    }


    CloseWindow();
    return 0;
}