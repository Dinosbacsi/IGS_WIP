#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <SDL.h>
#include <SDL_opengl.h>
#include <GL\GLU.h>
#include <stdbool.h>

#include "model.h"

#define map_width 50
#define map_length 100

    //Irányváltozók (kamera mozgásához)
enum cam_direction{up, down, left, right};
typedef enum {north, south, east, west} direction;
typedef enum {nothing, warehouse} building_type;
typedef enum {none, red, yellow} highlight;

// Pozíció struktúrák
typedef struct vec3
{
    float x, y, z;
} vec3;
typedef struct vec2
{
    float x, y;
} vec2;

// Pozíció struktúrák (integerrel)
typedef struct vec3i
{
    int x, y, z;
} vec3i;
typedef struct vec2i
{
    int x, y;
} vec2i;

/*
======================================================================================
    Objektum struktúrák
*/
struct Cursor
{
    vec2i pos;
    vec2i last;
    vec2i dif;
};

struct Virtual_Cursor
{
    vec2 pos;
};

struct Camera
{
    vec3 pos;
    float angle_h;
    float angle_v;
    float distance;
    float speed;
    float sensitivity;
    int field_of_view;
};

struct Tile
{
    vec2i pos;
    int occupied;
    bool highlighted;
};

struct Building
{
    // Épület pozíciója, területe, forgatási iránya
    vec2i pos;
    vec2i size;
    direction facing_direction;
    // Épület modellje
    struct Model building_model;
    building_type type;
    int exists;
};

struct Vehicle
{
    // Jármû pozíciója
    vec3 pos;
    vec3 rotate;

    // Jármû kerekeinek pozíciója
    // sorrend: BE, JE, BH, JH
    vec3 wheel[4];

    // Jármû modelljei
    struct Model vehicle_model;
    struct Model wheel_model;
};

/*
======================================================================================
    Kamera függvények
*/
// Kamera pozícionálása
void Position_Camera(struct Camera* cam);
// Kamera mozgatása adott helyre
void Move_Camera_To(struct Camera* cam, float cam_pos_x, float cam_pos_y);
// Kamera mozgatás nézethez relatívan
void Move_Camera_Relative(struct Camera* cam, enum cam_direction direction);
// Kamera forgatása egérrel
void Rotate_Camera(struct Camera* cam ,struct Cursor* cursor, SDL_Window* gWindow, int SCREEN_WIDTH, int SCREEN_HEIGHT);

/*
======================================================================================
    Kirajzoló függvények függvények
*/
// 3D-s kurzor kirajzolása
void Draw_3D_Cursor(struct Virtual_Cursor v_cursor);
// Égbolt kirajzolása
void Draw_Skybox(GLuint sky_texture);
// Teljes négyzetrács kirajzolása
void Draw_Full_Grid(int w, int l);
// Kurzor közelében négyzetrács kirajzolása
void Draw_Local_Grid(struct Virtual_Cursor v_cursor);
// Jármû kirajzolása
void Draw_Vehicle(struct Vehicle vehicle);
#endif // OBJECT_H_INCLUDED

/*
======================================================================================
    Épület kezelõ függvények
*/
// Épület létrehozása
void Place_Building(struct Model building_model, building_type type, int x, int y, int size_x, int size_y, direction direction, struct Building buildings[], int building_limit, struct Tile tiles[map_width][map_length]);
// Épület kirajzolása
void Draw_Building(struct Building building);
// Épület lebontása
void Bulldoze_Building(struct Virtual_Cursor v_cursor, struct Building buildings[], struct Tile tiles[map_width][map_length]);
/*
======================================================================================
    Tile kezelõ függvények
*/
// Tile állapotának ellenőrzése
bool Check_Tile(int x, int y, struct Tile tiles[map_width][map_length]);
// Tile kirajzolása
void Draw_Tile(struct Tile tile, GLuint tile_texture);
// Tile kijelölés kirajzolása
void Draw_Highlight(struct Tile tile);
