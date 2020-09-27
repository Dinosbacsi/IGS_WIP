/*
======================================================================================
    Ipari Vállalatok Termelési és Gyártási Folyamatainak Grafikus Szimulációja
                            szakdolgozat program
                        Pente Viktor Domonkos (ENOMZK)
======================================================================================
*/

/*
======================================================================================
    Headerek
*/
    //SDL, OpenGL, SOIL
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include <GL\GLU.h>
#include <freeglut.h>
#include <SOIL.h>

    //Általános headerek
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

    //Saját headerek
#include "object.h"
#include "model.h"

/*
======================================================================================
    Globális változók
*/
    //SDL ablak és OpenGL kontextus
SDL_Window* gWindow = NULL;
SDL_GLContext gContext;
    //SDL esemény változó
SDL_Event e;
    //Ablak méret
int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;
    //SDL Mixer csatornák száma
int mix_channels = 5;
    //Főciklus futásáról döntő változó
bool running = 1;
    //Egérgomb lenyomás változó
bool mouse_left_clicked = false;
    //Játékmód változók
enum mode{normal, build, road, bulldoze};
enum mode game_mode;
    //Debug mód változó
bool debug = 0;
    //Menükezelő változók
int menu_highlighted_building_category = 1;
int menu_selected_building_category = -1;
int menu_highlighted_building_type = 1;
int menu_selected_building_type = -1;
int bt_c;
    //Kamera
struct Camera camera;
    //Kurzor
struct Cursor cursor;
struct Virtual_Cursor v_cursor;
    //Alap fénybeállítások
GLfloat light_pos_default[] = {0.5, 1.0, 1.0, 0.0};           //fény pozíciója
GLfloat light_color_default[] = {1.0, 1.0, 1.0, 1.0};   //fény színe
GLfloat global_ambient_light[] = {0.3, 0.3, 0.3, 1.0};  //globális fény
    //Négyzetrács háló változói
#define map_width 50
#define map_length 100
    //Épület limit
#define building_limit 5000
    //Út node limit
#define node_limit 5000
#define segment_limit 5000
// IDEIGLENES(?)
    //Textúra változók
GLuint tex_grass;
GLuint tex_sky;
GLuint tex_igs_warehouse_1;
GLuint tex_igs_warehouse_2;
GLuint tex_igs_factory_1;
GLuint tex_igs_tank_1;
GLuint tex_igs_truck_small_box;
GLuint tex_igs_road_new;
GLuint tex_igs_road_new2;
    // Modell változók
struct Model test_model;
struct Model igs_warehouse_1;
struct Model igs_warehouse_2;
struct Model igs_factory_1;
struct Model igs_tank_1;
struct Model test_truck;
struct Model test_wheel;
struct Model igs_road_straight;
struct Model igs_road_curve;
struct Model igs_road_3_way;
struct Model igs_road_4_way;
struct Model igs_road_dead_end;
    //Tile-ok
Tile test_tile;
Tile tiles[map_width][map_length];
    //Épületek
Building test_building;
Building new_building;
Building igs_warehouse_small;
Building igs_warehouse_medium;
Building igs_factory_small;
Building igs_tank_small;
Building buildings[building_limit];
    //Utak - IDEIGLENES
Road_Segment test_road;
Road_Segment test_road2;
Node test_A;
Node test_B;
    //Épület kategóriák, típusok
Building building_types[50];
char building_category_names_UPPER_CASE[][30]={"SEMMI", "RAKTAR", "FELDOLGOZO UZEM", "GYAR", "IRODA"};
building_category building_category_list[] = {nothing, warehouse, processing_plant, factory, office};
    //Utak
Node road_nodes[map_width][map_length];
Road_Segment road_segments[segment_limit];
Node new_segment_A;
Node new_segment_B;
Road_Segment new_segment;
    //Járművek
struct Vehicle test_vehicle;
    // Felirat
char test[50] = "TESZT FELIRAT";
char text[50];
char text_fps[10];

/*
======================================================================================
    Függvény prototípusok
*/
void Initialize_SDL();
void Initialize_SDL_Mixer();
void Initialize_OpenGL();
void Initialize_Textures();
void Initialize_Models();
void Initialize_Map();
void Event_Handler();
void Mouse_Handler();
void Build_Mode_Handler();
void Road_Mode_Handler();
void Bulldoze_Mode_Handler();
void Render_Scene();
void Render_Interface();
void Render_Bitmap_String(int x, int y, int z, void *font, char *string, float r, float g, float b);
void Render_Bitmap_String_With_Backdrop(int x, int y, int z, void *font, char *string, float r, float g, float b, float br, float bg, float bb);
void Set_2D_Projection();
void Restore_3D_Projection();
void Reshape();
void Close();


/*
======================================================================================
    Main függvény
*/
int main( int argc, char* args[] )
{

    // SDL inicializáció ---------------------------------------------------------
    Initialize_SDL();
    // SDL mixer inicializáció ---------------------------------------------------
    Initialize_SDL_Mixer();
    // OpenGL inicializáció ------------------------------------------------------
    Initialize_OpenGL();
    // GLUT inicializáció
    glutInit(&argc, args);
    // Textúrák betöltése
    Initialize_Textures();
    // Modellek betöltése
    Initialize_Models();
    // Pálya inicializáláas
    Initialize_Map();


    camera.pos.x = 25;
    camera.pos.y = 50;
    camera.pos.z = 0;
    camera.angle_h = 0;
    camera.angle_v = 1;
    camera.distance = 5;
    camera.speed = 0.1;
    camera.sensitivity = 0.005;
    camera.field_of_view = 30;

    // Program futása
    while(running)
    {
        // SDL esemény kezelés
        Event_Handler();
        // Egérmozgatás kezelés
        Mouse_Handler();
        // Építő mód
        if(game_mode == build)
        {
            Build_Mode_Handler();
        }
        if(game_mode == road)
        {
            Road_Mode_Handler();
        }
        // Bontó mód
        if(game_mode == bulldoze)
        {
            Bulldoze_Mode_Handler();
        }

        // Renderelés
        Render_Scene();
        // Ablak átméretezés
        Reshape();
    }

    // SDL leállítása, erőforrások felszabadítása, ablak bezárása
    Close();

    return 0;
}

/*
======================================================================================
    Függvények
*/
void Initialize_SDL()
{
    //Inicializáció
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("HIBA! SDL nem inicializálódott! \nSDL hibakód: %s \n", SDL_GetError());
    }

    //Ablak és kontextus létrehozása
    gWindow = SDL_CreateWindow("IGS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    gContext = SDL_GL_CreateContext(gWindow);

    //OpenGL verzió beállítása (2.1)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    //OpenGL élsimítás beállítása
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    //Átméretezhető ablak beállítása
    SDL_SetWindowResizable(gWindow, SDL_TRUE);
    //VSync beállítása
    if(SDL_GL_SetSwapInterval(1) < 0)
    {
        printf("HIBA! VSync nem állítódott be! \nSDL hibakód: %s \n", SDL_GetError());
    }
}

void Initialize_SDL_Mixer()
{
    // Inicializáció
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
    {
        printf("HIBA! SDL Mixer nem inicializálódott! \nSDL Mixer hibakód: %s\n", Mix_GetError());
    }

    // Csatornák számának beállítása
    if(Mix_AllocateChannels(mix_channels) < 0)
    {
        printf("HIBA! Nem lehetett beállítani a hang csatornák számát! \nSDL hibakód: %s\n", SDL_GetError());
    }

    // Hangeffekt fájlok betöltése

    // Zene fájlok betöltése
}

void Initialize_OpenGL()
{
    GLenum error = GL_NO_ERROR;

    // Modelview Mátrix inicializálás
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	// Hiba ellenőrzés
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "HIBA! OpenGL nem inicializálódott! \nOpenGL hibakód: %s\n", gluErrorString( error ) );
	}

	//OpenGL beállítások
    //glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_MULTISAMPLE_ARB);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//Árnyékolás módjának beállítása
    glShadeModel(GL_SMOOTH);

    //Fény beállítás
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos_default);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color_default);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient_light);

	// Clear Color beállítás
	glClearColor(0.8, 0.8, 0.8, 0.0);
    glClearDepth(1.0);
}

void Initialize_Textures()
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Textúra beállítások
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_2D);

	// Textúrák betöltése
	tex_grass = SOIL_load_OGL_texture
	(
        "textures/ground_grass.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );
    tex_sky = SOIL_load_OGL_texture
	(
        "textures/skybox_1.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );
    tex_igs_warehouse_1 = SOIL_load_OGL_texture
	(
        "textures/igs_warehouse_1.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );
    tex_igs_warehouse_2 = SOIL_load_OGL_texture
	(
        "textures/igs_warehouse_2.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );
    tex_igs_factory_1 = SOIL_load_OGL_texture
	(
        "textures/igs_factory_1.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );
    tex_igs_tank_1 = SOIL_load_OGL_texture
	(
        "textures/igs_tank_1.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );
    tex_igs_truck_small_box = SOIL_load_OGL_texture
	(
        "textures/igs_truck_small_box.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );
    tex_igs_road_new = SOIL_load_OGL_texture
	(
        "textures/igs_road_new.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );
    tex_igs_road_new2 = SOIL_load_OGL_texture
	(
        "textures/igs_road_new2.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );

    glBindTexture(GL_TEXTURE_2D, tex_grass);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
}

void Initialize_Models()
{
    Load_Model("models/igs_warehouse_1.obj", &test_model, tex_igs_warehouse_1);
    Load_Model("models/igs_warehouse_1.obj", &igs_warehouse_1, tex_igs_warehouse_1);
    Load_Model("models/igs_warehouse_2.obj", &igs_warehouse_2, tex_igs_warehouse_2);
    Load_Model("models/igs_factory_1.obj", &igs_factory_1, tex_igs_factory_1);
    Load_Model("models/igs_tank_1.obj", &igs_tank_1, tex_igs_tank_1);
    Load_Model("models/igs_truck_small_box.obj", &test_truck, tex_igs_truck_small_box);
    Load_Model("models/igs_truck_small_wheel.obj", &test_wheel, tex_igs_truck_small_box);
    Load_Model("models/igs_road_straight.obj", &igs_road_straight, tex_igs_road_new);
    Load_Model("models/igs_road_curve.obj", &igs_road_curve, tex_igs_road_new);
    Load_Model("models/igs_road_3_way.obj", &igs_road_3_way, tex_igs_road_new);
    Load_Model("models/igs_road_4_way.obj", &igs_road_4_way, tex_igs_road_new);
    Load_Model("models/igs_road_end.obj", &igs_road_dead_end, tex_igs_road_new2);
}

void Initialize_Map()
{
    // Ciklusváltozók
    int i, j;
    // Ciklus a Tile-ok inicializálására
    for(i=0; i<map_width; i++)
    {
        for(j=0; j<map_length; j++)
        {
            tiles[i][j].pos.x = i;
            tiles[i][j].pos.y = j;
            tiles[i][j].occupied = 0;

            road_nodes[i][j].exists = false;
        }
    }

    // ÉPÜLET STRUKTÚRA TESZT
    /*
    test_building.building_model = test_model;
    test_building.facing_direction = north;
    test_building.pos.x = 25;
    test_building.pos.y = 50;
    */

    // ÉPÜLET TESZT 2
    /*
    Place_Building_OLD(test_model, warehouse, 25, 50, 1, 1, north, buildings, building_limit, tiles);
    Place_Building_OLD(test_model, warehouse, 24, 52, 1, 1, west, buildings, building_limit, tiles);
    Place_Building_OLD(test_model, warehouse, 23, 52, 1, 1, west, buildings, building_limit, tiles);
    */

    // ÉPÜLET TESZT 3
    // Épületek inicializálása
    Make_Building_Type(&igs_warehouse_small, "SMALL WAREHOUSE", igs_warehouse_1, warehouse, 1, 1);
    Make_Building_Type(&igs_warehouse_medium, "MEDIUM WAREHOUSE", igs_warehouse_2, warehouse, 3, 3);

    Make_Building_Type(&building_types[1], "KIS RAKTAR", igs_warehouse_1, warehouse, 1, 1);
    Make_Building_Type(&building_types[2], "KOZEPES RAKTAR", igs_warehouse_2, warehouse, 3, 3);
    Make_Building_Type(&building_types[3], "KIS TARTALY", igs_tank_1, warehouse, 1, 1);
    Make_Building_Type(&building_types[4], "KIS GYAR", igs_factory_1, factory, 2, 2);

    // JÁRMŰ STRUKTÚRA TESZT
    test_vehicle.vehicle_model = test_truck;
    test_vehicle.wheel_model = test_wheel;
    test_vehicle.pos.x = 25;
    test_vehicle.pos.y = 50.8;
    test_vehicle.pos.z = 0;
    test_vehicle.rotate.z = -10;

    test_vehicle.wheel[0].x = 0.32;
    test_vehicle.wheel[1].x = 0.32;
    test_vehicle.wheel[2].x = 0;
    test_vehicle.wheel[3].x = 0;

    test_vehicle.wheel[0].y = 0.065;
    test_vehicle.wheel[1].y = -0.065;
    test_vehicle.wheel[2].y = 0.065;
    test_vehicle.wheel[3].y = -0.065;

    test_vehicle.wheel[0].z = 0.03;
    test_vehicle.wheel[1].z = 0.03;
    test_vehicle.wheel[2].z = 0.03;
    test_vehicle.wheel[3].z = 0.03;

    // ÚT TESZT
    /*
    test_A.exists = true;
    test_A.pos.x = 20;
    test_A.pos.y = 25;
    test_B.exists = 1;
    test_B.pos.x = 20;
    test_B.pos.y = 35;

    test_road.exists = true;
    test_road.A = &test_A;
    test_road.B = &test_B;
    test_road.length = 10;
    */

    /*
    road_nodes[10][10].exists = true;
    road_nodes[10][10].pos.x = 10;
    road_nodes[10][10].pos.y = 10;
    road_nodes[10][10].connection_east.x = 10;
    road_nodes[10][10].connection_east.y = 20;

    road_nodes[10][20].exists = true;
    road_nodes[10][20].pos.x = 10;
    road_nodes[10][20].pos.y = 20;
    road_nodes[10][20].connection_west.x = 10;
    road_nodes[10][20].connection_west.y = 10;

    test_road.exists = true;
    test_road.A = &road_nodes[10][10];
    test_road.B = &road_nodes[10][20];
    test_road.length = 10;
    */

    /*
    road_nodes[15][15].exists = true;
    road_nodes[15][15].pos.x = 15;
    road_nodes[15][15].pos.y = 15;
    road_nodes[15][15].connection_south.x = 30;
    road_nodes[15][15].connection_south.y = 15;

    road_nodes[30][15].exists = true;
    road_nodes[30][15].pos.x = 30;
    road_nodes[30][15].pos.y = 15;
    road_nodes[30][15].connection_north.x = 15;
    road_nodes[30][15].connection_north.y = 15;

    test_road2.exists = true;
    test_road2.A = road_nodes[15][15];
    test_road2.B = road_nodes[30][15];
    test_road2.length = 15;
    */

    // ÚT TESZT 2
    Place_Road_Segment(road_segments, road_nodes, tiles, 10, 10, 10, 20);
    Place_Road_Segment(road_segments, road_nodes, tiles, 15, 15, 30, 15);
    Place_Road_Segment(road_segments, road_nodes, tiles, 15, 15, 15, 20);
    Place_Road_Segment(road_segments, road_nodes, tiles, 15, 15, 15, 5);
    Place_Road_Segment(road_segments, road_nodes, tiles, 10, 20, 15, 20);
    Place_Road_Segment(road_segments, road_nodes, tiles, 15, 20, 15, 22);
    Place_Road_Segment(road_segments, road_nodes, tiles, 12, 15, 15, 15);
}

void Event_Handler()
{
    while(SDL_PollEvent(&e) != 0)
    {
        switch(e.type)
        {
            // Ablak bezárás esemény
            case SDL_QUIT:
                running = 0;
                break;
            // Egérgörgő esemény
            case SDL_MOUSEWHEEL:
                if(e.wheel.y < 0)
                    camera.distance += 1;
                else if(e.wheel.y > 0)
                    camera.distance -= 1;
                break;
            // Egérgomb lenyomás esemény
            case SDL_MOUSEBUTTONDOWN:
                if(e.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_left_clicked = true;
                }
                else if(e.button.button == SDL_BUTTON_RIGHT)
                {
                    // Pozíció mentése kameraforgatáshoz
                    cursor.last.x = cursor.pos.x;
                    cursor.last.y = cursor.pos.y;
                }
                break;
            // Billentyű lenyomás esemény
            case SDL_KEYDOWN:
                switch( e.key.keysym.sym )
                {
                    // Escape : kilépés
                    case SDLK_ESCAPE:
                        running = 0;
                        break;

                    // F12 : Debug infó ki/bekapcsolása
                    case SDLK_F12:
                        if(debug == 0)
                        {
                            debug = 1;
                            printf("\nDebug info bekapcsolva.");
                        }
                        else
                        {
                            debug = 0;
                            printf("\nDebug info kikapcsolva.");
                        }
                        break;

                    // N : építő mód
                    case SDLK_n:
                        // Építő mód ki/be kapcsolása
                        if(game_mode == build)
                            game_mode = normal;
                        else
                            game_mode = build;

                        // Menü sor változó alaphelyzetbe állítása
                        menu_selected_building_category = -1;

                        // Építendő épület alaphelyzetbe állítása
                        new_building.category = nothing;
                        new_building.facing_direction = north;
                        break;

                    // M : útépító mód
                    case SDLK_m:
                        // Út építő mód ki/be kapcsolása
                        if(game_mode == road)
                        {
                            game_mode = normal;

                            new_segment.exists = false;
                        }
                        else
                            game_mode = road;
                            mouse_left_clicked = false;
                        break;

                    // B : bontó mód
                    case SDLK_b:
                        if(game_mode == bulldoze)
                            game_mode = normal;
                        else
                            game_mode = bulldoze;
                        break;

                    // 1 : építő módban -> raktár
                    case SDLK_1:
                        if(game_mode == build)
                            new_building.category = warehouse;
                            new_building.building_model = test_model;
                        break;

                    // R : építő módban -> forgatás
                    case SDLK_r:
                        if(game_mode == build)
                            switch(new_building.facing_direction)
                            {
                                case north:
                                    new_building.facing_direction = east;
                                    break;
                                case east:
                                    new_building.facing_direction = south;
                                    break;
                                case south:
                                    new_building.facing_direction = west;
                                    break;
                                case west:
                                    new_building.facing_direction = north;
                                    break;
                            }
                        break;

                    // FEL : menüben -> kiválasztás fel
                    case SDLK_UP:
                        if(game_mode == build && menu_selected_building_category == -1)
                        {
                            if(menu_highlighted_building_category > 1)
                                menu_highlighted_building_category--;
                        }
                        else if(game_mode == build && menu_selected_building_type == -1)
                        {
                            if(menu_highlighted_building_type > 1)
                                menu_highlighted_building_type--;
                        }
                        break;

                    // LE : menüben -> kiválasztás le
                    case SDLK_DOWN:
                        if(game_mode == build && menu_selected_building_category == -1)
                        {
                            if(menu_highlighted_building_category < last-1)
                                menu_highlighted_building_category++;
                        }
                        else if(game_mode == build && menu_selected_building_type == -1)
                        {
                            if(menu_highlighted_building_type < bt_c)
                                menu_highlighted_building_type++;
                        }
                        break;

                    // ENTER : menüben -> belépés a kategóriába
                    //       : kategóriában -> épület kiválasztása
                    case SDLK_RETURN:
                        if(game_mode == build && menu_selected_building_category == -1)
                        {
                            menu_selected_building_category = menu_highlighted_building_category;
                        }
                        else if(game_mode == build && menu_selected_building_category != -1 && menu_selected_building_type == -1)
                        {
                            menu_selected_building_type = menu_highlighted_building_type;
                        }
                        break;

                    // BACKSPACE : menüben -> kilépés a kategóriából
                    case SDLK_BACKSPACE:
                        if(game_mode == build && menu_selected_building_category != -1)
                        {
                            menu_highlighted_building_category = menu_selected_building_category;
                            menu_selected_building_category = -1;
                        }
                        else if(game_mode == build && menu_selected_building_category == -1)
                        {
                            game_mode = normal;
                        }
                        break;
                }
        }
    }
}

void Mouse_Handler()
{
    // Kamera forgatás egérnyomásra
    if(SDL_GetMouseState(&cursor.pos.x, &cursor.pos.y)&SDL_BUTTON(3))
    {
        Rotate_Camera(&camera, &cursor, gWindow, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    else
    // Kamera mozgatása élgörgetéssel
    {
        SDL_GetMouseState(&cursor.pos.x, &cursor.pos.y);
        // Kurzor a képernyő felső szélén
        if(cursor.pos.x <= 10)
            Move_Camera_Relative(&camera, up);
        // Kurzor a képernyő alsó szélén
        else if(cursor.pos.x >= SCREEN_WIDTH - 10)
            Move_Camera_Relative(&camera, down);
        // Kurzor a képernyő bal szélén
        if(cursor.pos.y <= 10)
            Move_Camera_Relative(&camera, left);
        // Kurzor a képernyő jobb szélén
        else if(cursor.pos.y >= SCREEN_HEIGHT - 10)
            Move_Camera_Relative(&camera, right);

    }

    // 3D curzor próba
/*
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    winX = (float)cursor.pos.x;
    winY = (float)viewport[3] - (float)cursor.pos.y;
    glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    v_cursor.pos.x = camera.pos.x - sin(camera.angle_h) * posX - cos(camera.angle_h) * posY;
    v_cursor.pos.y = camera.pos.y + cos(camera.angle_h) * posX - sin(camera.angle_h) * posY;
    v_cursor.pos.z = posZ;
    */
}

void Build_Mode_Handler()
{
    // Ha van kiválasztott épülettípus:
    if(menu_selected_building_type != -1)
    {
        int bt_selected = 0;
        int bt_i = 0;

        // Végiglépkedés az épülettípusokon, megkeresve a listájukból a kiválasztottat
        while(bt_selected < menu_selected_building_type)
        {
            if(building_types[bt_i].category == building_category_list[menu_selected_building_category])
            {
                bt_selected++;
            }

            bt_i++;
        }

        if(bt_selected !=0 && bt_selected <50)
        {
            printf("\n %d. epulettipus kivalasztva!", bt_i-1);

            // A megtalált indexű épülettípus behelyezése az új épületbe
            new_building.category = building_types[bt_i-1].category;
            new_building.building_model = building_types[bt_i-1].building_model;
            new_building.size.x = building_types[bt_i-1].size.x;
            new_building.size.y = building_types[bt_i-1].size.y;

            menu_selected_building_type = -1;
        }
    }

    if(new_building.category != nothing)
    {
        //bool tile_is_free = Check_Tile(roundf(v_cursor.pos.x), roundf(v_cursor.pos.y), tiles);

        //Tile-ok ellenőrzése
        bool tile_is_free = true;
        //int lx = (int)(roundf(v_cursor.pos.x) - ((float)new_building.size.x)/2);
        int lx = new_building.pos.x - (new_building.size.x/2);
        //int ly = (int)(roundf(v_cursor.pos.y) - ((float)new_building.size.y)/2);
        int ly = new_building.pos.y - (new_building.size.y/2);
        for(int ix = 0; ix < new_building.size.x; ix++)
        {
            for(int iy = 0; iy < new_building.size.y; iy++)
            {
                if(Check_Tile(lx+ix, ly+iy, tiles) != 0)
                    tile_is_free = false;
            }
        }

        // Új épület pozíciójának meghatározása
        new_building.pos.x = roundf(v_cursor.pos.x);
        new_building.pos.y = roundf(v_cursor.pos.y);

        if(new_building.pos.x < 0 + (new_building.size.x/2))
        {
            new_building.pos.x = 0 + (new_building.size.x/2);
        }
        if(new_building.pos.x > (map_width-1) - (new_building.size.x/2))
        {
            new_building.pos.x = (map_width-1) - (new_building.size.x/2);
        }
        if(new_building.pos.y < 0 + (new_building.size.y/2))
        {
            new_building.pos.y = 0 + (new_building.size.y/2);
        }
        if(new_building.pos.y > (map_length-1) - (new_building.size.y/2))
        {
            new_building.pos.y = (map_length-1) - (new_building.size.y/2);
        }

        // Ha bal egérgomb és tile szabad
        if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1) && tile_is_free)
        {
            // Épület elhelyezése
            Place_Building_OLD(new_building.building_model, new_building.category, new_building.pos.x, new_building.pos.y, new_building.size.x, new_building.size.y, new_building.facing_direction, buildings, building_limit, tiles);

            // Építendő épület alaphelyzetbe állítása
            new_building.category = nothing;
            new_building.facing_direction = north;
        }
    }
}

void Road_Mode_Handler()
{

    if(mouse_left_clicked)
    {
        if(new_segment.exists)
        {
            if(new_segment.A->pos.x == new_segment.B->pos.x)
            {
                int start_y = fminf(new_segment.A->pos.y, new_segment.B->pos.y);
                int mid_y = fminf(new_segment.A->pos.y, new_segment.B->pos.y);
                int end_y = fmaxf(new_segment.A->pos.y, new_segment.B->pos.y);

                for(mid_y = start_y; mid_y < end_y; mid_y++)
                {
                    if(Check_Tile(new_segment.A->pos.x, mid_y, tiles) == 2 || Check_Tile(new_segment.A->pos.x, mid_y, tiles) == 3)
                    {
                        Place_Road_Segment(road_segments, road_nodes, tiles, new_segment.A->pos.x, start_y, new_segment.A->pos.x, mid_y);
                        start_y = mid_y;
                    }
                }
                Place_Road_Segment(road_segments, road_nodes, tiles, new_segment.A->pos.x, start_y, new_segment.A->pos.x, end_y);
            }
            else if(new_segment.A->pos.y == new_segment.B->pos.y)
            {
                int start_x = fminf(new_segment.A->pos.x, new_segment.B->pos.x);
                int mid_x = fminf(new_segment.A->pos.x, new_segment.B->pos.x);
                int end_x = fmaxf(new_segment.A->pos.x, new_segment.B->pos.x);

                for(mid_x = start_x; mid_x < end_x; mid_x++)
                {
                    if(Check_Tile(mid_x, new_segment.A->pos.y, tiles) == 2 || Check_Tile(mid_x, new_segment.A->pos.y, tiles) == 3)
                    {
                        Place_Road_Segment(road_segments, road_nodes, tiles, start_x, new_segment.A->pos.y, mid_x, new_segment.A->pos.y);
                        start_x = mid_x;
                    }
                }
                Place_Road_Segment(road_segments, road_nodes, tiles, start_x, new_segment.A->pos.y, end_x, new_segment.A->pos.y);
            }

            new_segment.exists = false;
        }
        else
        {
            new_segment.exists = true;
            new_segment.A = &new_segment_A;
            new_segment.B = &new_segment_B;
            new_segment.A->pos.x = roundf(v_cursor.pos.x);
            new_segment.A->pos.y = roundf(v_cursor.pos.y);
        }

        mouse_left_clicked = false;
    }
    if(new_segment.exists)
    {
        new_segment.B->pos.x = roundf(v_cursor.pos.x);
        new_segment.B->pos.y = roundf(v_cursor.pos.y);

        if(!(new_segment.A->pos.x == new_segment.B->pos.x) && !(new_segment.A->pos.y == new_segment.B->pos.y))
        {
            if(fabs(new_segment.A->pos.x - roundf(v_cursor.pos.x)) > fabs(new_segment.A->pos.y - roundf(v_cursor.pos.y)))
            {
                new_segment.B->pos.x = roundf(v_cursor.pos.x);
                new_segment.B->pos.y = new_segment.A->pos.y;
            }
            else
            {
                new_segment.B->pos.x = new_segment.A->pos.x;
                new_segment.B->pos.y = roundf(v_cursor.pos.y);
            }
        }

        if(new_segment.A->pos.x == new_segment.B->pos.x && new_segment.A->pos.y == new_segment.B->pos.y)
            new_segment.length = 0;
        else if(new_segment.A->pos.x == new_segment.B->pos.x)
            new_segment.length = fabsf(new_segment.A->pos.y - new_segment.B->pos.y);
        else
            new_segment.length = fabsf(new_segment.A->pos.x - new_segment.B->pos.x);
    }
}

void Bulldoze_Mode_Handler()
{
    if(v_cursor.pos.x > 0 && v_cursor.pos.y > 0 && v_cursor.pos.x < map_width && v_cursor.pos.y < map_length)
    {
        int tile_is_occupied = Check_Tile(roundf(v_cursor.pos.x), roundf(v_cursor.pos.y), tiles);
        if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1) && tile_is_occupied != 0)
        {
            switch (tile_is_occupied)
            {
                // 1 : Épület
                case 1:
                    Bulldoze_Building_OLD(v_cursor, buildings, tiles);
                    break;
                // 2: út Node
                case 2:
                    Delete_Road_Node((int)roundf(v_cursor.pos.x), (int)roundf(v_cursor.pos.y), road_segments, road_nodes, tiles);
                    break;
                // 3: út Segment
                case 3:
                    Delete_Road_Segment(tiles[(int)roundf(v_cursor.pos.x)][(int)roundf(v_cursor.pos.y)].occupied_by_road_segment, road_nodes, tiles);
                    break;
            }
        }
    }

}

void Render_Scene()
{
    // Buffer kiürítése
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    // Fő mátrix push
    glPushMatrix();
        // Kamera pozícionálása
        Position_Camera(&camera);

        // Fény pozícionálása
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos_default);

        // Ciklus a Tile-ok kirajzolására
        int i, j;
        for(i=0; i<map_width; i++)
        {
            for(j=0; j<map_length; j++)
            {
                Draw_Tile(tiles[i][j], tex_grass);
            }
        }
        // 3D kurzor kirajzolása
        Calculate3DCursorLocation(cursor.pos.x, cursor.pos.y, &v_cursor);
        if(debug)
            Draw_3D_Cursor(v_cursor);

        // Ég kirajzolása
        Draw_Skybox(tex_sky);

        // Objektum Kirajzolása
        Draw_Building(test_building);
        Draw_Vehicle(test_vehicle);
        for(i=0; i<building_limit; i++)
        {
            if(buildings[i].exists != 0)
            {
                Draw_Building(buildings[i]);
            }
        }
        if(game_mode == build && new_building.category != nothing)
        {
            Draw_Building(new_building);
        }

        // Utak kirajzolása
        for(i=0; i<segment_limit; i++)
        {
            if(road_segments[i].exists)
            {
                Draw_Road_Segment(road_segments[i], igs_road_straight);
            }
        }
        if(game_mode == road)
        {
            if(new_segment.exists)
            {
                if(new_segment.length > 0)
                {
                    Draw_Road_Segment(new_segment, igs_road_straight);
                    if(new_segment.A->pos.x == new_segment.B->pos.x)
                    {
                        glPushMatrix();
                            glTranslatef(new_segment.A->pos.x, fminf(new_segment.A->pos.y, new_segment.B->pos.y), 0);
                            glRotatef(90, 0, 0, 1);
                            Draw_Model(&igs_road_dead_end);
                        glPopMatrix();

                        glPushMatrix();
                            glTranslatef(new_segment.A->pos.x, fmaxf(new_segment.A->pos.y, new_segment.B->pos.y), 0);
                            glRotatef(-90, 0, 0, 1);
                            Draw_Model(&igs_road_dead_end);
                        glPopMatrix();
                    }
                    else if(new_segment.A->pos.y == new_segment.B->pos.y)
                    {
                        glPushMatrix();
                            glTranslatef(fminf(new_segment.A->pos.x, new_segment.B->pos.x), new_segment.A->pos.y, 0);
                            Draw_Model(&igs_road_dead_end);
                        glPopMatrix();

                        glPushMatrix();
                            glTranslatef(fmaxf(new_segment.A->pos.x, new_segment.B->pos.x), new_segment.A->pos.y, 0);
                            glRotatef(180, 0, 0, 1);
                            Draw_Model(&igs_road_dead_end);
                        glPopMatrix();
                    }
                }
                else
                {
                    glPushMatrix();
                    glTranslatef(roundf(v_cursor.pos.x), roundf(v_cursor.pos.y), 0);
                    Draw_Model(&igs_road_straight);
                    glPopMatrix();
                }
            }
            else
            {
                glPushMatrix();
                glTranslatef(roundf(v_cursor.pos.x), roundf(v_cursor.pos.y), 0);
                Draw_Model(&igs_road_straight);
                glPopMatrix();
            }
        }

        // Node-k kirajzoláas
        for(i=0; i<map_width; i++)
        {
            for(j=0; j<map_length; j++)
            {
                if(road_nodes[i][j].exists)
                {
                    // IDEIGLENES - JOBB LENNE KÜLÖN FÜGGVÉNYBEN
                    glPushMatrix();
                        glTranslatef(road_nodes[i][j].pos.x, road_nodes[i][j].pos.y, 0);
                        switch(road_nodes[i][j].facing_direction)
                        {
                            case north:
                                glRotatef(-90, 0, 0, 1);
                                break;
                            case east:
                                glRotatef(0, 0, 0, 1);
                                break;
                            case south:
                                glRotatef(90, 0, 0, 1);
                                break;
                            case west:
                                glRotatef(180, 0, 0, 1);
                                break;
                        }
                        switch(road_nodes[i][j].type)
                        {
                            case dead_end:
                                Draw_Model(&igs_road_dead_end);
                                break;
                            case straight:
                                Draw_Model(&igs_road_straight);
                                break;
                            case curve:
                                Draw_Model(&igs_road_curve);
                                break;
                            case intersection_3_way:
                                Draw_Model(&igs_road_3_way);
                                break;
                            case intersection_4_way:
                                Draw_Model(&igs_road_4_way);
                                break;
                        }
                    glPopMatrix();
                }
            }
        }

        // Rácsháló kirajzolása
        //Draw_Full_Grid(map_width, map_length);
        if(game_mode == build || game_mode == road)
            Draw_Local_Grid(v_cursor);

        // Tile kijelölés rajzolása
        for(i=0; i<map_width; i++)
        {
            for(j=0; j<map_length; j++)
            {
                if(debug == true)
                {
                    if(tiles[i][j].occupied != 0 || tiles[i][j].occupied_by_building != NULL || tiles[i][j].occupied_by_node != NULL || tiles[i][j].occupied_by_road_segment != NULL)
                    tiles[i][j].highlighted = true;
                }

                if(tiles[i][j].highlighted == true)
                {
                    Draw_Highlight(tiles[i][j]);
                    tiles[i][j].highlighted = false;
                }
            }
        }

        // Interface kirajzolása
        Render_Interface();

    glPopMatrix();
    // Képernyõ frissítés
    SDL_GL_SwapWindow(gWindow);
}

void Render_Interface()
{
    // 2D-s projekció beállítása
    Set_2D_Projection();
    // Fények, textúra kikapcsolása
    glDisable(GL_LIGHTING);
    glBindTexture(GL_TEXTURE_2D,0);
    // Blending bekapcsolása
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Kirajzolás
    switch(game_mode)
    {
        // Alaphelyzet
        case normal:
            Render_Bitmap_String_With_Backdrop(10, 30, 0, GLUT_BITMAP_HELVETICA_18, "EPITES (N)", 1, 1, 1, 0, 0, 0);
            Render_Bitmap_String_With_Backdrop(10, 55, 0, GLUT_BITMAP_HELVETICA_18, "UT EPITES (M)", 1, 1, 1, 0, 0, 0);
            Render_Bitmap_String_With_Backdrop(10, 80, 0, GLUT_BITMAP_HELVETICA_18, "LEBONTAS (B)", 1, 1, 1, 0, 0, 0);
            break;

        // Építő mód
        case build:
            Render_Bitmap_String_With_Backdrop(10, 30, 0, GLUT_BITMAP_HELVETICA_18, "EPITES (N)", 1, 1, 0, 0, 0, 0);
            Render_Bitmap_String_With_Backdrop(10, 55, 0, GLUT_BITMAP_HELVETICA_18, "UT EPITES (M)", 0.8, 0.8, 0.8, 0, 0, 0);
            Render_Bitmap_String_With_Backdrop(10, 80, 0, GLUT_BITMAP_HELVETICA_18, "LEBONTAS (B)", 0.8, 0.8, 0.8, 0, 0, 0);
            break;

        // Út építő mód
        case road:
            Render_Bitmap_String_With_Backdrop(10, 30, 0, GLUT_BITMAP_HELVETICA_18, "EPITES (N)", 0.8, 0.8, 0.8, 0, 0, 0);
            Render_Bitmap_String_With_Backdrop(10, 55, 0, GLUT_BITMAP_HELVETICA_18, "UT EPITES (M)", 1, 1, 0, 0, 0, 0);
            Render_Bitmap_String_With_Backdrop(10, 80, 0, GLUT_BITMAP_HELVETICA_18, "LEBONTAS (B)", 0.8, 0.8, 0.8, 0, 0, 0);
            break;

        // Bontó mód
        case bulldoze:
            Render_Bitmap_String_With_Backdrop(10, 30, 0, GLUT_BITMAP_HELVETICA_18, "EPITES (N)", 0.8, 0.8, 0.8, 0, 0, 0);
            Render_Bitmap_String_With_Backdrop(10, 55, 0, GLUT_BITMAP_HELVETICA_18, "UT EPITES (M)", 0.8, 0.8, 0.8, 0, 0, 0);
            Render_Bitmap_String_With_Backdrop(10, 80, 0, GLUT_BITMAP_HELVETICA_18, "LEBONTAS (B)", 1, 0, 0, 0, 0, 0);
            break;
    }
    // Épületkategóriák kiíráas
    if(game_mode == build)
    {
        // Végiglépkedés az épület kategóriákon, és kiíratás
        for(int bc_i = warehouse; bc_i < last; bc_i++)
        {
            if(menu_highlighted_building_category == bc_i)
            {
                Render_Bitmap_String_With_Backdrop(230, 5 + (bc_i)*25, 0, GLUT_BITMAP_HELVETICA_18, building_category_names_UPPER_CASE[bc_i], 1, 1, 1, 0, 0, 0);
            }
            else
            {
                Render_Bitmap_String_With_Backdrop(230, 5 + (bc_i)*25, 0, GLUT_BITMAP_HELVETICA_18, building_category_names_UPPER_CASE[bc_i], 0.5, 0.5, 0.5, 0, 0, 0);
            }
        }

        // Kiválasztott kategória épülettípusának kiíratása
        if(menu_selected_building_category != -1)
        {
            bt_c = 0;
            for(int bt_i = 0; bt_i < 50; bt_i++)
            {
                if(building_types[bt_i].category == building_category_list[menu_selected_building_category])
                {
                    bt_c++;
                    if(menu_highlighted_building_type == bt_c)
                    {
                        Render_Bitmap_String_With_Backdrop(450, 5 + (bt_c)*25, 0, GLUT_BITMAP_HELVETICA_18, building_types[bt_i].name, 1, 1, 1, 0, 0, 0);
                    }
                    else
                    {
                        Render_Bitmap_String_With_Backdrop(450, 5 + (bt_c)*25, 0, GLUT_BITMAP_HELVETICA_18, building_types[bt_i].name, 0.5, 0.5, 0.5, 0, 0, 0);
                    }
                }
            }
        }
    }

    // Debug infó kiírása
    if(debug == true)
    {
        // 3D kurzor koordinátáinak kiírása
        sprintf(text, "Kurzor X = %d", (int)roundf(v_cursor.pos.x));
        Render_Bitmap_String(SCREEN_WIDTH - 150, 40, 0, GLUT_BITMAP_HELVETICA_18, text, 1, 1, 1);
        sprintf(text, "Kurzor Y = %d", (int)roundf(v_cursor.pos.y));
        Render_Bitmap_String(SCREEN_WIDTH - 150, 60, 0, GLUT_BITMAP_HELVETICA_18, text, 1, 1, 1);

        // FPS kiszámítása és kiírása
        static float fps;
        static float last_time = 0;
        float current_time = GetTickCount() * 0.001f;
        fps++;
        if(current_time - last_time > 1)
        {
            last_time = current_time;
            sprintf(text_fps, "FPS: %d", (int)fps);
            fps = 0;
        }
        Render_Bitmap_String(SCREEN_WIDTH - 150, 20, 0, GLUT_BITMAP_HELVETICA_18, text_fps, 1, 1, 0);
    }

    // Fények visszakapcsolása, blending kikapcsolása
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    // 3D-s projekció visszaállítása
    Restore_3D_Projection();
}

void Render_Bitmap_String(int x, int y, int z, void *font, char *string, float r, float g, float b)
{

    char *c;

    // Szín beállítása
    glColor3f(r, g, b);

    // Pozícionálás
    glLoadIdentity();
    glRasterPos3i(x, y, z);

    // Kiíratás
    for(c = string; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }

    // Szín visszaállítása
    glColor3f(1.0f, 1.0f, 1.0f);
}

void Render_Bitmap_String_With_Backdrop(int x, int y, int z, void *font, char *string, float r, float g, float b, float br, float bg, float bb)
{
    // Karakter változó
    char *c;

    // HÁTTÉR
    // Háttér színének beállítása
    glColor4f(br, bg, bb, 0.5f);
    glLoadIdentity();
    // Kirajzolás
    glBegin(GL_QUADS);
        glVertex3i(x-5, y-20, z);
        glVertex3i(x-5, y+5, z);
        glVertex3i(x+205, y+5, z);
        glVertex3i(x+205, y-20, z);
    glEnd();

    // FELIRAT
    // Felirat színének beállítása
    glColor4f(r, g, b, 1);
    // Pozícionálás
    glLoadIdentity();
    glRasterPos3i(x, y, z);
    // Kiíratás
    for(c = string; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }

    // Szín visszaállítása
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void Set_2D_Projection()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        glMatrixMode(GL_MODELVIEW);
}


void Restore_3D_Projection()
{
        glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

void Reshape()
{
    //Mátrix újratöltése
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //Ablak méretének bekérése, perspektíva, viewport újra beállítása átméretezés után
    SDL_GetWindowSize(gWindow, &SCREEN_WIDTH, &SCREEN_HEIGHT);
    gluPerspective(camera.field_of_view, (GLdouble)SCREEN_WIDTH / (GLdouble)SCREEN_HEIGHT, 0.1, 500.0);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Close()
{
    // Ablak bezárása
    SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	SDL_Quit();
}
