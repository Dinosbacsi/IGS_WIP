#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

    //�ltal�nos headerek
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

    //SDL, OpenGL, SOIL
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include <GL\GLU.h>
#include <freeglut.h>
#include <SOIL.h>

#define PI 3.14159

//Ir�nyv�ltoz�k (kamera mozg�s�hoz)
enum cam_direction{up, down, left, right};
// Ir�ny v�ltoz�k
typedef enum {north, south, east, west} direction;

// Poz�ci� strukt�r�k
typedef struct vec3
{
    float x, y, z;
} vec3;
typedef struct vec2
{
    float x, y;
} vec2;
// Poz�ci� strukt�r�k (integerrel)
typedef struct vec3i
{
    int x, y, z;
} vec3i;
typedef struct vec2i
{
    int x, y;
} vec2i;

//
struct Cursor
{
    vec2i pos;
    vec2i last;
    vec2i dif;
};

typedef struct Virtual_Cursor
{
    vec3 pos;
}Virtual_Cursor;

struct Camera
{
    vec3 pos;
    float angle_h;
    float angle_v;
    float distance;
    float speed;
    float sensitivity;
    int field_of_view;
    vec3 POV;
};

#include "map.h"

/*
======================================================================================
    Kamera
*/
// Kamera poz�cion�l�sa
void Position_Camera(struct Camera* cam);
// Kamera mozgat�sa adott helyre
void Move_Camera_To(struct Camera* cam, float cam_pos_x, float cam_pos_y);
// Kamera mozgat�s n�zethez relat�van
void Move_Camera_Relative(struct Camera* cam, enum cam_direction direction);
// Kamera forgat�sa eg�rrel
void Rotate_Camera(struct Camera* cam ,struct Cursor* cursor, SDL_Window* gWindow, int SCREEN_WIDTH, int SCREEN_HEIGHT);

/*
======================================================================================
    3D kurzor
*/
// 3D-s kurzor kirajzol�sa
void Draw_3D_Cursor(struct Virtual_Cursor v_cursor);
// 3D kurzor hely�nek meghat�roz�sa
void Calculate3DCursorLocation(int cursorX, int cursorY, Virtual_Cursor* v_cursor);
// 3D kurzor j�t�kos ter�leten tart�sa
void Restrict_3D_Cursor_To_Player_Area(Virtual_Cursor* v_cursor);

/*
======================================================================================
    N�gyzetr�cs
*/
// Teljes n�gyzetr�cs kirajzol�sa
void Draw_Full_Grid(int w, int l);
// Kurzor k�zel�ben n�gyzetr�cs kirajzol�sa
void Draw_Local_Grid(struct Virtual_Cursor v_cursor);

/*
======================================================================================
    HUD
*/
void Render_Bitmap_String(int x, int y, int z, void *font, char *string, float r, float g, float b);
void Render_Bitmap_String_With_Backdrop(int x, int y, int z, void *font, char *string, float r, float g, float b, float br, float bg, float bb);

/*
======================================================================================
    Random matek
*/
float Distance(float x1, float x2, float y1, float y2);
#endif // UTILITY_H_INCLUDED
