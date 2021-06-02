#include "building.h"

/*
======================================================================================
    Épület kezelõ függvények
*/
/*
void Make_Building_Type(Building* building_type, char name[50], struct Model building_model, building_category category, int size_x, int size_y)
{
    sprintf(building_type->name, name);
    building_type->building_model = building_model;
    building_type->category = category;
    building_type->size.x = size_x;
    building_type->size.y = size_y;
}
*/
void Make_Building_Type(Building* building_type, char name[50], char model_file_name[50], char texture_name[50], building_category category, int size_x, int size_y)
{
    GLuint building_texture = SOIL_load_OGL_texture
	(
        texture_name,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB
    );
    Load_Model(model_file_name, &building_type->building_model, building_texture);

    sprintf(building_type->name, name);
    building_type->category = category;
    building_type->size.x = size_x;
    building_type->size.y = size_y;

    printf("Epulet tipus letrehozva! \n");
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
        if(direction == north || direction == south)
        {
            int lx = (int)((float)x - ((float)size_x)/2);
            int ly = (int)((float)y - ((float)size_y)/2);

            for(int ix = 1; ix <= size_x; ix++)
            {
                for(int iy = 1; iy <= size_y; iy++)
                {
                    tiles[lx+ix][ly+iy].occupied_by_building = &buildings[i];
                }
            }

            // Épület belépőpontjának elhelyezése
            if(direction == north)
            {
                buildings[i].entry_point.x = roundf(x + size_x/2) + 1;
                buildings[i].entry_point.y = y;
            }
            else if(direction == south)
            {
                buildings[i].entry_point.x = roundf(x - size_x/2) - 1;
                if(size_x % 2 == 0)
                {
                    buildings[i].entry_point.x++;
                }

                buildings[i].entry_point.y = y;
            }
        }
        else if(direction == east || direction == west)
        {
            int lx = (int)((float)x - ((float)size_y)/2);
            int ly = (int)((float)y - ((float)size_x)/2);

            for(int ix = 1; ix <= size_y; ix++)
            {
                for(int iy = 1; iy <= size_x; iy++)
                {
                    tiles[lx+ix][ly+iy].occupied_by_building = &buildings[i];
                }
            }

            // Épület belépőpontjának elhelyezése
            if(direction == east)
            {
                buildings[i].entry_point.x = x;
                buildings[i].entry_point.y = roundf(y - size_x/2) - 1;
                if(size_x % 2 == 0)
                {
                    buildings[i].entry_point.y++;
                }
            }
            else if(direction == west)
            {
                buildings[i].entry_point.x = x;
                buildings[i].entry_point.y = roundf(y + size_x/2) + 1;
            }
        }
    }
}
void Place_Building_By_Name(char building_name[], int x, int y, direction direction, Building building_types[], Building buildings[], int building_limit, struct Tile tiles[map_width][map_length])
{
    for(int i = 0; i < 50; i++)
    {
        if(strcmp(building_types[i].name, building_name)==0)
        {
            Place_Building_OLD(building_types[i].building_model, building_types[i].category, x, y, building_types[i].size.x, building_types[i].size.y, direction, buildings, building_limit, tiles);
            printf("\nEpulel elhelyezve!");
        }
    }
}

void Draw_Building(Building building)
{
    glPushMatrix();
        // Épület pozícionálása
        if(fmodf((float)building.size.x, 2.0) == 0)
        {
            //glTranslatef(building.pos.x + 0.25 * building.size.x, 0, 0);
            glTranslatef(building.pos.x + 0.5, 0, 0);
        }
        else
        {
            glTranslatef(building.pos.x, 0, 0);
        }
        if(fmodf((float)building.size.y, 2.0) == 0)
        {
            //glTranslatef(0, building.pos.y + 0.25 * building.size.y, 0);
            glTranslatef(0, building.pos.y + 0.5, 0);
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

building_category Building_Type_Enum(char *sval)
{
    building_category result = nothing;
    int i=0;
    for (i=0; building_category_table[i]!=NULL; ++i, ++result)
        if (0==strcmp(sval, building_category_table[i])) return result;
    return -1;
}
