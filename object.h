#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <SDL.h>
#include <SDL_opengl.h>
#include <GL\GLU.h>
#include <stdbool.h>
#include <string.h>

#include "model.h"

#define map_width 50
#define map_length 100
#define segment_limit 5000

    //Irányváltozók (kamera mozgásához)
enum cam_direction{up, down, left, right};

typedef enum {north, south, east, west} direction;
typedef enum {none, red, yellow} highlight;
    //Épület típusok
typedef enum {nothing, warehouse, processing_plant, factory, office, last} building_category;
    // Node típusok
typedef enum {dead_end, straight, curve, intersection_3_way, intersection_4_way} node_type;

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
};

typedef struct Building
{
    // Épület pozíciója, mérete, iránya
    vec2i pos;
    vec2i size;
    direction facing_direction;

    // Épület kategóriája, modellje
    building_category category;
    struct Model building_model;

    // Épület neve,
    char name[50];
    int exists;
}Building;

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

typedef struct Node
{
    vec2i pos;
    int number_of_connections;
    node_type type;
    direction facing_direction;

    vec2i connection_north;
    vec2i connection_east;
    vec2i connection_south;
    vec2i connection_west;

    bool exists;
}Node;

typedef struct Road_Segment
{
    Node* A;
    Node* B;
    int length;
    direction road_direction;

    bool exists;
}Road_Segment;

typedef struct Tile
{
    vec2i pos;
    int occupied;
    Building* occupied_by_building;
    Road_Segment* occupied_by_road_segment;
    Node* occupied_by_node;
    bool highlighted;
}Tile;

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
// 3D kurzor helyének meghatározása
void Calculate3DCursorLocation(int cursorX, int cursorY, Virtual_Cursor* v_cursor);
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
void Make_Building_Type(Building* building_type, char name[50], struct Model building_model, building_category category, int size_x, int size_y);
// Épület elhelyezés
void Place_Building_OLD(struct Model building_model, building_category category, int x, int y, int size_x, int size_y, direction direction, struct Building buildings[], int building_limit, Tile tiles[map_width][map_length]);
// Épület kirajzolása
void Draw_Building(Building building);
// Épület lebontása
void Bulldoze_Building_OLD(struct Virtual_Cursor v_cursor, Building buildings[], Tile tiles[map_width][map_length]);
/*
======================================================================================
    Út kezelõ függvények
*/
void Place_Road_Segment(Road_Segment road_segments[], Node road_nodes[map_width][map_length], Tile tiles[map_width][map_length], int a_x, int a_y, int b_x, int b_y);
void Split_Road_Segment(Road_Segment* road_to_split, Road_Segment road_segments[], Node road_nodes[map_width][map_length], Tile tiles[map_width][map_length], int x, int y);
void Update_Road_Node(Node* road_node, Tile tiles[map_width][map_length]);
void Draw_Road_Segment(Road_Segment road, struct Model road_model);
void Delete_Road_Segment(Road_Segment* deleted_road_segment, Node road_nodes[map_width][map_length], Tile tiles[map_width][map_length]);
void Delete_Road_Node(int x, int y, Road_Segment road_segments[], Node road_nodes[map_width][map_length], Tile tiles[map_width][map_length]);
/*
======================================================================================
    Tile kezelõ függvények
*/
// Tile állapotának ellenőrzése
//bool Check_Tile(int x, int y, struct Tile tiles[map_width][map_length]);
int Check_Tile(int x, int y, struct Tile tiles[map_width][map_length]);
// Tile kirajzolása
void Draw_Tile(struct Tile tile, GLuint tile_texture);
// Tile kijelölés kirajzolása
void Draw_Highlight(struct Tile tile);
