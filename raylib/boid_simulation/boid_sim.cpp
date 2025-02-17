#include "include/raylib.h"
#include "include/raymath.h"
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

// NOTE: type declarations
struct Boid {
  Vector2 position, velocity, accelaration;
  float rotation, rotation_vel;
  Texture2D tex;
  Boid(Vector2 p, Vector2 v, Vector2 a, float r) {
    position = p;
    velocity = v;
    accelaration = a;
    rotation = r;
    rotation_vel = 0;
    tex = LoadTexture("./boid.png");
  }
};

// NOTE: globals
const int WIDTH = 1280, HEIGHT = 720;
const int CENTER_X = WIDTH / 2, CENTER_Y = HEIGHT / 2;
const int BOID_COUNT = 100;
const int OPSTICAL_COUNT = 10;
const float MAX_VELOCITY = 100.0f;
const int WARPING_THRESHOLD = 50;
const float DAMPANING = 0.9f;
float cohegen_strength = 1.0f;
float separation_strength = 3000.0f;
float alignment_strength = 1.0f;
float avoidance_strength = 1000.0f;
float rotation_rate = 0.8f;
int cohegen_radius = 100;
int separation_radius = 30;
int alignment_radius = 60;
int avoidance_radius = 30;
Boid *boid_list[BOID_COUNT];
Vector2 *opstical_list[OPSTICAL_COUNT];
// NOTE: functionality

void createBoids() {
  for (size_t i = 0; i < BOID_COUNT; i++) {
    Boid *boid_ptr =
        new Boid({(float)(rand() % WIDTH), (float)(rand() % HEIGHT)},
                 {(float)(rand() % 100), (float)(rand() % 100)}, {0},
                 (float)rand() / RAND_MAX * 360);
    if (boid_ptr == nullptr) {
      perror("someting went wrong ");
      exit(1);
    }
    boid_list[i] = boid_ptr;
  }
}

void deleteBoids() {
  for (size_t i = 0; i < BOID_COUNT; i++) {
    delete (boid_list[i]);
  }
}

void drawBoids() {
  for (size_t i = 0; i < BOID_COUNT; i++) {
    DrawTextureEx(boid_list[i]->tex, boid_list[i]->position,
                  boid_list[i]->rotation, 0.03, WHITE);
  }
}

void createOpsticals() {
  for (size_t i = 0; i < OPSTICAL_COUNT; i++) {
    Vector2 *opstical_ptr = new Vector2{(float)(i * 100), (float)(i * 100)};
    if (opstical_ptr == nullptr) {
      perror("someting went wrong ");
      exit(1);
    }
    opstical_list[i] = opstical_ptr;
  }
}

void deleteOpsticals() {
  for (size_t i = 0; i < OPSTICAL_COUNT; i++) {
    delete (opstical_list[i]);
  }
}

void drawOpsticals() {
  for (size_t i = 0; i < OPSTICAL_COUNT; i++) {
    DrawCircle(opstical_list[i]->x, opstical_list[i]->y, 10, RED);
  }
}

void applyCohegen(int i) {
  int neighbor_count = 0;
  Vector2 sum = {0};
  for (size_t j = 0; j < BOID_COUNT; j++) {
    if (i != j && Vector2Distance(boid_list[i]->position,
                                  boid_list[j]->position) < cohegen_radius) {
      neighbor_count++;
      sum = Vector2Add(
          sum, Vector2Subtract(boid_list[j]->position, boid_list[i]->position));
    }
  }
  if (neighbor_count > 0) {
    boid_list[i]->accelaration =
        Vector2Add(boid_list[i]->accelaration,
                   Vector2Scale(sum, cohegen_strength / neighbor_count));
  }
}

void applySeparation(int i) {
  int neighbor_count = 0;
  Vector2 sum = {0, 0};
  for (size_t j = 0; j < BOID_COUNT; j++) {
    Vector2 dp =
        Vector2Subtract(boid_list[i]->position, boid_list[j]->position);
    float dp_len_sqr = Vector2Length(dp) * Vector2Length(dp);
    if (i != j && Vector2Distance(boid_list[i]->position,
                                  boid_list[j]->position) < separation_radius) {
      neighbor_count++;
      sum = Vector2Add(sum, Vector2Scale(dp, 1.0f / dp_len_sqr));
    }
  }
  if (neighbor_count > 0) {
    boid_list[i]->accelaration =
        Vector2Add(boid_list[i]->accelaration,
                   Vector2Scale(sum, separation_strength / neighbor_count));
  }
}

void applyAlignment(int i) {
  int neighbor_count = 0;
  Vector2 sum = {0};
  for (size_t j = 0; j < BOID_COUNT; j++) {
    if (i != j && Vector2Distance(boid_list[i]->position,
                                  boid_list[j]->position) < alignment_radius) {
      neighbor_count++;
      sum = Vector2Add(
          sum, Vector2Subtract(boid_list[j]->velocity, boid_list[i]->velocity));
    }
  }
  if (neighbor_count > 0) {
    boid_list[i]->accelaration =
        Vector2Add(boid_list[i]->accelaration,
                   Vector2Scale(sum, alignment_strength / neighbor_count));
  }
}

void warp(int i) {
  if (boid_list[i]->position.x > WIDTH + WARPING_THRESHOLD) {
    boid_list[i]->position.x = -WARPING_THRESHOLD;
  }
  if (boid_list[i]->position.x < -WARPING_THRESHOLD) {
    boid_list[i]->position.x = WIDTH + WARPING_THRESHOLD;
  }
  if (boid_list[i]->position.y > HEIGHT + WARPING_THRESHOLD) {
    boid_list[i]->position.y = -WARPING_THRESHOLD;
  }
  if (boid_list[i]->position.y < -WARPING_THRESHOLD) {
    boid_list[i]->position.y = HEIGHT + WARPING_THRESHOLD;
  }
}
void avoidOpsticals(int i) {
  int neighbor_count = 0;
  Vector2 sum = {0, 0};
  for (size_t j = 0; j < OPSTICAL_COUNT; j++) {
    Vector2 dp = Vector2Subtract(boid_list[i]->position, *opstical_list[j]);
    float dp_len_sqr = Vector2Length(dp);
    if (i != j && Vector2Distance(boid_list[i]->position, *opstical_list[j]) <
                      avoidance_radius) {
      neighbor_count++;
      sum = Vector2Add(sum, Vector2Scale(dp, 1.0f / dp_len_sqr));
    }
  }
  if (neighbor_count > 0) {
    boid_list[i]->accelaration = Vector2Add(
        boid_list[i]->accelaration, Vector2Scale(sum, avoidance_strength));
  }
}
void updateBoid(float dt) {
  for (size_t i = 0; i < BOID_COUNT; i++) {
    float angle = 0, d_theta = 0;
    // reset accelaration
    boid_list[i]->accelaration = {0, 0};
    // applying the forces
    applyCohegen(i);
    applySeparation(i);
    applyAlignment(i);
    avoidOpsticals(i);
    warp(i);
    // updating the velocity and position
    boid_list[i]->velocity = Vector2Add(
        boid_list[i]->velocity, Vector2Scale(boid_list[i]->accelaration, dt));
    if (Vector2Length(boid_list[i]->velocity) >= MAX_VELOCITY) {
      boid_list[i]->velocity =
          Vector2Scale(Vector2Normalize(boid_list[i]->velocity), MAX_VELOCITY);
    }
    boid_list[i]->position = Vector2Add(
        boid_list[i]->position, Vector2Scale(boid_list[i]->velocity, dt));
    angle =
        atan2(boid_list[i]->velocity.y, boid_list[i]->velocity.x) / PI * 180 +
        90;
    d_theta = angle - boid_list[i]->rotation;
    boid_list[i]->rotation_vel = d_theta * DAMPANING * rotation_rate;
    boid_list[i]->rotation += boid_list[i]->rotation_vel * dt;
  }
}

int main() {
  InitWindow(1280, 720, "boid simulation");
  srand(time(0));
  createBoids();
  createOpsticals();
  while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    std::cout << 1.0f / dt << "\n";
    updateBoid(dt);
    BeginDrawing();
    ClearBackground(SKYBLUE);
    drawBoids();
    drawOpsticals();
    EndDrawing();
  }
  deleteOpsticals();
  deleteBoids();
  return 0;
}
