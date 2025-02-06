#define RAYGUI_IMPLEMENTATION
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <raygui.h>
#include <raylib.h>
#include <vector>
#include <string>
#include <ctype.h>
//why the hell arent these built in
bool isNumber(const char* n){
    for(int i =0;i<strlen(n);i++){
        if(!isdigit(n[i])){
            return false;
        }
    }
    return true;
}
std::string floatToString(float value, int precision) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}
//gloabal variables and declarations 
std::vector<float> solutions;
char parameters[3][100] = {0};
bool editable[3] = {0};
int WIDTH = 1280;
int HEIGHT = 720;
enum State {mainSection,firstInputSection,secondInputSection,resultSection,error};
State programeState = mainSection;
struct inputArea{
    Rectangle container;
    int fontSize = 20;
    const char* title;
    int index;
    inputArea(Rectangle  rec,const char* t,int f,int i){
        container = rec;
        title = t;
        fontSize = f;
        index = i;
    }
    void drawTitle(){
        GuiLabel({container.x+15,container.y+15,container.width-20,container.height/2-20},title);
    }
    void drawTextBox(){
        if (CheckCollisionPointRec(GetMousePosition(), {container.x + 10, container.y + container.height / 2 + 10, container.width - 20, container.height / 2 - 20})) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                editable[index] = true;
            }
        }else{
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                editable[index] = false; 
            }

        }
        
        GuiTextBox({container.x+20,container.y+container.height/2,container.width-40,container.height/2-40},parameters[index],100,editable[index]);  
    }
    void drawBorder(){
        DrawRectangleRounded(container,0.5,8,{156,160,95,255});
        DrawRectangleRounded({container.x+10,container.y+10,container.width-20,container.height-20},0.5,8,{233,234,220,255});
    }
    void draw(){
        GuiSetStyle(DEFAULT,TEXT_SIZE,40);
        drawBorder();
        drawTitle();
        drawTextBox();
    }

};
//equation solvers
bool firstDegree(const char* a,const char* b){
    if(!(isNumber(a)&&isNumber(b))){
        programeState = error;
        return false;
    }
    float A = std::stof(a);
    float B = std::stof(b);
    solutions.push_back(-B/(A));
    return true;
}
bool secondDegree(const char* a, const char* b , const char* c){
    if(!(isNumber(a)&&isNumber(b)&&isNumber(c))){
        programeState = error;
        return false;
    }
    float A = std::stof(a);
    float B = std::stof(b);
    float C = std::stof(c);
    float delta = sqrt(B*B-4*A*C);
    if(delta>0){
    solutions.push_back( (-B-delta)/(2*A));
    solutions.push_back( (-B+delta)/(2*A));
    }else if(delta == 0){
        solutions.push_back(-B/(2*A));
    }else{
        printf("there are no solutions");
    }
    return true;
}
//screen setters
void mainScreen(){
    GuiSetStyle(DEFAULT,TEXT_SIZE,40);
    int text_size1 = MeasureText("First degree",40);
    int text_size2 = MeasureText("Second degree",40);
    if(GuiButton({100,(float)(HEIGHT*3/4),(float)text_size1,(float)(HEIGHT/8)},"First degree")==1){
        programeState = firstInputSection;
    }
    if(GuiButton({(float)(WIDTH-100-text_size2),(float)(HEIGHT*3/4),(float)text_size2,(float)(HEIGHT/8)},"Second degree")==1){
        programeState = secondInputSection;
    }
}
void firstInputScreen(){
    inputArea area1((Rectangle){70,(float)(HEIGHT/4),300,(float)HEIGHT/3},"a",40,0);
    inputArea area2((Rectangle){(float)WIDTH-370,(float)(HEIGHT/4),300,(float)HEIGHT/3},"b",40,1);
    area1.draw();
    area2.draw();
    if(GuiButton({(float)(WIDTH-350),(float)(HEIGHT*3/4),300,(float)(HEIGHT/8)},"Get Solution") == 1){
        if(firstDegree(parameters[0],parameters[1])){
            programeState = resultSection;
        }
    }
}
void secondInputScreen(){
    inputArea area1((Rectangle){70,(float)(HEIGHT/4),200,(float)HEIGHT/4},"a",30,0);
    inputArea area2((Rectangle){300,(float)(HEIGHT/4),200,(float)HEIGHT/4},"b",30,1);
    inputArea area3((Rectangle){(float)WIDTH-270,(float)(HEIGHT/4),200,(float)HEIGHT/4},"c",30,2);
    area1.draw();
    area2.draw();
    area3.draw();
    if(GuiButton({(float)(WIDTH-350),(float)(HEIGHT*3/4),300,(float)(HEIGHT/8)},"Get Solution") == 1){
        if(secondDegree(parameters[0],parameters[1],parameters[2])){
            programeState = resultSection;
        }
    }
}
void errorScreen(){
    GuiSetStyle(DEFAULT,TEXT_SIZE,60);
    int error_width = MeasureText("Something went wrong",60);
    float error_message_position = (GetScreenWidth()-error_width)/2+40;
    GuiLabel({error_message_position,200,(float)error_width,50},"Something went wrong");
    if(GuiButton({(float)(WIDTH-300),(float)(HEIGHT*3/4),200,(float)(HEIGHT/8)},"Retry")==1){
        programeState = mainSection;
    }
}
void resultScreen(){
    GuiSetStyle(DEFAULT,TEXT_SIZE,40);
    int text_width = MeasureText("the list of solutions is: ",40);
    GuiLabel({100,200,(float)text_width,50},"the list of solutions is:");
    for (int i=0;i<solutions.size();i++){
        std::string s = " "+floatToString(solutions[i],2)+"  ";
        int element_width = MeasureText(s.c_str(),40);
        GuiLabel({(float)(text_width+20+i*element_width),200,(float)element_width,50},s.c_str());
    }
    
}
//main funtion
int main(){
    InitWindow(WIDTH,HEIGHT,"equation solver");
    int title_width = MeasureText("Equation Solver",60);
    float title_positionx = (GetScreenWidth()-title_width)/2+40;
    GuiSetStyle(DEFAULT,TEXT_COLOR_NORMAL,0x000000ff);
    while (!WindowShouldClose())
    {

        BeginDrawing();
            ClearBackground({233,234,220,255});
            GuiSetStyle(DEFAULT,TEXT_SIZE,60);
            GuiLabel({title_positionx,50,(float)title_width ,50},"equation solver");
            switch (programeState)
            {
            case mainSection:mainScreen(); break;
            case firstInputSection:firstInputScreen(); break;
            case secondInputSection:secondInputScreen(); break;
            case resultSection:resultScreen(); break;
            case error:errorScreen(); break; 
            }
            
        EndDrawing(); 
    }
    CloseWindow(); 
    return 0;
}