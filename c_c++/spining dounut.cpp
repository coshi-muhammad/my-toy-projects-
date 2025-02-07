
#include <iostream>
#include <cmath>


const double PI = 3.14159265358979323846;
const int screen_width = 50;
const float theta_spacing = 0.07f;
const float phi_spacing = 0.02f;
const float R1 = 1;
const float R2 = 2;
const float K2 = 5;
const float K1 = screen_width * K2 * 3 / (8 * (R1 + R2));
char output[screen_width][screen_width];
float  zbuffer[screen_width][screen_width];
bool running = true;
void render_frame(float A, float B) {
    for (int i = 0;i < screen_width;i++) {
        for (int j = 0;j < screen_width;j++) {
            output[i][j] = ' ';
            zbuffer[i][j] = 0;
        }
    }
    float cosA = std::cos(A);
    float cosB = std::cos(B);
    float sinA = std::sin(A);
    float sinB = std::sin(B);
    for (float theta = 0;theta < 2 * PI;theta += theta_spacing) {
        float costheta = std::cos(theta);
        float sintheta = std::sin(theta);
        for (float phi = 0;phi < 2 * PI;phi += phi_spacing) {
            float cosphi = std::cos(phi);
            float sinphi = std::sin(phi);
            float circlex = R2 + R1 * costheta;
            float circley = R1 * sintheta;
            float x = circlex * (cosB * cosphi + sinA * sinB * sinphi)
                - circley * cosA * sinB;
            float y = circlex * (sinB * cosphi - sinA * cosB * sinphi)
                + circley * cosA * cosB;
            float z = K2 + cosA * circlex * sinphi + circley * sinA;
            float ooz = 1 / z;
            int xp = (int)(screen_width / 2 + K1 * ooz * x);
            int yp = (int)(screen_width / 2 - K1 * ooz * y);
            float L = cosphi * costheta * sinB - cosA * costheta * sinphi -
                sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi);
            if (L > 0) {
                if (ooz > zbuffer[xp][yp]) {
                    zbuffer[xp][yp] = ooz;
                    int luminance_index = L * 8;
                    output[xp][yp] = ".,-~:;=!*#$@"[luminance_index];
                }
            }
        }
    }
    printf("\x1b[H");
    for (int j = 0; j < screen_width; j++) {
        for (int i = 0; i < screen_width; i++) {
            putchar(output[i][j]);
        }
        putchar('\n');
    }
}
int main(){
    float a = 0.0f, b = 0.0f;
    while (running) {

        render_frame(a,b);
        a += 0.2f;
        b += 0.02f;
    }
    return 0;
}

