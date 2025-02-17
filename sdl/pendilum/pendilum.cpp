#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "simulation.hpp"

const int WIDTH = 800;
const int HIEGHT = 600;

Simulation simulation("pendilum",WIDTH,HIEGHT);
int main(int argc,char *args[]){
    simulation.runSimulation();
    simulation.destructSimulation();
    return 0;
}