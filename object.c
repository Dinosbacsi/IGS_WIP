#include <SDL.h>
#include <SDL_opengl.h>
#include <GL\GLU.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "object.h"

/*
======================================================================================
    Kamera függvények
*/
void Position_Camera(struct Camera* cam)
{
    gluLookAt(
        // Kamera nézõpontjának koordinátája
        cam->pos.x + (cam->distance * cos(cam->angle_h)), cam->pos.y + (cam->distance * sin(cam->angle_h)), cam->distance * sin(cam->angle_v),
        // Kamera célpontjának koordinátája
        cam->pos.x, cam->pos.y, cam->pos.z,
        // "Fel" iránya
        0.0, 0.0, 1.0
              );
}

void Move_Camera_To(struct Camera* cam, float new_cam_pos_x, float new_cam_pos_y)
{
    cam->pos.x = new_cam_pos_x;
    cam->pos.y = new_cam_pos_y;
}

void Move_Camera_Relative(struct Camera* cam, enum cam_direction direction)
{
    switch(direction)
    {
        case up:
            cam->pos.x -= cam->speed * -sin(cam->angle_h);
            cam->pos.y -= cam->speed * cos(cam->angle_h);
            break;
        case down:
            cam->pos.x += cam->speed * -sin(cam->angle_h);
            cam->pos.y += cam->speed * cos(cam->angle_h);
            break;
        case left:
            cam->pos.x -= cam->speed * cos(cam->angle_h);
            cam->pos.y -= cam->speed * sin(cam->angle_h);
            break;
        case right:
            cam->pos.x += cam->speed * cos(cam->angle_h);
            cam->pos.y += cam->speed * sin(cam->angle_h);
            break;
    }


}

void Rotate_Camera(struct Camera* cam ,struct Cursor* cursor, SDL_Window* gWindow, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    // Kurzormozgatás távolságának meghatározása
    cursor->dif.x = cursor->last.x - cursor->pos.x;
    cursor->dif.y = cursor->last.y - cursor->pos.y;

    // Kamera szögének módosítása
    cam->angle_h += cursor->dif.x * cam->sensitivity;
    cam->angle_v -= cursor->dif.y * cam->sensitivity;

    // Kamera vertikális szögének szabályozása
    if(cam->angle_v > M_PI/2)
        cam->angle_v = M_PI/2;
    else if(cam->angle_v < 0)
        cam->angle_v = 0;
    // Kamera horizontális szögének szabályozása
    if(cam->angle_h > 2*M_PI)
        cam->angle_h = 0;
    else if(cam->angle_h < 0)
        cam->angle_h = 2*M_PI;

    // Kurzor megállítása az ablak széleinél
    if(cursor->pos.x < SCREEN_WIDTH - 20 && cursor->pos.x > 20 && cursor->pos.y < SCREEN_HEIGHT - 20 && cursor->pos.y > 20)
    {
        cursor->last.x = cursor->pos.x;
        cursor->last.y = cursor->pos.y;
    }
    else
    {
        if(cursor->pos.x >= SCREEN_WIDTH - 20)
        {
            SDL_WarpMouseInWindow(gWindow, SCREEN_WIDTH - 20, cursor->pos.y);
            cursor->last.y = cursor->pos.y - cursor->dif.y;
        }
        else if(cursor->pos.x <= 20)
        {
            SDL_WarpMouseInWindow(gWindow, 20, cursor->pos.y);
            cursor->last.y = cursor->pos.y - cursor->dif.y;
        }

        if(cursor->pos.y >= SCREEN_HEIGHT - 20)
        {
            SDL_WarpMouseInWindow(gWindow, cursor->pos.x, SCREEN_HEIGHT - 20);
            cursor->last.x = cursor->pos.x - cursor->dif.x;
        }
        else if(cursor->pos.y <= 20)
        {
            SDL_WarpMouseInWindow(gWindow, cursor->pos.x, 20);
            cursor->last.x = cursor->pos.x - cursor->dif.x;
        }
    }
}

/*
======================================================================================
    Kirajzoló függvények
*/
void Draw_3D_Cursor(struct Virtual_Cursor v_cursor)
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
        glTranslatef(v_cursor.pos.x, v_cursor.pos.y, 0);
        glScalef(0.3, 0.3, 0.3);
        glBegin(GL_TRIANGLES);
            //X tengely
                //piros
            glColor3f(1.0f,0.0f,0.0f);
                //y felöli oldal
            glNormal3f(1, 1, 0.8);
            glVertex3f(1, 0, 0);
            glVertex3f(0, 0.2, 0);
            glVertex3f(0, 0, 0.2);
                //-y felöli oldal
            glNormal3f(1, -1, 0.8);
            glVertex3f(0, -0.2, 0);
            glVertex3f(1, 0, 0);
            glVertex3f(0, 0, 0.2);

            //Y tengely
                //zöld
            glColor3f(0.0f,1.0f,0.0f);
                //x felöli oldal
            glNormal3f(1, 1, 0.8);
            glVertex3f(0, 1, 0);
            glVertex3f(0, 0, 0.2);
            glVertex3f(0.2, 0, 0);
                //-x felöli oldal
            glNormal3f(-1, 1, 0.8);
            glVertex3f(0, 1, 0);
            glVertex3f(-0.2, 0, 0);
            glVertex3f(0, 0, 0.2);

            //Z tengely
                //kék
            glColor3f(0.0f,0.0f,1.0f);
                //-x-y felöli oldal
            glNormal3f(-1, -1, 0.2);
            glVertex3f(0, 0, 0.5);
            glVertex3f(-0.2, 0, 0);
            glVertex3f(0, -0.2, 0);
                //x-y felöli oldal
            glNormal3f(1, -1, 0.2);
            glVertex3f(0, 0, 0.5);
            glVertex3f(0, -0.2, 0);
            glVertex3f(0.2, 0, 0);
                //xy felöli oldal
            glNormal3f(1, 1, 0.2);
            glVertex3f(0, 0, 0.5);
            glVertex3f(0.2, 0, 0);
            glVertex3f(0, 0.2, 0);
                //-xy felöli oldal
            glNormal3f(-1, 1, 0.2);
            glVertex3f(0, 0, 0.5);
            glVertex3f(0, 0.2, 0);
            glVertex3f(-0.2, 0, 0);
        glEnd();
    glPopMatrix();
    glColor3f(1.0f,1.0f,1.0f);
}

void Draw_Skybox(GLuint sky_texture)
{
    // Textúra beállítása
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, sky_texture);

	// Kirajzolás
	glPushMatrix();

        // Észak
        glBegin(GL_QUADS);
            glTexCoord2f(0.5f, 0.33f);
            glVertex3d(250, 250, 250);

            glTexCoord2f(0.5f, 0.66f);
            glVertex3d(250, 250, -250);

            glTexCoord2f(0.75f, 0.66f);
            glVertex3d(250, -250, -250);

            glTexCoord2f(0.75f, 0.33f);
            glVertex3d(250, -250, 250);
        glEnd();

        // Dél
        glBegin(GL_QUADS);
            glTexCoord2f(0.001f, 0.33f);
            glVertex3d(-250, -250, 250);

            glTexCoord2f(0.001f, 0.66f);
            glVertex3d(-250, -250, -250);

            glTexCoord2f(0.25f, 0.66f);
            glVertex3d(-250, 250, -250);

            glTexCoord2f(0.25f, 0.33f);
            glVertex3d(-250, 250, 250);
        glEnd();

        // Kelet
        glBegin(GL_QUADS);
            glTexCoord2f(0.25f, 0.33f);
            glVertex3d(-250, 250, 250);

            glTexCoord2f(0.25f, 0.66f);
            glVertex3d(-250, 250, -250);

            glTexCoord2f(0.5f, 0.66f);
            glVertex3d(250, 250, -250);

            glTexCoord2f(0.5f, 0.33f);
            glVertex3d(250, 250, 250);
        glEnd();

        // Nyugat
        glBegin(GL_QUADS);
            glTexCoord2f(0.75f, 0.33f);
            glVertex3d(250, -250, 250);

            glTexCoord2f(0.75f, 0.66f);
            glVertex3d(250, -250, -250);

            glTexCoord2f(0.999f, 0.66f);
            glVertex3d(-250, -250, -250);

            glTexCoord2f(0.999f, 0.33f);
            glVertex3d(-250, -250, 250);
        glEnd();

        // Fel
        glBegin(GL_QUADS);
            glTexCoord2f(0.25f, 0.0f);
            glVertex3d(250, -250, 250);

            glTexCoord2f(0.25f, 0.33f);
            glVertex3d(250, 250, 250);

            glTexCoord2f(0.5f, 0.33f);
            glVertex3d(-250, 250, 250);

            glTexCoord2f(0.5f, 0.0f);
            glVertex3d(-250, -250, 250);
        glEnd();

	glPopMatrix();

	// Textúra mód visszaállítása
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
}

void Draw_Full_Grid(int w, int l)
{
    // Ciklus változók
    int i, j;

    glPushMatrix();
        glTranslatef(-0.5f, -0.5f, 0);
        for(i=0; i<w; i++)
        {
            glBegin(GL_LINES);
                glVertex3f(i, 0, 0.001);
                glVertex3f(i, l, 0.001);
            glEnd();
        }
        for(j=0; j<l; j++)
        {
            glBegin(GL_LINES);
                glVertex3f(0, j, 0.001);
                glVertex3f(w, j, 0.001);
            glEnd();
        }
    glPopMatrix();
}

void Draw_Local_Grid(struct Virtual_Cursor v_cursor)
{
    // Kerekített pozíció változók
    float x, y;
    x = roundf(v_cursor.pos.x);
    y = roundf(v_cursor.pos.y);

    /*// Koordináta kiíratás
    printf("\n Kurzor X: %f ", v_cursor.pos_x);
    printf("\n Kerekitett X: %f", x);
    printf("\n Kurzor Y: %f", v_cursor.pos_y);
    printf("\n Kerekitett Y: %f", y); */

    // Szín beállítása
    glDisable(GL_LIGHTING);
    glBindTexture(GL_TEXTURE_2D, 0);
    glColor3f(1.0f,1.0f,0.0f);

    // Kirajzolás
    glPushMatrix();
        glTranslatef(x, y, 0.001);

        // X tengely menti vonalak
        glBegin(GL_LINES);
            glVertex3f(-2, -3.5, 0);
            glVertex3f(+2, -3.5, 0);

            glVertex3f(-3, -2.5, 0);
            glVertex3f(+3, -2.5, 0);

            glVertex3f(-4, -1.5, 0);
            glVertex3f(+4, -1.5, 0);

            glVertex3f(-4, -0.5, 0);
            glVertex3f(+4, -0.5, 0);

            glVertex3f(-4, 0.5, 0);
            glVertex3f(+4, 0.5, 0);

            glVertex3f(-4, 1.5, 0);
            glVertex3f(+4, 1.5, 0);

            glVertex3f(-3, 2.5, 0);
            glVertex3f(+3, 2.5, 0);

            glVertex3f(-2, 3.5, 0);
            glVertex3f(+2, 3.5, 0);
        glEnd();

        // Y tengely menti vonalak
        glBegin(GL_LINES);
            glVertex3f(-3.5, -2, 0);
            glVertex3f(-3.5, +2, 0);

            glVertex3f(-2.5, -3, 0);
            glVertex3f(-2.5, +3, 0);

            glVertex3f(-1.5, -4, 0);
            glVertex3f(-1.5, +4, 0);

            glVertex3f(-0.5, -4, 0);
            glVertex3f(-0.5, +4, 0);

            glVertex3f(0.5, -4, 0);
            glVertex3f(0.5, +4, 0);

            glVertex3f(1.5, -4, 0);
            glVertex3f(1.5, +4, 0);

            glVertex3f(2.5, -3, 0);
            glVertex3f(2.5, +3, 0);

            glVertex3f(3.5, -2, 0);
            glVertex3f(3.5, +2, 0);
        glEnd();
    glPopMatrix();

    //Fény, színek visszaállítása
    glEnable(GL_LIGHTING);
    glColor3f(1.0f,1.0f,1.0f);
}

void Draw_Vehicle(struct Vehicle vehicle)
{
    glPushMatrix();
        // Jármű pozícionálása
        glTranslatef(vehicle.pos.x, vehicle.pos.y, vehicle.pos.z);
        glRotatef(vehicle.rotate.z, 0, 0, 1);
        glRotatef(vehicle.rotate.y, 0, 1, 0);
        glRotatef(vehicle.rotate.x, 1, 0, 0);

        // Jármű modell kirajzolása
        Draw_Model(&vehicle.vehicle_model);
        // Jármű kerekeinek kirajzolása
        for(int i = 0; i<=3; i++)
        {
            glPushMatrix();
                glTranslatef(vehicle.wheel[i].x, vehicle.wheel[i].y, vehicle.wheel[i].z);
                if(i == 1 || i == 3)
                    glRotatef(180, 0, 0, 1);
                Draw_Model(&vehicle.wheel_model);
            glPopMatrix();
        }
    glPopMatrix();
}

/*
======================================================================================
    Épület kezelõ függvények
*/
void Place_Building(struct Model building_model, building_type type, int x, int y, int size_x, int size_y, direction direction, struct Building buildings[], int building_limit, struct Tile tiles[map_width][map_length])
{   int i = 0;

    // Első üres rekesz keresése az épületek listájában
    while(buildings[i].exists != 0 &&  i <= building_limit)
    {
        i++;
    }
    // Ha van üres hely az épületek listájában
    if(i <= building_limit)
    {
        // Épület elhelyezése
        buildings[i].exists = 1;
        buildings[i].building_model = building_model;
        buildings[i].type = type;

        buildings[i].pos.x = x;
        buildings[i].size.x = size_x;
        buildings[i].pos.y = y;
        buildings[i].size.y = size_y;
        buildings[i].facing_direction = direction;

        // Tile-ok lefoglalása
        tiles[x][y].occupied = i+1;
    }
}

void Draw_Building(struct Building building)
{
    glPushMatrix();
        // Épület pozícionálása
        glTranslatef(building.pos.x, building.pos.y, 0);
        switch(building.facing_direction)
        {
            case north:
                break;
            case east:
                glRotatef(-90, 0, 0, 1);
                break;
            case south:
                glRotatef(-180, 0, 0, 1);
                break;
            case west:
                glRotatef(-270, 0, 0, 1);
                break;
        }

        // Épület kirajzolása
        Draw_Model(&building.building_model);
    glPopMatrix();
}

void Bulldoze_Building(struct Virtual_Cursor v_cursor, struct Building buildings[], struct Tile tiles[map_width][map_length])
{
    int x = roundf(v_cursor.pos.x);
    int y = roundf(v_cursor.pos.y);
    int i = tiles[x][y].occupied - 1;

    // Épület lebontása
    buildings[i].exists = 0;
    buildings[i].type = none;

    // Tile-ok felszabadítása
    tiles[x][y].occupied = 0;
}

/*
======================================================================================
    Tile kezelõ függvények
*/
bool Check_Tile(int x, int y, struct Tile tiles[map_width][map_length])
{
    // Tile ellenőrzése
    if(tiles[x][y].occupied != 0)
    {
        tiles[x][y].highlighted = true;
        return false;
    }
    else
        return true;
}

void Draw_Tile(struct Tile tile, GLuint tile_texture)
{
    // Tile textúra beállítása
    glBindTexture(GL_TEXTURE_2D, tile_texture);

    glPushMatrix();
        // Mátrix mozgatása a Tile pozíciójára
        glTranslatef(tile.pos.x, tile.pos.y, 0);
        // Tile kirajzolása
        glBegin(GL_QUADS);
            // Normálvektor beállítása
            glNormal3f(0.0f, 0.0f, 1.0f);
            // Lap kirajzolása
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(-0.5f, -0.5f, 0.0f);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(0.5f, -0.5f, 0.0f);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(0.5f, 0.5f, 0.0f);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(-0.5f, 0.5f, 0.0f);
        glEnd();
    glPopMatrix();
}

void Draw_Highlight(struct Tile tile)
{
    // Szín beállítása
    glDisable(GL_LIGHTING);
    glBindTexture(GL_TEXTURE_2D, 0);
    glColor3f(1.0f,0.0f,0.0f);

    glPushMatrix();
        // Mátrix mozgatása a Tile pozíciójára
        glTranslatef(tile.pos.x, tile.pos.y, 0.25);

        // Kirajzolás
        glBegin(GL_QUADS);
            glVertex3f(-0.5f, -0.5f, 0.0f);
            glVertex3f(0.5f, -0.5f, 0.0f);
            glVertex3f(0.5f, 0.5f, 0.0f);
            glVertex3f(-0.5f, 0.5f, 0.0f);
        glEnd();
    glPopMatrix();

    // Fény, színek visszaállítása
    glEnable(GL_LIGHTING);
    glColor3f(1.0f,1.0f,1.0f);
}
