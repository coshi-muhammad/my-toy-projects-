#include "screan.h"
#include <numeric>
#include <math.h>
struct vec3 {
    float x,y,z;
};
struct conection {
    int a,b;
};   
    int frameCount = 0;
    int lastTime = 0;
    int currentTime;
    float fps = 0.0;
void rotate(vec3& point,float x = 1, float y = 1, float z = 1){
    float rad = 0;
    rad = x;
    point.y = cos(rad) * point.y - sin(rad) * point.z;
    point.z = sin(rad)* point.y + cos(rad)* point.z;
    rad = y;
    point.x = cos(rad) * point.x + sin(rad) * point.z;
    point.z = -sin(rad) * point.x + cos(rad) * point.z;
    rad = z ;
    point.x = cos(rad) * point.x - sin(rad) * point.y;
    point.y = sin(rad) * point.x + cos(rad) * point.y;
}
void line(Screen& screen , float x1 , float y1 , float x2 , float y2){
    float dx = x2 - x1;
    float dy = y2 - y1;
    float lineLength = sqrt(dx*dx+dy*dy);
    float angle = atan2(dy,dx);
    for(float i = 0 ; i <lineLength ; i++){
        screen.pixelPlacer(
                            x1 + cos(angle)*i,
                            y1 + sin(angle)*i
        );
    }
}
int main (int argc,char *argv[]){
    Screen screen;
    std::vector<vec3> points {
        {200,200,200},
        {600,200,200},
        {600,600,200},
        {200,600,200},

        {200,200,600},
        {600,200,600},
        {600,600,600},
        {200,600,600}
    };
    std :: vector<conection> conections {
        {0,4},  
        {1,5},  
        {2,6},  
        {3,7},  
        {0,1},  
        {1,2},  
        {2,3},  
        {3,0},  
        {4,5},  
        {5,6},  
        {6,7},  
        {7,4},  
    };
    vec3 centroid{0,0,0};
    for ( auto& point:points){
        centroid.x+=point.x;
        centroid.y+=point.y;
        centroid.z+=point.z;
    }
    centroid.x /= points.size() ;
    centroid.y /= points.size() ;
    centroid.z /= points.size() ;
    while(true){
        for(auto& point: points){
            point.x -= centroid.x;
            point.y -= centroid.y;
            point.z -= centroid.z;
            rotate(point,0.002,0.002,0.002);
            point.x += centroid.x;
            point.y += centroid.y;
            point.z += centroid.z;
            screen.pixelPlacer(point.x,point.y);
        }
        for ( auto& conection : conections){
            line(screen,points[conection.a].x,points[conection.a].y,points[conection.b].x,points[conection.b].y);
        }
        screen.showPixels();
        screen.pixelclear();
        screen.events();
        frameCount++;
        currentTime = SDL_GetTicks();
        if (currentTime - lastTime >= 1000) {
            fps = frameCount / ((currentTime - lastTime) / 1000.0f);  // Calculate FPS
            std::cout << "FPS: " << fps << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }
    }

    return 0;
};