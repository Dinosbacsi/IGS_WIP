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
#include <SOIL.h>

    //Általános headerek
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

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
enum mode{normal, build, bulldoze};
enum mode game_mode;
//bool build_mode = 0;
//bool bulldoze_mode = 0;
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

// IDEIGLENES(?)
    //Textúra változók
GLuint tex_grass;
GLuint tex_sky;
GLuint tex_igs_warehouse_1;
GLuint tex_igs_truck_small_box;
    // Modell változók
struct Model test_model;
struct Model test_truck;
struct Model test_wheel;
    //Tile-ok
struct Tile test_tile;
struct Tile tiles[map_width][map_length];
    //Épületek
struct Building test_building;
struct Building new_building;
struct Building buildings[building_limit];
    //Járművek
struct Vehicle test_vehicle;
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
void Bulldoze_Mode_Handler();
void Render_Scene();
void Render_HUD();
void Reshape();
void Set_2D_Projection();
void Restore_3D_Projection();
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
    glEnable(GL_CULL_FACE);
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
    tex_igs_truck_small_box = SOIL_load_OGL_texture
	(
        "textures/igs_truck_small_box.png",
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
    Load_Model("models/igs_truck_small_box.obj", &test_truck, tex_igs_truck_small_box);
    Load_Model("models/igs_truck_small_wheel.obj", &test_wheel, tex_igs_truck_small_box);
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
        }
    }

    // ÉPÜLET STRUKTÚRA TESZT
    /*test_building.building_model = test_model;
    test_building.facing_direction = north;
    test_building.pos.x = 25;
    test_building.pos.y = 50;*/

    // ÉPÜLET TESZT 2
    Place_Building(test_model, warehouse, 25, 50, 1, 1, north, buildings, building_limit, tiles);
    Place_Building(test_model, warehouse, 24, 52, 1, 1, west, buildings, building_limit, tiles);
    Place_Building(test_model, warehouse, 23, 52, 1, 1, west, buildings, building_limit, tiles);

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
                if(e.button.button == SDL_BUTTON_RIGHT)
                {
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
                    // N : építő mód
                    case SDLK_n:
                        // Építő mód ki/be kapcsolása
                        if(game_mode == build)
                            game_mode = normal;
                        else
                            game_mode = build;

                        // Építendő épület alaphelyzetbe állítása
                        new_building.type = nothing;
                        new_building.facing_direction = north;
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
                            new_building.type = warehouse;
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
}

void Build_Mode_Handler()
{
    if(new_building.type != nothing)
    {
        bool tile_is_free = Check_Tile(roundf(v_cursor.pos.x), roundf(v_cursor.pos.y), tiles);

        new_building.pos.x = roundf(v_cursor.pos.x);
        new_building.pos.y = roundf(v_cursor.pos.y);

        // Ha bal egérgomb és tile szabad
        if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1) && tile_is_free)
        {
            // Épület elhelyezése
            Place_Building(new_building.building_model, new_building.type, new_building.pos.x, new_building.pos.y, new_building.size.x, new_building.size.y, new_building.facing_direction, buildings, building_limit, tiles);

            // Építendő épület alaphelyzetbe állítása
            new_building.type = nothing;
            new_building.facing_direction = north;
        }
    }
}

void Bulldoze_Mode_Handler()
{
    bool tile_is_occupied = Check_Tile(roundf(v_cursor.pos.x), roundf(v_cursor.pos.y), tiles);
    if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1) && !tile_is_occupied)
    {
        Bulldoze_Building(v_cursor, buildings, tiles);
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

        // Tile teszt rajz
        /*test_tile.pos_x = 0;
        test_tile.pos_y = 0;
        glBindTexture(GL_TEXTURE_2D, tex_grass);
        Draw_Tile(test_tile);*/

        // Ciklus a Tile-ok kirajzolására
        int i, j;

        for(i=0; i<map_width; i++)
        {
            for(j=0; j<map_length; j++)
            {
                Draw_Tile(tiles[i][j], tex_grass);
                if(tiles[i][j].highlighted == true)
                {
                    Draw_Highlight(tiles[i][j]);
                    tiles[i][j].highlighted = false;
                }
            }
        }
        // 3D kurzor kirajzolása
        v_cursor.pos.x = camera.pos.x;
        v_cursor.pos.y = camera.pos.y;
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
        if(game_mode == build && new_building.type != nothing)
        {
            Draw_Building(new_building);
        }

        // Rácsháló kirajzolása
        //Draw_Full_Grid(map_width, map_length);
        if(game_mode == build)
            Draw_Local_Grid(v_cursor);
    glPopMatrix();
    // Képernyõ frissítés
    SDL_GL_SwapWindow(gWindow);
}

void Render_HUD()
{

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

void Set_2D_Projection()
{
    // Mátrix váltás
    glMatrixMode(GL_PROJECTION);

    glPushMatrix();
        // Egység mátrix betöltése
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

void Close()
{
    // Ablak bezárása
    SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	SDL_Quit();
}
