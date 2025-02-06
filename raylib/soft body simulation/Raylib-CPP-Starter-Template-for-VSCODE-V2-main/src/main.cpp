#include <raylib.h>
#include <raymath.h>
#include <raygui.h>
#include <cmath>
#include <vector>
constexpr int WIDTH = 1280;
constexpr int HEIGHT =720;
Vector2 gravity = {0,4};
float dt = 1.0f/120.0f;
float El=0.8f;
float edgeLength = 50;
bool vecInRange(Vector2 v1,Vector2 v2){
    return (Vector2Distance(v1,v2)<100);
}
class MassPoint{
    public:
    Vector2 position;
    Vector2 speed;
    Vector2 scaledSpeed;
    Vector2 acelaration;
    Vector2 force;
    float mass;
    float radius;
    Color color;
    MassPoint(){

    }
    MassPoint(Vector2 p,Vector2 s,float m,Color c){
        position =p;
        speed = s;
        mass = m;
        radius = mass*10;
        color = c;
    }
    void Draw(){
        DrawCircle(position.x,position.y,radius,color);
    }
    void folowMouse(){
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)&&vecInRange(GetMousePosition(),position)){
        position=GetMousePosition();

    }
}
void Update(){
            speed = Vector2Add(speed, force);
            scaledSpeed = Vector2Scale(speed, dt);
            position = Vector2Add(position, scaledSpeed);
    
}
void resetForce(){
    force = Vector2Zero();
}
};
class Spring{
public:
MassPoint* p1;
MassPoint* p2;
float L,cL,k,d,force;
Spring(MassPoint* mp1,MassPoint* mp2,float rl){
    p1 = mp1;
    p2 = mp2;
    L= Vector2Distance(p1->position,p2->position);
    cL = rl;
    k = 700.0f;
    d = 20.0f;
}
void Draw(){
    DrawLine(p1->position.x,p1->position.y,p2->position.x,p2->position.y,WHITE);
}

void Update(){
    colisionDetect();
    force = 0;
    float dL = (L - cL)*k;
    Vector2 dv = Vector2Subtract(p1->speed,p2->speed);
    Vector2 p1_to_p2 =Vector2Subtract(p1->position,p2->position);
    Vector2 p2_to_p1 = Vector2Subtract(p2->position,p1->position);
    Vector2 normilized1 = Vector2Normalize(p1_to_p2);
    Vector2 normilized2 = Vector2Normalize(p2_to_p1);
    float dotVectors = Vector2DotProduct(dv,normilized1)*d;
    force = dL + dotVectors;
    Vector2 force1 = Vector2Scale(normilized2,force*dt/p1->mass);
    Vector2 force2 = Vector2Scale(normilized1,force*dt/p2->mass);
    p1->force = Vector2Add(p1->force,force1);
    p2->force = Vector2Add(p2->force,force2);

    // p1->speed = Vector2Add(p1->speed,p1->force);
    // p2->speed = Vector2Add(p2->speed,p2->force);

    // p1->scaledSpeed = Vector2Scale(p1->speed,dt);
    // p2->scaledSpeed = Vector2Scale(p2->speed,dt);
    // p1->position = Vector2Add(p1->position,p1->scaledSpeed);
    // p2->position = Vector2Add(p2->position,p2->scaledSpeed);

    L= Vector2Distance(p1->position,p2->position);
}
void colisionDetect(){
    if(p1->position.y>HEIGHT-p1->radius){
        p1->position.y = HEIGHT-p1->radius;
        p1->speed.y *=-1;
        p1->speed.y *=El;
        
    }
    if(p1->position.y<p1->radius){
        p1->position.y = p1->radius;
        p1->speed.y *=-1;
        p1->speed.y *=El;
    }
    if(p1->position.x>WIDTH-p1->radius){
        p1->position.x = WIDTH-p1->radius;
        p1->speed.x *=-1;
        p1->speed.x *=El;
    }
    if(p1->position.x<p1->radius){
        p1->position.x = p1->radius;
        p1->speed.x *=-1;
        p1->speed.x *=El;
    }
    if(p2->position.y>HEIGHT-p2->radius){
        p2->position.y = HEIGHT - p2->radius;
        p2->speed.y *=-1;
        p2->speed.y *=El;
    }
    if(p2->position.y<p2->radius){
        p2->position.y = p2->radius;
        p2->speed.y *=-1;
        p2->speed.y *=El;
    }
    if(p2->position.x>WIDTH-p2->radius){
        p2->position.x = WIDTH -p2->radius;
        p2->speed.x *=-1;
        p2->speed.x *=El;
    }
    if(p2->position.x<p2->radius){
        p2->position.x = p2->radius;
        p2->speed.x *=-1;
        p2->speed.x *=El;
    }

}
};
std::vector<std::vector<MassPoint>> vertecies;
std::vector<Spring> edges;
void fillVertecies(int rowNum,int colomNum,std::vector<std::vector<MassPoint>>& v){
   for(int i =0;i<rowNum;i++){
    std::vector<MassPoint> colum;
    for(int j =0;j<colomNum;j++){
        colum.push_back(MassPoint((Vector2){100+j*edgeLength,100+i*edgeLength},(Vector2){0,0},2.0f,RED));
    }
    v.push_back(colum);
   } 
}
void DrawVertecies(std::vector<std::vector<MassPoint>>& v){
    for (int i=0;i<(int)v.size();i++){
        for(int j =0;j<(int)v[0].size();j++){
            MassPoint p = v[i][j];
            DrawCircle(p.position.x,p.position.y,p.radius,p.color);
        }
    }
}
void fillSprings(std::vector<Spring>& s,std::vector<std::vector<MassPoint>>& v){
    float diagonal = std::sqrt(2)*edgeLength;
    int rowNum = (int)v.size();
    if(rowNum ==0){
        return;
    }
    int columNum = (int)v[0].size();
    int expectedSprings = (rowNum - 1) * columNum + // vertical
                         rowNum * (columNum - 1) + // horizontal
                         2 * (rowNum - 1) * (columNum - 1); // diagonals
    s.reserve(s.size() + expectedSprings);
    for (int i = 0; i < rowNum; i++) {
    for (int j = 0; j < columNum; j++) {
        // Horizontal connection
        if (j + 1 < columNum) {
            s.push_back(Spring(&v[i][j],&v[i][j+1],edgeLength)); // Connect (i, j) to (i, j+1)
        }

        // Vertical connection
        if (i + 1 < rowNum) {
             // Connect (i, j) to (i+1, j)
            s.push_back(Spring(&v[i][j],&v[i+1][j],edgeLength)); // Connect (i, j) to (i, j+1)
        }

        // Bottom-right diagonal connection
        if (i + 1 < rowNum && j + 1 < columNum) {
             // Connect (i, j) to (i+1, j+1)
            s.push_back(Spring(&v[i][j],&v[i+1][j+1],diagonal)); // Connect (i, j) to (i, j+1)
        }

        // Bottom-left diagonal connection
        if (i + 1 < rowNum && j - 1 >= 0) {
             // Connect (i, j) to (i+1, j-1)
            s.push_back(Spring(&v[i][j],&v[i+1][j-1],diagonal)); // Connect (i, j) to (i, j+1)
        }
    }
}
}
void DrawSprings(std::vector<Spring>s){
    for(int i=0;i<s.size();i++){
        s[i].Draw();
    }
}
// void ballColision(MassPoint& p1,MassPoint& p2){
//         float distance = Vector2Distance(p1.position,p2.position);
//         float overlap = p1.radius+p1.radius-distance;
//         if(distance ==0){
//             return;
//         }
//     if(overlap>0){
//         Vector2 dp = Vector2Subtract(p1.position,p2.position);
//         Vector2 normalizedDp = Vector2Normalize(dp);
//         Vector2 indp = Vector2Invert(normalizedDp);
//         Vector2 moveVector = Vector2Scale(normalizedDp,overlap/2);
//         p1.position = Vector2Add(p1.position,moveVector);
//         p2.position = Vector2Subtract(p2.position,moveVector);
//         p1.speed = Vector2Reflect(p1.speed,normalizedDp);
//         p2.speed = Vector2Reflect(p2.speed,indp);
        
        
//     }
// }
void ballCollision(MassPoint& p1, MassPoint& p2) {
    float distance = Vector2Distance(p1.position, p2.position);

    // Handle degenerate case where points overlap perfectly
    if (distance == 0.0f) {
        distance = 0.001f; // Small arbitrary value to avoid divide-by-zero
    }

    float overlap = (p1.radius + p2.radius) - distance;

    if (overlap > 0.0f) {
        // Direction from p2 to p1
        Vector2 dp = Vector2Subtract(p1.position, p2.position);
        Vector2 normalizedDp = Vector2Normalize(dp);

        // Separate the overlapping balls equally
        Vector2 moveVector = Vector2Scale(normalizedDp, overlap / 2.0f);
        p1.position = Vector2Add(p1.position, moveVector);
        p2.position = Vector2Subtract(p2.position, moveVector);

        // Compute relative velocity
        Vector2 relativeVelocity = Vector2Subtract(p1.speed, p2.speed);

        // Project relative velocity onto the collision normal
        float velocityAlongNormal = Vector2DotProduct(relativeVelocity, normalizedDp);

        // Reflect only if objects are moving toward each other
        if (velocityAlongNormal < 0.0f) {
            // Adjust velocities based on elasticity (assuming perfectly elastic collision here)
            Vector2 impulse = Vector2Scale(normalizedDp, -2.0f * velocityAlongNormal / 2.0f);

            p1.speed = Vector2Add(p1.speed, impulse);
            p2.speed = Vector2Subtract(p2.speed, impulse);
        }
    }
}
void checkCollisionForAllBalls(){
    for(int i=0;i<vertecies.size();i++){
        for(int j=0;j<vertecies[0].size();j++){
            for (int k =0;k<vertecies.size();k++){
                for(int h = 0;h<vertecies[0].size();h++){
                if(abs(i-k) <= 1 && abs(j-h) <= 1) continue;
                ballCollision(vertecies[i][j],vertecies[k][h]);
                }
            }
        }
    }
}
void UpdateBody(std::vector<Spring>& s){
    // Reset all forces first
    for(auto& row : vertecies) {
        for(auto& point : row) {
            point.resetForce();
            // Apply gravity
            point.force = Vector2Add(point.force, Vector2Scale(gravity, point.mass));
        }
    }
    
    // Update all springs
    for(int i=0; i<s.size(); i++){
        s[i].Update();
    }
    
    // Update positions after all forces are accumulated
    for(auto& row : vertecies) {
        for(auto& point : row) {
            point.Update();
            // point.speed = Vector2Add(point.speed, point.force);
            // point.scaledSpeed = Vector2Scale(point.speed, dt);
            // point.position = Vector2Add(point.position, point.scaledSpeed);
        }
    }
}
void controlPoints(std::vector<std::vector<MassPoint>>& v) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        
        // Check all points in the grid
        for (int i = 0; i < v.size(); i++) {
            for (int j = 0; j < v[0].size(); j++) {
                // If point is within range of mouse
                if (Vector2Distance(mousePos, v[i][j].position) < 20) {
                    // Set point position to mouse position
                    v[i][j].position = mousePos;
                    // Reset speed to prevent momentum
                    v[i][j].speed = {0, 0};
                }
            }
        }
    }
}
int main() 
{

    fillVertecies(6,6,vertecies);
    fillSprings(edges,vertecies);
    InitWindow(WIDTH,HEIGHT, "spring");
    SetTargetFPS(60);
    // MassPoint p1({400,400},{0,0},2.0f,RED);
    // MassPoint p2({600,400},{0,0},2.0f,RED);
    // MassPoint p3({500,426},{0,0},2.0f,RED);
    // Spring s1(&p1,&p2);
    // Spring s2(&p1,&p3);
    // Spring s3(&p3,&p2);
    while (!WindowShouldClose())
    {
        // s1.Update();
        // s2.Update();
        // s3.Update();
        // p1.folowMouse();
        // p2.folowMouse();
        // p3.folowMouse();
        controlPoints(vertecies);
        checkCollisionForAllBalls();
        UpdateBody(edges);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawSprings(edges);
        DrawVertecies(vertecies);
        // s1.Draw();
        // s2.Draw();
        // s3.Draw();
        // p1.Draw();
        // p2.Draw();
        // p3.Draw();
        EndDrawing();
    }
    
    CloseWindow();
}