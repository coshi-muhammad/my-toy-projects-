#include "include/dynamic_array.h"
#include "include/raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"
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
  Boid(Vector2 p, Vector2 v, Vector2 a, float r, Texture2D t) {
    position = p;
    velocity = v;
    accelaration = a;
    rotation = r;
    rotation_vel = 0;
    tex = t;
  }
};

// NOTE: globals
const int WIDTH = 1280, HEIGHT = 720;
const int CENTER_X = WIDTH / 2, CENTER_Y = HEIGHT / 2;
const float MAX_VELOCITY = 100.0f;
const int WARPING_THRESHOLD = 50;
const float DAMPANING = 0.9f;
int inisial_boid_count = 0;
int inisial_opstical_count = 0;
float cohegen_strength = 1.0f;
float separation_strength = 3000.0f;
float alignment_strength = 1.0f;
float avoidance_strength = 1000.0f;
float rotation_rate = 0.8f;
float cohegen_radius = 100;
bool show_cohegen_radius = false;
float separation_radius = 30;
bool show_separation_radius = false;
float alignment_radius = 60;
bool show_alignment_radius = false;
float avoidance_radius = 30;
bool show_avoidance_radius = false;
float deadly_radius = 25;
bool show_deadly_radius = false;
bool add_boid = false;
bool add_opstical = false;
bool show_dropdown_checkboxes = false;
bool show_dropdown_strength_slider = false;
bool show_dropdown_radius_slider = false;
int element_hight = 30;
int panel_height = 0;
DynamicArray *boids = NULL;
DynamicArray *opsticals = NULL;
int margins = 10;
bool starting_screen = true;
// NOTE: functionality

void createBoids(Texture2D tex) {
  boids = alocateArray(sizeof(Boid));
  for (size_t i = 0; i < inisial_boid_count; i++) {
    Boid temp({(float)(rand() % WIDTH), (float)(rand() % HEIGHT)},
              {(float)(rand() % 100), (float)(rand() % 100)}, {0},
              (float)rand() / RAND_MAX * 360, tex);

    pushIntoArray(boids, &temp);
  }
}

void deleteBoids() { deleteArray(boids); }

void drawBoids() {
  Boid *temp = NULL;
  Rectangle src, dst;
  Vector2 center;
  for (size_t i = 0; i < boids->size; i++) {

    temp = CAST(Boid) getArrayElement(boids, i);
    src = {0, 0, (float)temp->tex.width, (float)temp->tex.height};
    dst = {temp->position.x, temp->position.y, (float)temp->tex.width * 0.03f,
           (float)temp->tex.height * 0.03f};
    center = {dst.width / 2, dst.height / 2};
    DrawTexturePro(temp->tex, src, dst, center, temp->rotation, WHITE);
    if (show_cohegen_radius) {
      DrawCircleLines(temp->position.x, temp->position.y, cohegen_radius, BLUE);
    }
    if (show_separation_radius) {
      DrawCircleLines(temp->position.x, temp->position.y, separation_radius,
                      YELLOW);
    }
    if (show_alignment_radius) {
      DrawCircleLines(temp->position.x, temp->position.y, alignment_radius,
                      GREEN);
    }
  }
}

void createOpsticals() {
  opsticals = alocateArray(sizeof(Vector2));
  for (size_t i = 0; i < inisial_opstical_count; i++) {
    Vector2 temp{(float)(rand() % WIDTH), (float)(rand() % HEIGHT)};
    pushIntoArray(opsticals, &temp);
  }
}

void deleteOpsticals() { deleteArray(opsticals); }

void drawOpsticals() {
  Vector2 *temp = NULL;
  for (size_t i = 0; i < opsticals->size; i++) {
    temp = CAST(Vector2) getArrayElement(opsticals, i);
    DrawCircle(temp->x, temp->y, 10, RED);
    if (show_avoidance_radius) {
      DrawCircleLines(temp->x, temp->y, avoidance_radius, ORANGE);
    }
    if (show_deadly_radius) {
      DrawCircleLines(temp->x, temp->y, deadly_radius, MAGENTA);
    }
  }
}

void applyCohegen(int i) {
  int neighbor_count = 0;
  Vector2 sum = {0};
  Boid *temp1 = NULL;
  Boid *temp2 = NULL;
  temp1 = CAST(Boid) getArrayElement(boids, i);
  for (size_t j = 0; j < boids->size; j++) {
    temp2 = CAST(Boid) getArrayElement(boids, j);
    if (i != j &&
        Vector2Distance(temp1->position, temp2->position) < cohegen_radius) {
      neighbor_count++;
      sum = Vector2Add(sum, Vector2Subtract(temp2->position, temp1->position));
    }
  }
  if (neighbor_count > 0) {
    temp1->accelaration =
        Vector2Add(temp1->accelaration,
                   Vector2Scale(sum, cohegen_strength / neighbor_count));
  }
}

void applySeparation(int i) {
  int neighbor_count = 0;
  Vector2 sum = {0, 0};
  Boid *temp1 = NULL;
  Boid *temp2 = NULL;
  temp1 = CAST(Boid) getArrayElement(boids, i);
  for (size_t j = 0; j < boids->size; j++) {
    temp2 = CAST(Boid) getArrayElement(boids, j);
    Vector2 dp = Vector2Subtract(temp1->position, temp2->position);
    float dp_len_sqr = Vector2Length(dp) * Vector2Length(dp);
    if (i != j &&
        Vector2Distance(temp1->position, temp2->position) < separation_radius) {
      neighbor_count++;
      sum = Vector2Add(sum, Vector2Scale(dp, 1.0f / dp_len_sqr));
    }
  }
  if (neighbor_count > 0) {
    temp1->accelaration =
        Vector2Add(temp1->accelaration,
                   Vector2Scale(sum, separation_strength / neighbor_count));
  }
}

void applyAlignment(int i) {
  int neighbor_count = 0;
  Vector2 sum = {0};
  Boid *temp1 = NULL;
  Boid *temp2 = NULL;
  temp1 = CAST(Boid) getArrayElement(boids, i);
  for (size_t j = 0; j < boids->size; j++) {
    temp2 = CAST(Boid) getArrayElement(boids, j);
    if (i != j &&
        Vector2Distance(temp1->position, temp2->position) < alignment_radius) {
      neighbor_count++;
      sum = Vector2Add(sum, Vector2Subtract(temp2->velocity, temp1->velocity));
    }
  }
  if (neighbor_count > 0) {
    temp1->accelaration =
        Vector2Add(temp1->accelaration,
                   Vector2Scale(sum, alignment_strength / neighbor_count));
  }
}

void warp(int i) {
  Boid *temp = CAST(Boid) getArrayElement(boids, i);
  if (temp->position.x > WIDTH + WARPING_THRESHOLD) {
    temp->position.x = -WARPING_THRESHOLD;
  }
  if (temp->position.x < -WARPING_THRESHOLD) {
    temp->position.x = WIDTH + WARPING_THRESHOLD;
  }
  if (temp->position.y > HEIGHT + WARPING_THRESHOLD) {
    temp->position.y = -WARPING_THRESHOLD;
  }
  if (temp->position.y < -WARPING_THRESHOLD) {
    temp->position.y = HEIGHT + WARPING_THRESHOLD;
  }
}

void avoidOpsticals(int i) {
  int neighbor_count = 0;
  Vector2 sum = {0, 0};
  Boid *temp_b = NULL;
  Vector2 *temp_o = NULL;
  bool deleted = false;
  temp_b = CAST(Boid) getArrayElement(boids, i);
  for (size_t j = 0; j < opsticals->size; j++) {
    temp_o = CAST(Vector2) getArrayElement(opsticals, j);
    Vector2 dp = Vector2Subtract(temp_b->position, *temp_o);
    float dp_len_sqr = Vector2Length(dp);
    if (Vector2Distance(temp_b->position, *temp_o) < avoidance_radius) {
      neighbor_count++;
      sum = Vector2Add(sum, Vector2Scale(dp, 1.0f / dp_len_sqr));
    }
    if (Vector2Distance(temp_b->position, *temp_o) < deadly_radius) {
      removeElementAtId(boids, i);
      deleted = true;
    }
  }
  if (neighbor_count > 0 && !deleted) {
    temp_b->accelaration =
        Vector2Add(temp_b->accelaration, Vector2Scale(sum, avoidance_strength));
  }
}

void addBoidOrOpstical(Texture2D tex) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && add_boid) {
    Boid temp({GetMousePosition().x, GetMousePosition().y},
              {(float)(rand() % 100), (float)(rand() % 100)}, {0},
              (float)rand() / RAND_MAX * 360, tex);
    pushIntoArray(boids, &temp);
  } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && add_opstical) {
    Vector2 temp = {GetMousePosition().x, GetMousePosition().y};
    pushIntoArray(opsticals, &temp);
  }
}

void updateBoid(float dt) {
  Boid *temp = NULL;
  for (size_t i = 0; i < boids->size; i++) {
    temp = CAST(Boid) getArrayElement(boids, i);
    float angle = 0, d_theta = 0;
    // reset accelaration
    temp->accelaration = {0, 0};
    // applying the forces
    applyCohegen(i);
    applySeparation(i);
    applyAlignment(i);
    avoidOpsticals(i);
    warp(i);
    // updating the velocity and position
    temp->velocity =
        Vector2Add(temp->velocity, Vector2Scale(temp->accelaration, dt));
    if (Vector2Length(temp->velocity) >= MAX_VELOCITY) {
      temp->velocity =
          Vector2Scale(Vector2Normalize(temp->velocity), MAX_VELOCITY);
    }
    temp->position =
        Vector2Add(temp->position, Vector2Scale(temp->velocity, dt));
    angle = atan2(temp->velocity.y, temp->velocity.x) / PI * 180 + 90;
    d_theta = angle - temp->rotation;
    temp->rotation_vel = d_theta * DAMPANING * rotation_rate;
    temp->rotation += temp->rotation_vel * dt;
  }
}
void checkBoxdropdown(Vector2 mous_pos, int number_of_elements) {
  panel_height = (element_hight + margins) * number_of_elements + margins;
  Rectangle button = {(float)WIDTH / 6 - 100, (float)margins, 200, 50};
  Rectangle pannel = {button.x - 50, 70, button.width + 100,
                      (float)panel_height};
  if (show_dropdown_checkboxes && !CheckCollisionPointRec(mous_pos, pannel) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
      !CheckCollisionPointRec(mous_pos, button)) {
    show_dropdown_checkboxes = false;
  }

  if (GuiButton(button, "show radii")) {
    show_dropdown_checkboxes = !show_dropdown_checkboxes;
  }
  if (show_dropdown_checkboxes) {
    DrawRectangleRounded(pannel, 0.1f, 20, BLACK);
    DrawRectangleRounded(
        {pannel.x + 2, pannel.y + 2, pannel.width - 4, pannel.height - 4}, 0.1f,
        20, WHITE);
    GuiCheckBox((Rectangle){pannel.x + margins, pannel.y + margins,
                            (float)element_hight, (float)element_hight},
                "cohegen", &show_cohegen_radius);
    GuiCheckBox((Rectangle){pannel.x + margins,
                            pannel.y + margins * 2 + element_hight,
                            (float)element_hight, (float)element_hight},
                "separation", &show_separation_radius);
    GuiCheckBox((Rectangle){pannel.x + margins,
                            pannel.y + margins * 3 + element_hight * 2,
                            (float)element_hight, (float)element_hight},
                "alignment", &show_alignment_radius);
    GuiCheckBox((Rectangle){pannel.x + margins,
                            pannel.y + margins * 4 + element_hight * 3,
                            (float)element_hight, (float)element_hight},
                "avoidance", &show_avoidance_radius);
    GuiCheckBox((Rectangle){pannel.x + margins,
                            pannel.y + margins * 5 + element_hight * 4,
                            (float)element_hight, (float)element_hight},
                "dead", &show_deadly_radius);
    GuiCheckBox((Rectangle){pannel.x + margins,
                            pannel.y + margins * 6 + element_hight * 5,
                            (float)element_hight, (float)element_hight},
                "add boid", &add_boid);
    GuiCheckBox((Rectangle){pannel.x + margins,
                            pannel.y + margins * 7 + element_hight * 6,
                            (float)element_hight, (float)element_hight},
                "add opstical", &add_opstical);
  }
}
void forceStrengthdropdown(Vector2 mous_pos, int number_of_elements) {
  panel_height = (element_hight + margins) * number_of_elements + margins;
  Rectangle button = {(float)WIDTH / 6 * 3 - 100, (float)margins, 200, 50};
  Rectangle pannel = {button.x - 50, 70, button.width + 100,
                      (float)panel_height};
  if (show_dropdown_strength_slider &&
      !CheckCollisionPointRec(mous_pos, pannel) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
      !CheckCollisionPointRec(mous_pos, button)) {
    show_dropdown_strength_slider = false;
  }

  if (GuiButton(button, "controle strength")) {
    show_dropdown_strength_slider = !show_dropdown_strength_slider;
  }
  if (show_dropdown_strength_slider) {
    DrawRectangleRounded(pannel, 0.1f, 20, BLACK);
    DrawRectangleRounded(
        {pannel.x + 2, pannel.y + 2, pannel.width - 4, pannel.height - 4}, 0.1f,
        20, WHITE);
    GuiLabel({pannel.x + 20, pannel.y, pannel.width - 40, (float)element_hight},
             "cohegen strength");
    GuiSlider({pannel.x + 20, pannel.y + element_hight, pannel.width - 40,
               (float)element_hight},
              "0", "5", &cohegen_strength, 0.0f, 5.0f);
    GuiLabel({pannel.x + 20, pannel.y + element_hight * 2, pannel.width - 40,
              (float)element_hight},
             "alignment strength");
    GuiSlider({pannel.x + 20, pannel.y + element_hight * 3, pannel.width - 40,
               (float)element_hight},
              "0", "5", &alignment_strength, 0.0f, 5.0f);
    GuiLabel({pannel.x + 20, pannel.y + element_hight * 4, pannel.width - 40,
              (float)element_hight},
             "separation strength");
    GuiSlider({pannel.x + 20, pannel.y + element_hight * 5, pannel.width - 40,
               (float)element_hight},
              "0", "15", &separation_strength, 0.0f, 15000.0f);
    GuiLabel({pannel.x + 20, pannel.y + element_hight * 6, pannel.width - 40,
              (float)element_hight},
             "avoidance strength");
    GuiSlider({pannel.x + 20, pannel.y + element_hight * 7, pannel.width - 40,
               (float)element_hight},
              "0", "5", &avoidance_strength, 0.0f, 5000.0f);
  }
}
void forceRadiusdropdown(Vector2 mous_pos, int number_of_elements) {
  panel_height = (element_hight + margins) * number_of_elements + margins;
  Rectangle button = {(float)WIDTH / 6 * 5 - 100, (float)margins, 200, 50};
  Rectangle pannel = {button.x - 50, 70, button.width + 150,
                      (float)panel_height};
  if (show_dropdown_radius_slider &&
      !CheckCollisionPointRec(mous_pos, pannel) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
      !CheckCollisionPointRec(mous_pos, button)) {
    show_dropdown_radius_slider = false;
  }

  if (GuiButton(button, "controle raii")) {
    show_dropdown_radius_slider = !show_dropdown_radius_slider;
  }
  if (show_dropdown_radius_slider) {
    DrawRectangleRounded(pannel, 0.1f, 20, BLACK);
    DrawRectangleRounded(
        {pannel.x + 2, pannel.y + 2, pannel.width - 4, pannel.height - 4}, 0.1f,
        20, WHITE);
    GuiLabel({pannel.x + 20, pannel.y, pannel.width - 40, (float)element_hight},
             "cohegen radius");
    GuiSlider({pannel.x + 20, pannel.y + element_hight, pannel.width - 70,
               (float)element_hight},
              "0", "300", &cohegen_radius, 0.0f, 300.0f);
    GuiLabel({pannel.x + 20, pannel.y + element_hight * 2, pannel.width - 70,
              (float)element_hight},
             "alignment radius");
    GuiSlider({pannel.x + 20, pannel.y + element_hight * 3, pannel.width - 70,
               (float)element_hight},
              "0", "300", &alignment_radius, 0.0f, 300.0f);
    GuiLabel({pannel.x + 20, pannel.y + element_hight * 4, pannel.width - 70,
              (float)element_hight},
             "separation radius");
    GuiSlider({pannel.x + 20, pannel.y + element_hight * 5, pannel.width - 70,
               (float)element_hight},
              "0", "300", &separation_radius, 0.0f, 300.0f);
    GuiLabel({pannel.x + 20, pannel.y + element_hight * 6, pannel.width - 70,
              (float)element_hight},
             "avoidance radius");
    GuiSlider({pannel.x + 20, pannel.y + element_hight * 7, pannel.width - 70,
               (float)element_hight},
              "0", "300", &avoidance_radius, 0.0f, 300.0f);
    GuiLabel({pannel.x + 20, pannel.y + element_hight * 8, pannel.width - 70,
              (float)element_hight},
             "deadly radius");
    GuiSlider({pannel.x + 20, pannel.y + element_hight * 9, pannel.width - 70,
               (float)element_hight},
              "0", "300", &deadly_radius, 0.0f, 300.0f);
  }
}
void addVariableControllers() {
  Vector2 mous_pos = GetMousePosition();
  checkBoxdropdown(mous_pos, 7);
  forceStrengthdropdown(mous_pos, 6);
  forceRadiusdropdown(mous_pos, 8);
}

// NOTE: main function
int main() {
  InitWindow(1280, 720, "boid simulation");
  int label1_size = MeasureText("enter the inisial number of boids 0-1000", 20);
  int label2_size =
      MeasureText("enter the inisial number of opsticals 0-1000", 20);
  int label3_size = MeasureText("ENTER A VALID INPUT ,please", 20);
  bool edit1 = false, edit2 = false;
  bool show_error = false;
  Rectangle text_input1 = {(float)WIDTH / 4 - 200, (float)HEIGHT / 3 + 60, 400,
                           50};
  Rectangle text_input2 = {(float)WIDTH / 4 * 3 - 200, (float)HEIGHT / 3 + 60,
                           400, 50};
  GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
  while (starting_screen && !WindowShouldClose()) {
    if (CheckCollisionPointRec(GetMousePosition(), text_input1) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      edit1 = true;
      edit2 = false;
    }
    if (CheckCollisionPointRec(GetMousePosition(), text_input2) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      edit1 = false;
      edit2 = true;
    }
    BeginDrawing();
    ClearBackground(SKYBLUE);
    GuiLabel({(float)WIDTH / 4 - (float)label1_size / 2, (float)HEIGHT / 3,
              (float)label1_size, 50},
             "enter the inisial number of boids 0-1000");
    GuiLabel({(float)WIDTH / 4 * 3 - (float)label1_size / 2, (float)HEIGHT / 3,
              (float)label2_size, 50},
             "enter the inisial number of opsticals 0-1000");
    GuiValueBox(text_input1, NULL, &inisial_boid_count, 0, 1000, edit1);
    GuiValueBox(text_input2, NULL, &inisial_opstical_count, 0, 1000, edit2);
    if (GuiButton({(float)WIDTH * 3 / 4, (float)HEIGHT * 2 / 3, 100, 50},
                  "Submit")) {
      if (inisial_boid_count > 0 && inisial_boid_count <= 1000 &&
          inisial_opstical_count > 0 && inisial_opstical_count <= 1000) {
        starting_screen = false;
      } else {
        show_error = true;
      }
    };
    if (show_error) {
      int default_color = GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL);
      GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0xff0000ff);
      GuiLabel({(float)WIDTH / 2 - (float)label3_size / 2, 100,
                (float)label3_size, 50},
               "ENTER A VALID INPUT ,please");
      GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, default_color);
    }
    EndDrawing();
  }
  srand(time(0));
  Texture2D tex = LoadTexture("./boid.png");
  createBoids(tex);
  createOpsticals();
  while (!WindowShouldClose() && !starting_screen) {
    float dt = GetFrameTime();
    addBoidOrOpstical(tex);
    updateBoid(dt);
    BeginDrawing();
    ClearBackground(SKYBLUE);
    drawBoids();
    drawOpsticals();
    addVariableControllers();
    EndDrawing();
  }
  deleteOpsticals();
  deleteBoids();
  return 0;
}
