/* 
  this file and ds_gui.c are the only ones that should import raylib, we only care about 
  graphics when we are about to render them
*/
#include "raylib.h"

#include "ds_logic.h"

#define WIDTH 800
#define HEIGHT 600

int main()
{
    InitWindow(WIDTH, HEIGHT, "Digital Simulator");
    SetTargetFPS(60);
    while(!WindowShouldClose()) {
    }
    CloseWindow();
    return 0;
}