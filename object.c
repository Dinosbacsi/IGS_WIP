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
        cam->pos.x + (cam->distance * cos(cam->angle_h)) * cos(cam->angle_v), cam->pos.y + (cam->distance * sin(cam->angle_h)) * cos(cam->angle_v), cam->distance * sin(cam->angle_v),
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
    if(cam->angle_v > (M_PI/2)-0.001)
        cam->angle_v = (M_PI/2)-0.001;
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

void Calculate3DCursorLocation(int cursorX, int cursorY, Virtual_Cursor* v_cursor)
{
        GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    winX = (float)cursorX;
    winY = (float)viewport[3] - (float)cursorY;
    glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    v_cursor->pos.x = posX;
    v_cursor->pos.y = posY;
    v_cursor->pos.z = posZ;
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
void Make_Building_Type(Building* building_type, char name[50], struct Model building_model, building_category category, int size_x, int size_y)
{
    sprintf(building_type->name, name);
    building_type->building_model = building_model;
    building_type->category = category;
    building_type->size.x = size_x;
    building_type->size.y = size_y;
}

void Place_Building_OLD(struct Model building_model, building_category category, int x, int y, int size_x, int size_y, direction direction, Building buildings[], int building_limit, struct Tile tiles[map_width][map_length])
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
        buildings[i].category = category;

        buildings[i].pos.x = x;
        buildings[i].size.x = size_x;
        buildings[i].pos.y = y;
        buildings[i].size.y = size_y;
        buildings[i].facing_direction = direction;

        // Tile-ok lefoglalása
        int lx = (int)((float)x - ((float)size_x)/2);
        int ly = (int)((float)y - ((float)size_y)/2);
        for(int ix = 1; ix <= size_x; ix++)
        {
            for(int iy = 1; iy <= size_y; iy++)
            {
                tiles[lx+ix][ly+iy].occupied_by_building = &buildings[i];
            }
        }
    }
}

void Draw_Building(Building building)
{
    glPushMatrix();
        // Épület pozícionálása
        if(fmodf((float)building.size.x, 2.0) == 0)
        {
            glTranslatef(building.pos.x + 0.25 * building.size.x, 0, 0);
        }
        else
        {
            glTranslatef(building.pos.x, 0, 0);
        }
        if(fmodf((float)building.size.y, 2.0) == 0)
        {
            glTranslatef(0, building.pos.y + 0.25 * building.size.y, 0);
        }
        else
        {
            glTranslatef(0, building.pos.y, 0);
        }
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

void Bulldoze_Building_OLD(struct Virtual_Cursor v_cursor, struct Building buildings[], Tile tiles[map_width][map_length])
{
    int x = roundf(v_cursor.pos.x);
    int y = roundf(v_cursor.pos.y);
    Building* bulldozed_building = tiles[x][y].occupied_by_building;

    // Tile-ok felszabadítása
    for(int w = 0; w < map_width; w++)
    {
        for(int l = 0; l < map_length; l++)
        {
            if(tiles[w][l].occupied_by_building == bulldozed_building)
                tiles[w][l].occupied_by_building = NULL;
        }
    }

    // Épület lebontása
    bulldozed_building->exists = 0;
    bulldozed_building->category = none;
}

/*
======================================================================================
    Út kezelõ függvények
*/
void Place_Road_Segment(Road_Segment road_segments[], Node road_nodes[map_width][map_length], Tile tiles[map_width][map_length], int a_x, int a_y, int b_x, int b_y)
{
    // Debug info kiíratás
    printf("\n========== Ut letrehozasa ==========\n");

    //Road_Segment new_road;
    int new_road_index = 0;
    while(new_road_index <= segment_limit && road_segments[new_road_index].exists == true)
        new_road_index++;
    if(new_road_index > segment_limit)
    {
        // RETURN HIBA VAGY VALAMI
    }

    // Megnézni, hogy A node helyén már létezik-e másik
    // Ha nem létezik, akkor létrehozás
    if(road_nodes[a_x][a_y].exists == false)
    {
        road_nodes[a_x][a_y].exists = true;
        road_nodes[a_x][a_y].pos.x = a_x;
        road_nodes[a_x][a_y].pos.y = a_y;

        printf("A node letrehozva az x = %d, y = %d helyen!\n", road_nodes[a_x][a_y].pos.x, road_nodes[a_x][a_y].pos.y);
    }

    // Megnézni, hogy B node helyén már létezik-e másik
    // Ha nem létezik, akkor létrehozás
    if(road_nodes[b_x][b_y].exists == false)
    {
        road_nodes[b_x][b_y].exists = true;
        road_nodes[b_x][b_y].pos.x = b_x;
        road_nodes[b_x][b_y].pos.y = b_y;

        printf("B node letrehozva az x = %d, y = %d helyen!\n", road_nodes[b_x][b_y].pos.x, road_nodes[b_x][b_y].pos.y);
    }

    // Node-ok közötti csatlakozás kialakítása
    if(a_x == b_x)
    {
        if(a_y > b_y)
        {
            road_nodes[a_x][a_y].connection_north.x = b_x;
            road_nodes[a_x][a_y].connection_north.y = b_y;

            road_nodes[b_x][b_y].connection_south.x = a_x;
            road_nodes[b_x][b_y].connection_south.y = a_y;
        }
        else if(b_y > a_y)
        {
            road_nodes[a_x][a_y].connection_south.x = b_x;
            road_nodes[a_x][a_y].connection_south.y = b_y;

            road_nodes[b_x][b_y].connection_north.x = a_x;
            road_nodes[b_x][b_y].connection_north.y = a_y;
        }
    }
    else if(a_y == b_y)
    {
        if(a_x > b_x)
        {
            road_nodes[a_x][a_y].connection_west.x = b_x;
            road_nodes[a_x][a_y].connection_west.y = b_y;

            road_nodes[b_x][b_y].connection_east.x = a_x;
            road_nodes[b_x][b_y].connection_east.y = a_y;
        }
        else if(b_x > a_x)
        {
            road_nodes[a_x][a_y].connection_east.x = b_x;
            road_nodes[a_x][a_y].connection_east.y = b_y;

            road_nodes[b_x][b_y].connection_west.x = a_x;
            road_nodes[b_x][b_y].connection_west.y = a_y;
        }
    }

    // Node-ok típusának frissítése
    Update_Road_Node(&road_nodes[a_x][a_y], tiles);
    Update_Road_Node(&road_nodes[b_x][b_y], tiles);

    // Útszakasz létrehozása, node-ok hozzárendelése
    road_segments[new_road_index].exists = true;
    road_segments[new_road_index].A = &road_nodes[a_x][a_y];
    road_segments[new_road_index].B = &road_nodes[b_x][b_y];
    printf("Ut letrehozva az A(%d; %d), B(%d; %d) helyen!\n", road_segments[new_road_index].A->pos.x, road_segments[new_road_index].A->pos.y, road_segments[new_road_index].B->pos.x, road_segments[new_road_index].B->pos.y);

    // Út  hosszának meghatározása
    if(a_x == b_x)
    {
        if(a_y > b_y)
            road_segments[new_road_index].length = a_y - b_y;
        else
            road_segments[new_road_index].length = b_y - a_y;
    }
    else if(a_y == b_y)
    {
        if(a_x > b_x)
            road_segments[new_road_index].length = a_x - b_x;
        else
            road_segments[new_road_index].length = b_x - a_x;
    }
    printf("Ut hossza kiszamolva: %d0m hosszu.\n", road_segments[new_road_index].length);

    // Ha az új Útszakasz bármelyik Node-ja másik útszakaszban van benne, akkor annak az útszakasznak a kettéosztása
    if(tiles[road_segments[new_road_index].A->pos.x][road_segments[new_road_index].A->pos.y].occupied_by_road_segment != NULL)
    {
        Split_Road_Segment(tiles[road_segments[new_road_index].A->pos.x][road_segments[new_road_index].A->pos.y].occupied_by_road_segment, road_segments, road_nodes, tiles, road_segments[new_road_index].A->pos.x, road_segments[new_road_index].A->pos.y);
    }
    if(tiles[road_segments[new_road_index].B->pos.x][road_segments[new_road_index].B->pos.y].occupied_by_road_segment != NULL)
    {
        Split_Road_Segment(tiles[road_segments[new_road_index].B->pos.x][road_segments[new_road_index].B->pos.y].occupied_by_road_segment, road_segments, road_nodes, tiles, road_segments[new_road_index].B->pos.x, road_segments[new_road_index].B->pos.y);
    }

    // Tile-ok lefoglalása
    if(a_x == b_x)
    {
        if(a_y < b_y)
        {
            for(int i_y = a_y+1; i_y < b_y; i_y++)
            {
                tiles[a_x][i_y].occupied_by_road_segment = &road_segments[new_road_index];
            }
        }
        else
        {
            for(int i_y = b_y+1; i_y < a_y; i_y++)
            {
                tiles[a_x][i_y].occupied_by_road_segment = &road_segments[new_road_index];
            }
        }
    }
    else if(a_y == b_y)
    {
        if(a_x < b_x)
        {
            for(int i_x = a_x+1; i_x < b_x; i_x++)
            {
                tiles[i_x][a_y].occupied_by_road_segment = &road_segments[new_road_index];
            }
        }
        else
        {
            for(int i_x = b_x+1; i_x < a_x; i_x++)
            {
                tiles[i_x][a_y].occupied_by_road_segment = &road_segments[new_road_index];
            }
        }
    }
}
void Update_Road_Node(Node* node, Tile tiles[map_width][map_length])
{
    node->number_of_connections = 0;
    bool connection_to_east = false;
    bool connection_to_north = false;
    bool connection_to_south = false;
    bool connection_to_west = false;

    if(node->connection_east.x != 0 || node->connection_east.y != 0)
    {
        node->number_of_connections++;
        connection_to_east = true;
    }
    if(node->connection_north.x != 0 || node->connection_north.y != 0)
    {
        node->number_of_connections++;
        connection_to_north = true;
    }
    if(node->connection_south.x != 0 || node->connection_south.y != 0)
    {
        node->number_of_connections++;
        connection_to_south = true;
    }
    if(node->connection_west.x != 0 || node->connection_west.y != 0)
    {
        node->number_of_connections++;
        connection_to_west = true;
    }

    switch(node->number_of_connections)
    {
        case 0:
            {
                node->exists = false;
                break;
            }
        case 1:
            {
                if(connection_to_east)
                    node->facing_direction = east;
                else if(connection_to_north)
                    node->facing_direction = north;
                else if(connection_to_south)
                    node->facing_direction = south;
                else if(connection_to_west)
                    node->facing_direction = west;

                node->type = dead_end;
                break;
            }
        case 2:
            {
                if(connection_to_north && connection_to_east)
                    node->facing_direction = north;
                else if(connection_to_north && connection_to_west)
                    node->facing_direction = west;
                else if(connection_to_south && connection_to_east)
                    node->facing_direction = east;
                else if(connection_to_south && connection_to_west)
                    node->facing_direction = south;

                node->type = curve;

                if(connection_to_north && connection_to_south)
                {
                    node->facing_direction = north;
                    node->type = straight;
                }
                else if(connection_to_east && connection_to_west)
                {
                    node->facing_direction = east;
                    node->type = straight;
                }
                break;
            }
        case 3:
            {
                if(!connection_to_east)
                    node->facing_direction = west;
                else if(!connection_to_north)
                    node->facing_direction = south;
                else if(!connection_to_south)
                    node->facing_direction = north;
                else if(!connection_to_west)
                    node->facing_direction = east;

                node->type = intersection_3_way;
                break;
            }
        case 4:
            {
                node->facing_direction = north;
                node->type = intersection_4_way;
                break;
            }
    }

    if(node->exists)
    {
        tiles[node->pos.x][node->pos.y].occupied_by_node = node;
    }
    else
    {
        tiles[node->pos.x][node->pos.y].occupied_by_node = NULL;
    }
}

void Draw_Road_Segment(Road_Segment road, struct Model road_model)
{
    // IDEIGLENES
    // Út adatok kiírása
    /*
    printf("\nUt A node-janak koordinatai: %d, %d", road.A->pos.x, road.A->pos.y);
    printf("\nUt B node-janak koordinatai: %d, %d", road.B->pos.x, road.B->pos.y);
    printf("\nUt hossza: %d", road.length);
    */

    glPushMatrix();

    // Ha az út X tengellyel párhuzamos
    if(road.A->pos.x == road.B->pos.x)
    {
        // Megnézni, hogy A vagy B-e a kisebb és ott kezdeni
        if(road.A->pos.y < road.B->pos.y)
        {
            glTranslatef(road.A->pos.x, road.A->pos.y, 0);
        }
        else
        {
            glTranslatef(road.B->pos.x, road.B->pos.y, 0);
        }

        // Út kirajzolása a szakasz hossza mentén
        glRotatef(90, 0, 0, 1);
        glTranslatef(1, 0, 0);
        for(int i = 0; i < road.length-1; i++)
        {
            Draw_Model(&road_model);
            glTranslatef(1, 0, 0);
        }
    }
    // Ha az út Y tengelylyel párhuzamos
    else if(road.A->pos.y == road.B->pos.y)
    {
        // Megnézni, hogy A vagy B a kisebb és ott kezdeni
        if(road.A->pos.x < road.B->pos.x)
        {
            glTranslatef(road.A->pos.x, road.A->pos.y, 0);
        }
        else
        {
            glTranslatef(road.B->pos.x, road.B->pos.y, 0);
        }

        // Út kirajzolása a szakasz hossza mentén
        glTranslatef(1, 0, 0);
        for(int i = 0; i < road.length-1; i++)
        {
            Draw_Model(&road_model);
            glTranslatef(1, 0, 0);
        }
    }

    glPopMatrix();
}
void Delete_Road_Segment(Road_Segment* deleted_road_segment, Node road_nodes[map_width][map_length], Tile tiles[map_width][map_length])
{
    //Road_Segment* deleted_road_segment = tiles[x][y].occupied_by_road_segment;

    // Tile-ok felszabadítása
    for(int w = 0; w < map_width; w++)
    {
        for(int l = 0; l < map_length; l++)
        {
            if(tiles[w][l].occupied_by_road_segment == deleted_road_segment)
                tiles[w][l].occupied_by_road_segment = NULL;
        }
    }

    // Node-ok közötti kapcsolat törlése
    int a_x = deleted_road_segment->A->pos.x;
    int a_y = deleted_road_segment->A->pos.y;
    int b_x = deleted_road_segment->B->pos.x;
    int b_y = deleted_road_segment->B->pos.y;
    if(a_x == b_x)
    {
        if(a_y > b_y)
        {
            road_nodes[a_x][a_y].connection_north.x = 0;
            road_nodes[a_x][a_y].connection_north.y = 0;

            road_nodes[b_x][b_y].connection_south.x = 0;
            road_nodes[b_x][b_y].connection_south.y = 0;
        }
        else if(b_y > a_y)
        {
            road_nodes[a_x][a_y].connection_south.x = 0;
            road_nodes[a_x][a_y].connection_south.y = 0;

            road_nodes[b_x][b_y].connection_north.x = 0;
            road_nodes[b_x][b_y].connection_north.y = 0;
        }
    }
    else if(a_y == b_y)
    {
        if(a_x > b_x)
        {
            road_nodes[a_x][a_y].connection_west.x = 0;
            road_nodes[a_x][a_y].connection_west.y = 0;

            road_nodes[b_x][b_y].connection_east.x = 0;
            road_nodes[b_x][b_y].connection_east.y = 0;
        }
        else if(b_x > a_x)
        {
            road_nodes[a_x][a_y].connection_east.x = 0;
            road_nodes[a_x][a_y].connection_east.y = 0;

            road_nodes[b_x][b_y].connection_west.x = 0;
            road_nodes[b_x][b_y].connection_west.y = 0;
        }
    }
    deleted_road_segment->A->number_of_connections--;
    deleted_road_segment->B->number_of_connections--;
    Update_Road_Node(deleted_road_segment->A, tiles);
    Update_Road_Node(deleted_road_segment->B, tiles);

    // Út szakasz törlése
    deleted_road_segment->exists = 0;
    deleted_road_segment->length = 0;
    deleted_road_segment->A = NULL;
    deleted_road_segment->B = NULL;
}
void Split_Road_Segment(Road_Segment* road_to_split, Road_Segment road_segments[], Node road_nodes[map_width][map_length], Tile tiles[map_width][map_length], int x, int y)
{
    // Eredeti Node koordináták megjegyzése
    int a_x = road_to_split->A->pos.x;
    int a_y = road_to_split->A->pos.y;
    int b_x = road_to_split->B->pos.x;
    int b_y = road_to_split->B->pos.y;

    // Eredeti szakasz törlése
    Delete_Road_Segment(road_to_split, road_nodes, tiles);

    // Új szakaszok létrehozása
   Place_Road_Segment(road_segments, road_nodes, tiles, a_x, a_y, x, y);
   Place_Road_Segment(road_segments, road_nodes, tiles, x, y, b_x, b_y);
}

void Delete_Road_Node(int x, int y, Road_Segment road_segments[], Node road_nodes[map_width][map_length], Tile tiles[map_width][map_length])
{
    for(int i = 0; i < segment_limit; i++)
    {
        if(road_segments[i].A == &road_nodes[x][y] || road_segments[i].B == &road_nodes[x][y])
        {
            Delete_Road_Segment(&road_segments[i], road_nodes, tiles);
        }
    }
}
/*
======================================================================================
    Tile kezelõ függvények
*/
//bool Check_Tile(int x, int y, struct Tile tiles[map_width][map_length])
int Check_Tile(int x, int y, struct Tile tiles[map_width][map_length])
{
    if(tiles[x][y].occupied_by_building != NULL)
    {
        tiles[x][y].highlighted = true;
        return 1;
    }
    else if(tiles[x][y].occupied_by_node != NULL)
    {
        tiles[x][y].highlighted = true;
        return 2;
    }
    else if(tiles[x][y].occupied_by_road_segment != NULL)
    {
        tiles[x][y].highlighted = true;
        return 3;
    }
    else
    {
        return 0;
    }
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
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, 0);
    glColor4f(1.0f,0.0f,0.0f, 0.5f);

    glPushMatrix();
        // Mátrix mozgatása a Tile pozíciójára
        glTranslatef(tile.pos.x, tile.pos.y, 0.0);

        // Kirajzolás
        glBegin(GL_QUADS);
            glVertex3f(-0.5f, -0.5f, 1.0f);
            glVertex3f(0.5f, -0.5f, 1.0f);
            glVertex3f(0.5f, 0.5f, 1.0f);
            glVertex3f(-0.5f, 0.5f, 1.0f);
        glEnd();

    glPopMatrix();

    // Fény, színek visszaállítása
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glColor3f(1.0f,1.0f,1.0f);
}
