#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raymath.h>
#include <raygui.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
// this is my first refactoring experience make it just works then think about making it look nice and maybe show it on a video 
//constants 
const int WIDTH = 1280;
const int HEIGHT = 720;
const float DISTANCE_CONSTANT= sqrtf(3)/3;
const float SIDE_LENGTH = 100;
const float DT = 1.0f/60.0f;
const float DAMP = 0.99f;
const float MAX_VELOCITY = 100;
int frames_since_start = 0;
int milli_second_since_start;
int time_of_death;
int time_since_death = 0;
bool died_now = false;
bool finished_typing = false;
int score = 0;
std::random_device rdev;
std::vector<double> intervals = {-200, 0.0, WIDTH, WIDTH +200};
std::vector<double> weights = {1.0, 0.0, 0.0, 1.0}; 
std::piecewise_linear_distribution<float> dist_pos(intervals.begin(),intervals.end(),weights.begin());
std::uniform_real_distribution<float> dist_vel(-MAX_VELOCITY,MAX_VELOCITY);
enum Astroid_Size {small,big};
enum Game_State{main_screen,game_screen,death_screen,leaderboard_screen};
Game_State state = main_screen;
std::vector<char*> leaderboard;
class Bullet{
    public:
        Bullet(Vector2 p,Vector2 v){
            position = p;
            velocity = v;
        }
        void draw(){
            DrawCircle(position.x,position.y,2,WHITE);
        }
        void update(){
            position+= Vector2Scale(velocity,DT);
        }
        Vector2 getPosition(){
            return position;
        }
    private:
        Vector2 position,velocity;
};
std::vector<Bullet> Bullets;
class Player{
    public:
        Player(Vector2 p,float r){
            centeral_point = p;
            rotation = r;
            bullet_sound = LoadSound("src\\SOUNDS\\bullet_sound.wav");
            hit_box = {centeral_point.x-SIDE_LENGTH/2,centeral_point.y-SIDE_LENGTH/2,SIDE_LENGTH,SIDE_LENGTH};
            velocity = {0};
            updateRotation(); 
        }
        void controlles(){
            if(IsKeyDown(KEY_A)||IsKeyDown(KEY_LEFT)){
                rotation -= 120*DEG2RAD*DT; 
            }
            if(IsKeyDown(KEY_D)||IsKeyDown(KEY_RIGHT)){
                rotation += 120*DEG2RAD*DT; 
            }
            if(IsKeyDown(KEY_W)||IsKeyDown(KEY_UP)){
                if(Vector2Length(velocity)<=MAX_VELOCITY)
                    velocity += Vector2Scale(Vector2Subtract( vertecies[0],centeral_point),DT*5);
            }
            if(IsKeyDown(KEY_S)||IsKeyDown(KEY_DOWN)){
                velocity = Vector2Scale(Vector2Subtract( vertecies[0],centeral_point),-DT*MAX_VELOCITY*2);
            }
            if(IsKeyPressed(KEY_SPACE)){
                Bullets.emplace_back(vertecies[0],Vector2Scale(Vector2Subtract(vertecies[0],centeral_point),DT*700));
                PlaySound(bullet_sound);
            }
        }
        void update(){
            controlles();
            centeral_point +=Vector2Scale(velocity,DT);
            velocity = Vector2Scale(velocity,DAMP);
            updateRotation();
            hit_box = {centeral_point.x-SIDE_LENGTH/2,centeral_point.y-SIDE_LENGTH/2,SIDE_LENGTH,SIDE_LENGTH};
        }
        void updateRotation(){
            rotation_sin = sin(rotation);
            rotaion_cos = cos(rotation);
            vertecies[0] = {centeral_point.x,centeral_point.y-(SIDE_LENGTH*DISTANCE_CONSTANT)};
            vertecies[1] = {centeral_point.x-(SIDE_LENGTH/2),centeral_point.y+(SIDE_LENGTH*DISTANCE_CONSTANT/2)};
            vertecies[2] = {centeral_point.x+(SIDE_LENGTH/2),centeral_point.y+(SIDE_LENGTH*DISTANCE_CONSTANT/2)};
            for(int i = 0; i<3;i++){
                Vector2 old_pos = vertecies[i]-centeral_point;
                vertecies[i] = {old_pos.x*rotaion_cos-old_pos.y*rotation_sin,old_pos.x*rotation_sin+old_pos.y*rotaion_cos};
                vertecies[i]+= centeral_point;
            }
        }
        void draw(){
            DrawTriangleLines(vertecies[0],vertecies[1],vertecies[2],WHITE);
        }
        Rectangle getHitBox(){
            return hit_box;
        }
        Vector2* getVertacies(){
            return vertecies;
        }
        void setPosition(Vector2 p){
            centeral_point = p;
        }
    private:
        Sound bullet_sound;
        Rectangle hit_box;
        Vector2 vertecies[3],centeral_point,velocity;
        float rotation,rotation_sin,rotaion_cos;
};
class Astroid{
    public:
        Astroid(Vector2 p,Vector2 v,Astroid_Size s,float r){
            position = p;
            velocity = v;
            size =s;
            radious = r;
            if(size == big){
                hp =3;
                radious = 100;
            }else{
                hp=1;
                radious = 40;
            }
            hit_box = {position.x-radious*3.0f/4.0f,position.y-radious*3.0f/4.0f,radious*3.0f/2.0f,radious*3.0f/2.0f};

        }
        void update(){
            hit_box = {position.x-radious*3.0f/4.0f,position.y-radious*3.0f/4.0f,radious*3.0f/2.0f,radious*3.0f/2.0f}; 
            position += Vector2Scale(velocity,DT); 
        }
        void playerCollision(Player& p){
            if(CheckCollisionRecs(hit_box,p.getHitBox())){
                state = death_screen;
            }
        }
        int bulletColision(Bullet b,int i){
            if(CheckCollisionPointRec(b.getPosition(),hit_box)){
                hp--;
                Bullets.erase(Bullets.begin()+i);
                i--;
            }
            return i;
        }
        void draw(){
            DrawPolyLines(position,8,radious,rotation,WHITE);
        }
        Vector2 getPosition(){
            return position;
        }
        float getHp(){
            return hp;
        }
        Rectangle getHitBox(){
            return hit_box;
        }
        Astroid_Size getSize(){
            return size;
        }
        private:
            Vector2 position,velocity;
            Rectangle hit_box;
            Astroid_Size size;
            float radious,rotation;
            int hp;
};
std::vector<Astroid> Astroids;
class DeathAnimation{
    public:
        DeathAnimation(Player p){
            vertacies[0] = p.getVertacies()[0];
            vertacies[1] = p.getVertacies()[1];
            vertacies[2] = p.getVertacies()[2];
            vertacies[3] = p.getVertacies()[0];
            vertacies[4] = p.getVertacies()[1];
            vertacies[5] = p.getVertacies()[2];
            velocities[0] = {dist_vel(rdev),dist_vel(rdev)};
            velocities[1] = {dist_vel(rdev),dist_vel(rdev)};
            velocities[2] = {dist_vel(rdev),dist_vel(rdev)};
        }
        void update(){
            for(int i = 0;i<6;i+=2){
                vertacies[i] += Vector2Scale( velocities[i/2],DT);
                vertacies[i+1] += Vector2Scale( velocities[i/2],DT);
            }
        }
        void draw(){
            DrawLine(vertacies[0].x,vertacies[0].y,vertacies[1].x,vertacies[1].y,WHITE);
            DrawLine(vertacies[2].x,vertacies[2].y,vertacies[3].x,vertacies[3].y,WHITE);
            DrawLine(vertacies[4].x,vertacies[4].y,vertacies[5].x,vertacies[5].y,WHITE);
        }
        void corectPosition(Player p){
            vertacies[0] = p.getVertacies()[0];
            vertacies[1] = p.getVertacies()[1];
            vertacies[2] = p.getVertacies()[2];
            vertacies[3] = p.getVertacies()[0];
            vertacies[4] = p.getVertacies()[1];
            vertacies[5] = p.getVertacies()[2];
        }
    private:
        Vector2 vertacies[6],velocities[3];
};
void updateBullets(){
    for(size_t i = 0;i<Bullets.size();i++){
        if(!CheckCollisionPointRec(Bullets[i].getPosition(),{0,0,WIDTH,HEIGHT})){
            Bullets.erase(Bullets.begin()+i);
            i--;
            continue;
        }
        Bullets[i].update();
    }
}
void drawBullets(){
    std::for_each(Bullets.begin(),Bullets.end(),[](Bullet& b){
        b.draw();
    });
}
void spawnAstroids(){
    if(milli_second_since_start%500 == 0){
        Astroids.emplace_back((Vector2){dist_pos(rdev),dist_pos(rdev)},(Vector2){dist_vel(rdev),dist_vel(rdev)},big,dist_pos(rdev));
    }
}
void checkColisionAstroidPlayer(Player& p){
    std::for_each(Astroids.begin(),Astroids.end(),[&p](Astroid& a){
        a.playerCollision(p);
    });
}
void checkColisionAstroidBullet(){
    for(size_t i = 0;i<Bullets.size();i++){
        std::for_each(Astroids.begin(),Astroids.end(),[&i](Astroid& a){
            i = a.bulletColision(Bullets[i],i);
        });
    }
}
void updateAstroids(Player& p){
    spawnAstroids();
    checkColisionAstroidBullet();
    checkColisionAstroidPlayer(p);
    for(size_t i = 0;i<Astroids.size();i++){
        if(Astroids[i].getHp()<=0){
                if(Astroids[i].getSize() == big){
                    Astroids.emplace_back(Astroids[i].getPosition(),(Vector2){dist_vel(rdev),dist_vel(rdev)},small,dist_pos(rdev));
                    Astroids.emplace_back(Astroids[i].getPosition(),(Vector2){dist_vel(rdev),dist_vel(rdev)},small,dist_pos(rdev));
                }
                Astroids.erase(Astroids.begin()+i);
                score += 100;
            continue;
        }
        Astroids[i].update();
    }
}
void drawAstroids(){
    std::for_each(Astroids.begin(),Astroids.end(),[](Astroid& a){
        a.draw();
    });
}
int extractInteger(char a[50]){
    std::string placeholder = a;
    std::string temp;
    for(int i =0;i<placeholder.size();i++){
        if(isdigit(placeholder[i])){
            temp+= placeholder[i];
        }
    }
    return std::stoi(temp);
}
void orderLeaderboard(){
    bool is_not_ordered=true;
    while(is_not_ordered&&leaderboard.size()>1){
        for(size_t i=0;i<leaderboard.size()-1;i++){
            printf("%s\n",leaderboard[i]);
            printf("%s",leaderboard[i+1]);
            if(extractInteger(leaderboard[i])<extractInteger(leaderboard[i+1])){
                char* temp = leaderboard[i+1];
                leaderboard[i+1] = leaderboard[i];
                leaderboard[i] = temp;
            }
        }

            for(size_t i = 0;i<leaderboard.size()-1;i++){
                if(extractInteger(leaderboard[i])<extractInteger(leaderboard[i+1])){
                    is_not_ordered = true;
                    break;
                }
                is_not_ordered = false;
            }
    }
}
void updateLeaderboard(char* name ,int score){
    static bool run_before=false;
    if(!run_before){
        char* buffer = new char[50] ;
        if (sprintf(buffer,"%s :%010d",name,score) >50){
            perror("too big to fit are you trying to do something");
            exit(1);
        }
        leaderboard.push_back(buffer);


        orderLeaderboard();
        if(leaderboard.size()>10){
            delete leaderboard.back();
            leaderboard.pop_back();
        }
        run_before = true;
    }
}
void drawLeaderBoard(){
    GuiSetStyle(DEFAULT,TEXT_SIZE,60);
    int leaderboard_text_size = MeasureText("THE LEADERBOARD",60);
    GuiLabel({(float)(WIDTH - leaderboard_text_size)/2,50,(float)leaderboard_text_size,80},"THE LEADERBOARD");
    GuiSetStyle(DEFAULT,TEXT_SIZE,40);
    int leaderboard_element_size ;
    for(size_t i = 0;i<leaderboard.size();i++){
        leaderboard_element_size = MeasureText(leaderboard[i],40);
        GuiLabel({(float)(WIDTH-leaderboard_element_size)/2,(float)(200+i*50),(float)leaderboard_element_size,50},leaderboard[i]);
    }
}
void storeInAFile(){
    static bool run_before = false;
    if(!run_before){
        std::ofstream leaderboard_file("leaderboard.txt");
        for(size_t i =0 ; i<leaderboard.size();i++){
            leaderboard_file << leaderboard[i]<<"\n";
        }
        leaderboard_file.close();  
        run_before = true;
    }
}
void loadFromAFile(){
    std::ifstream leaderboard_file("leaderboard.txt");
    std::string line_str;
    while(std::getline(leaderboard_file,line_str)){
        char* line = new char[line_str.length()+1]; 
        strcpy(line,line_str.c_str());
        printf("%s",line);
        leaderboard.push_back(line);
    }
    leaderboard_file.close();
}
int main() 
    {
    InitWindow(WIDTH, HEIGHT, "Astroids");
    InitAudioDevice();
    SetTargetFPS(60); 
    GuiSetStyle(TEXTBOX,BASE_COLOR_PRESSED,0x000000ff);
    GuiSetStyle(TEXTBOX,BORDER_COLOR_PRESSED,0xffffffff);
    GuiSetStyle(TEXTBOX,TEXT_COLOR_PRESSED,0xffffffff);
    GuiSetStyle(BUTTON,BASE_COLOR_NORMAL,0x000000ff);
    Player player({WIDTH/2,HEIGHT/2},0.0f);
    DeathAnimation animation(player); 
    char player_name[50] = "";
    loadFromAFile();
    while (!WindowShouldClose())
    {
        frames_since_start++;
        milli_second_since_start = (int)frames_since_start*DT*1000;    
        switch (state){
            case main_screen: if(IsKeyPressed(KEY_SPACE)){
                state = game_screen;
            } break;
            case game_screen: 
                animation.corectPosition(player); 
                player.update();
                updateBullets();
                updateAstroids(player);
                break;
            case death_screen: 
                if(time_of_death == 0){
                    PlaySound(LoadSound("src\\SOUNDS\\death_sound.wav"));
                    time_of_death = milli_second_since_start;
                }
                time_since_death = milli_second_since_start-time_of_death;
                if(time_since_death >1500){
                    state = leaderboard_screen;
                }
                animation.update();
            break;
            case leaderboard_screen :
            if(strcmp(player_name,"")!=0&&finished_typing){
                updateLeaderboard(player_name,score);
                storeInAFile();
            }
            break;
            
        }
        GuiSetStyle(DEFAULT,TEXT_SIZE,40);
        GuiSetStyle(DEFAULT,TEXT_COLOR_NORMAL,0xffffffff);
        char score_str[255];
        sprintf(score_str,"%010d",score);
        int score_text_size = MeasureText(score_str,40);
        BeginDrawing();
            ClearBackground(BLACK);
            int text_size = MeasureText("Press Space To Start The Game",40);   
            switch(state){
                case main_screen: 
                    player.draw(); 
                    GuiLabel({(float)(WIDTH-text_size)/2,(float)(HEIGHT - 50)/2,(float)text_size,50},"Press Space To Start The Game"); 
                     GuiLabel({(float)(WIDTH-score_text_size),0,(float)score_text_size,50},score_str);
                break;
                case game_screen:
                    player.draw();
                    drawAstroids();
                    drawBullets();
                    GuiLabel({(float)(WIDTH-score_text_size),0,(float)score_text_size,50},score_str);
                break;
                case death_screen:
                    animation.draw();
                    drawAstroids();
                    drawBullets();
                    GuiLabel({(float)(WIDTH-score_text_size),0,(float)score_text_size,50},score_str);
                break;
                case leaderboard_screen: 
                if(strcmp(player_name,"") >= 0 &&!finished_typing){
                    int text_size = MeasureText("Enter Your Name",40);
                    GuiLabel({(float)(WIDTH-text_size)/2,(float)(HEIGHT-120)/2,(float)text_size,50},"Enter Your Name");
                    GuiTextBox({(float)(WIDTH-text_size)/2,(float)(HEIGHT)/2,(float)text_size,50},player_name,50,true);
                    finished_typing = GuiButton({(float)(WIDTH+text_size+10)/2,(float)(HEIGHT)/2,(float)MeasureText("Submit",40),50},"Submit");  
                }else{
                    int text_size = MeasureText("Retry",40);
                    if(GuiButton({(float)WIDTH-text_size-20,HEIGHT-100,(float)text_size,50},"Retry")){
                        player.setPosition({WIDTH/2,HEIGHT/2});
                        Astroids.clear();
                        Astroids.shrink_to_fit();
                        Bullets.clear();
                        Bullets.shrink_to_fit();
                        finished_typing = false;
                        state = main_screen;
                    }
                    drawLeaderBoard();
                }
                break;
            }
            GuiLabel({(float)(WIDTH-score_text_size),0,(float)score_text_size,50},score_str);
        EndDrawing();
    }
    
    CloseWindow();
}
