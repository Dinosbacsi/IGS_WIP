#include "vehicle.h"

/*
======================================================================================
    Jármű kezelõ függvények
*/
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
                if(i == 0 || i == 1)
                {
                    glRotatef(vehicle.wheel_turn, 0, 0, 1);
                }
                if(i == 1 || i == 3)
                {
                    glRotatef(180, 0, 0, 1);
                    glRotatef(vehicle.wheel_rotate, 0, -1, 0);
                }
                else
                {
                    glRotatef(vehicle.wheel_rotate, 0, 1, 0);
                }
                Draw_Model(&vehicle.wheel_model);
            glPopMatrix();
        }
    glPopMatrix();
}
void Place_Vehicle(Vehicle vehicles[], Vehicle* vehicle_type, int tile_x, int tile_y, Road_Segment road_segments[], Tile tiles[map_width][map_length], Node road_nodes[map_width][map_length])
{
    int new_vehicle_index = 0;
    while(vehicles[new_vehicle_index].exists == true && new_vehicle_index <= vehicle_limit)
    {
        new_vehicle_index++;
    }
    if(new_vehicle_index < vehicle_limit && Check_Tile(tile_x, tile_y, tiles) == 3)
    {
        // Jármű létrehozása
        Vehicle* new_vehicle = &vehicles[new_vehicle_index];

        new_vehicle->exists = true;
        new_vehicle->vehicle_model = vehicle_type->vehicle_model;
        new_vehicle->wheel_model = vehicle_type->wheel_model;
        new_vehicle->wheel[0] = vehicle_type->wheel[0];
        new_vehicle->wheel[1] = vehicle_type->wheel[1];
        new_vehicle->wheel[2] = vehicle_type->wheel[2];
        new_vehicle->wheel[3] = vehicle_type->wheel[3];
        new_vehicle->wheel_rotate = 0;
        new_vehicle->max_speed = vehicle_type->max_speed;
        new_vehicle->acceleration_rate = vehicle_type->acceleration_rate;

        // Jármű elhelyezése
        new_vehicle->next_node = tiles[tile_x][tile_y].occupied_by_road_segment->A;
        new_vehicle->previous_node = tiles[tile_x][tile_y].occupied_by_road_segment->B;
        new_vehicle->current_tile = &tiles[tile_x][tile_y];
        if(new_vehicle->next_node->pos.x == tile_x)
        {
            new_vehicle->pos.y = tile_y;
            if(new_vehicle->next_node->pos.y > tile_y)
            {
                new_vehicle->pos.x = tile_x + 0.2;
                new_vehicle->rotate.z = 90;
                new_vehicle->facing = south;
                new_vehicle->previous_facing = south;
            }
            else
            {
                new_vehicle->pos.x = tile_x - 0.2;
                new_vehicle->rotate.z = 270;
                new_vehicle->facing = north;
                new_vehicle->previous_facing = north;
            }
        }
        else
        {
            new_vehicle->pos.x = tile_x;
            if(new_vehicle->next_node->pos.x > tile_x)
            {
                new_vehicle->pos.y = tile_y - 0.2;
                new_vehicle->rotate.z = 0;
                new_vehicle->facing = east;
                new_vehicle->previous_facing = east;
            }
            else
            {
                new_vehicle->pos.y = tile_y + 0.2;
                new_vehicle->rotate.z = 180;
                new_vehicle->facing = west;
                new_vehicle->previous_facing = west;
            }
        }
        printf("\nJarmu elhelyezve!");

        new_vehicle->destination_node = &road_nodes[104][299];
        Find_Path(new_vehicle, road_nodes);
    }
}
void Vehicle_Cruise(Vehicle* vehicle, Node road_nodes[map_width][map_length], Tile tiles[map_width][map_length])
{
    if(vehicle->current_tile->pos.x == vehicle->next_node->pos.x)
    {
        if(fabsf(vehicle->pos.y - vehicle->next_node->pos.y) >= 0.5)
        {
            if(vehicle->speed < vehicle->max_speed)
                vehicle->speed += vehicle->acceleration_rate;

            if(vehicle->pos.y > vehicle->next_node->pos.y)
            {
                vehicle->pos.y -= vehicle->speed;
                vehicle->current_tile = &tiles[(int)roundf(vehicle->pos.x)][(int)roundf(vehicle->pos.y)];
            }
            else
            {
                vehicle->pos.y += vehicle->speed;
                vehicle->current_tile = &tiles[(int)roundf(vehicle->pos.x)][(int)roundf(vehicle->pos.y)];
            }
        }
        else
        {
            if(vehicle->turning == false)
            {
                //Vehicle_Cruise_Choose_Direction(vehicle, road_nodes);
                Vehicle_Follow_Path(vehicle);
            }
            else
            {
                switch(vehicle->facing)
                {
                case north:
                    Vehicle_Go_North(vehicle, road_nodes);
                    break;
                case east:
                    Vehicle_Go_East(vehicle, road_nodes);
                    break;
                case south:
                    Vehicle_Go_South(vehicle, road_nodes);
                    break;
                case west:
                    Vehicle_Go_West(vehicle, road_nodes);
                    break;
                }
            }
        }
    }
    else if(vehicle->current_tile->pos.y == vehicle->next_node->pos.y)
    {
        if(fabsf(vehicle->pos.x - vehicle->next_node->pos.x) >= 0.5)
        {
            if(vehicle->speed < vehicle->max_speed)
                vehicle->speed += vehicle->acceleration_rate;

            if(vehicle->pos.x > vehicle->next_node->pos.x)
            {
                vehicle->pos.x -= vehicle->speed;
            }
            else
            {
                vehicle->pos.x += vehicle->speed;
            }
        }
        else
        {
            if(vehicle->turning == false)
                Vehicle_Cruise_Choose_Direction(vehicle, road_nodes);
            else
            {
                switch(vehicle->facing)
                {
                case north:
                    Vehicle_Go_North(vehicle, road_nodes);
                    break;
                case east:
                    Vehicle_Go_East(vehicle, road_nodes);
                    break;
                case south:
                    Vehicle_Go_South(vehicle, road_nodes);
                    break;
                case west:
                    Vehicle_Go_West(vehicle, road_nodes);
                    break;
                }
            }
        }
    }

    vehicle->wheel_rotate += vehicle->speed * 1000;
    if(vehicle->wheel_rotate >360)
    {
        vehicle->wheel_rotate -= 360;
    }
    if(vehicle->turning == false)
    {
        Vehicle_Steer_Straight(vehicle);
    }
    vehicle->current_tile = &tiles[(int)roundf(vehicle->pos.x)][(int)roundf(vehicle->pos.y)];
}

void Vehicle_Cruise_Choose_Direction(Vehicle* vehicle, Node road_nodes[map_width][map_length])
{
    if(vehicle->next_node->number_of_connections == 1)
    {
        if(vehicle->next_node->connection_north.x != 0 && vehicle->next_node->connection_north.y)
        {
            vehicle->previous_facing = vehicle->facing;
            vehicle->facing = north;
            vehicle->turning = true;
        }
        else if(vehicle->next_node->connection_east.x != 0 && vehicle->next_node->connection_east.y)
        {
            vehicle->previous_facing = vehicle->facing;
            vehicle->facing = east;
            vehicle->turning = true;
        }
        else if(vehicle->next_node->connection_south.x != 0 && vehicle->next_node->connection_south.y)
        {
            vehicle->previous_facing = vehicle->facing;
            vehicle->facing = south;
            vehicle->turning = true;
        }
        else if(vehicle->next_node->connection_west.x != 0 && vehicle->next_node->connection_west.y)
        {
            vehicle->previous_facing = vehicle->facing;
            vehicle->facing = west;
            vehicle->turning = true;
        }
    }
    else
    {
        bool direction_found = false;
        while(direction_found == false)
        {
            switch(rand() % (4 + 1 - 1) + 1)
            {
            case 1:
                if (vehicle->previous_node != &road_nodes[vehicle->next_node->connection_north.x][vehicle->next_node->connection_north.y] && road_nodes[vehicle->next_node->connection_north.x][vehicle->next_node->connection_north.y].exists == true)
                {
                    vehicle->previous_facing = vehicle->facing;
                    vehicle->facing = north;
                    vehicle->turning = true;

                    direction_found = true;
                }
                break;
            case 2:
                if (vehicle->previous_node != &road_nodes[vehicle->next_node->connection_east.x][vehicle->next_node->connection_east.y] && road_nodes[vehicle->next_node->connection_east.x][vehicle->next_node->connection_east.y].exists == true)
                {
                    vehicle->previous_facing = vehicle->facing;
                    vehicle->facing = east;
                    vehicle->turning = true;

                    direction_found = true;
                }
                break;
            case 3:
                if (vehicle->previous_node != &road_nodes[vehicle->next_node->connection_south.x][vehicle->next_node->connection_south.y] && road_nodes[vehicle->next_node->connection_south.x][vehicle->next_node->connection_south.y].exists == true)
                {
                    vehicle->previous_facing = vehicle->facing;
                    vehicle->facing = south;
                    vehicle->turning = true;

                    direction_found = true;
                }
                break;
            case 4:
                if (vehicle->previous_node != &road_nodes[vehicle->next_node->connection_west.x][vehicle->next_node->connection_west.y] && road_nodes[vehicle->next_node->connection_west.x][vehicle->next_node->connection_west.y].exists == true)
                {
                    vehicle->previous_facing = vehicle->facing;
                    vehicle->facing = west;
                    vehicle->turning = true;

                    direction_found = true;
                }
                break;
            }
        }
    }
}
void Vehicle_Follow_Path(Vehicle* vehicle)
{
    if(vehicle->next_node->connection_east.x == vehicle->path_nodes[vehicle->current_node_in_path+1]->pos.x && vehicle->next_node->connection_east.y == vehicle->path_nodes[vehicle->current_node_in_path+1]->pos.y)
    {
        vehicle->previous_facing = vehicle->facing;
        vehicle->facing = east;
        vehicle->turning = true;
    }
    else if(vehicle->next_node->connection_north.x == vehicle->path_nodes[vehicle->current_node_in_path+1]->pos.x && vehicle->next_node->connection_north.y == vehicle->path_nodes[vehicle->current_node_in_path+1]->pos.y)
    {
        vehicle->previous_facing = vehicle->facing;
        vehicle->facing = north;
        vehicle->turning = true;
    }
    else if(vehicle->next_node->connection_south.x == vehicle->path_nodes[vehicle->current_node_in_path+1]->pos.x && vehicle->next_node->connection_south.y == vehicle->path_nodes[vehicle->current_node_in_path+1]->pos.y)
    {
        vehicle->previous_facing = vehicle->facing;
        vehicle->facing = south;
        vehicle->turning = true;
    }
    else if(vehicle->next_node->connection_west.x == vehicle->path_nodes[vehicle->current_node_in_path+1]->pos.x && vehicle->next_node->connection_west.y == vehicle->path_nodes[vehicle->current_node_in_path+1]->pos.y)
    {
        vehicle->previous_facing = vehicle->facing;
        vehicle->facing = west;
        vehicle->turning = true;
    }

    vehicle->current_node_in_path++;
}

void Vehicle_Go_North(Vehicle* vehicle, Node road_nodes[map_width][map_length])
{
    switch(vehicle->previous_facing)
    {
    case north:
        vehicle->previous_node = vehicle->next_node;
        vehicle->next_node = &road_nodes[vehicle->next_node->connection_north.x][vehicle->next_node->connection_north.y];
        vehicle->turning = false;
        break;
    case south:
        if(vehicle->rotate.z < 270)
        {
            vehicle->rotate.z += vehicle->speed * 100;
            Animate_Vehicle_Turning(vehicle);
        }
        else
        {
            vehicle->pos.x = vehicle->next_node->pos.x - 0.2;
            vehicle->pos.y = vehicle->next_node->pos.y - 0.5;
            vehicle->rotate.z = 270;

            vehicle->previous_node = vehicle->next_node;
            vehicle->next_node = &road_nodes[vehicle->next_node->connection_north.x][vehicle->next_node->connection_north.y];
            vehicle->turning = false;
        }
        break;
    case east:
        if(vehicle->rotate.z > -90)
        {
            vehicle->rotate.z -= vehicle->speed * 100;
            Animate_Vehicle_Turning(vehicle);
        }
        else
        {
            vehicle->pos.x = vehicle->next_node->pos.x - 0.2;
            vehicle->pos.y = vehicle->next_node->pos.y - 0.5;
            vehicle->rotate.z = 270;

            vehicle->previous_node = vehicle->next_node;
            vehicle->next_node = &road_nodes[vehicle->next_node->connection_north.x][vehicle->next_node->connection_north.y];
            vehicle->turning = false;
        }
        break;
    case west:
        if(vehicle->rotate.z < 270)
        {
            vehicle->rotate.z += vehicle->speed * 100;
            Animate_Vehicle_Turning(vehicle);
        }
        else
        {
            vehicle->pos.x = vehicle->next_node->pos.x - 0.2;
            vehicle->pos.y = vehicle->next_node->pos.y - 0.5;
            vehicle->rotate.z = 270;

            vehicle->previous_node = vehicle->next_node;
            vehicle->next_node = &road_nodes[vehicle->next_node->connection_north.x][vehicle->next_node->connection_north.y];
            vehicle->turning = false;
        }
        break;
    }
}
void Vehicle_Go_East(Vehicle* vehicle, Node road_nodes[map_width][map_length])
{
    switch(vehicle->previous_facing)
    {
    case east:
        vehicle->previous_node = vehicle->next_node;
        vehicle->next_node = &road_nodes[vehicle->next_node->connection_east.x][vehicle->next_node->connection_east.y];
        vehicle->turning = false;
        break;
    case west:
        if(vehicle->rotate.z < 360)
        {
            vehicle->rotate.z += vehicle->speed * 100;
            Animate_Vehicle_Turning(vehicle);
        }
        else
        {
            vehicle->pos.x = vehicle->next_node->pos.x + 0.5;
            vehicle->pos.y = vehicle->next_node->pos.y - 0.2;
            vehicle->rotate.z = 0;

            vehicle->previous_node = vehicle->next_node;
            vehicle->next_node = &road_nodes[vehicle->next_node->connection_east.x][vehicle->next_node->connection_east.y];
            vehicle->turning = false;
        }
        break;
    case north:
        if(vehicle->rotate.z < 360)
        {
            vehicle->rotate.z += vehicle->speed * 100;
            Animate_Vehicle_Turning(vehicle);
        }
        else
        {
            vehicle->pos.x = vehicle->next_node->pos.x + 0.5;
            vehicle->pos.y = vehicle->next_node->pos.y - 0.2;
            vehicle->rotate.z = 0;

            vehicle->previous_node = vehicle->next_node;
            vehicle->next_node = &road_nodes[vehicle->next_node->connection_east.x][vehicle->next_node->connection_east.y];
            vehicle->turning = false;
        }
        break;
    case south:
        if(vehicle->rotate.z > 0)
        {
            vehicle->rotate.z -= vehicle->speed * 100;
            Animate_Vehicle_Turning(vehicle);
        }
        else
        {
            vehicle->pos.x = vehicle->next_node->pos.x + 0.5;
            vehicle->pos.y = vehicle->next_node->pos.y - 0.2;
            vehicle->rotate.z = 0;

            vehicle->previous_node = vehicle->next_node;
            vehicle->next_node = &road_nodes[vehicle->next_node->connection_east.x][vehicle->next_node->connection_east.y];
            vehicle->turning = false;
        }
        break;
    }
}
void Vehicle_Go_South(Vehicle* vehicle, Node road_nodes[map_width][map_length])
{
    switch(vehicle->previous_facing)
    {
    case south:
        vehicle->previous_node = vehicle->next_node;
        vehicle->next_node = &road_nodes[vehicle->next_node->connection_south.x][vehicle->next_node->connection_south.y];
        vehicle->turning = false;
        break;
    case north:
        if(vehicle->rotate.z < 450)
        {
            vehicle->rotate.z += vehicle->speed * 100;
            Animate_Vehicle_Turning(vehicle);
        }
        else
        {
            vehicle->pos.x = vehicle->next_node->pos.x + 0.2;
            vehicle->pos.y = vehicle->next_node->pos.y + 0.5;
            vehicle->rotate.z = 90;

            vehicle->previous_node = vehicle->next_node;
            vehicle->next_node = &road_nodes[vehicle->next_node->connection_south.x][vehicle->next_node->connection_south.y];
            vehicle->turning = false;
        }
        break;
    case east:
        if(vehicle->rotate.z < 90)
        {
            vehicle->rotate.z += vehicle->speed * 100;
            Animate_Vehicle_Turning(vehicle);
        }
        else
        {
            vehicle->pos.x = vehicle->next_node->pos.x + 0.2;
            vehicle->pos.y = vehicle->next_node->pos.y + 0.5;
            vehicle->rotate.z = 90;

            vehicle->previous_node = vehicle->next_node;
            vehicle->next_node = &road_nodes[vehicle->next_node->connection_south.x][vehicle->next_node->connection_south.y];
            vehicle->turning = false;
        }
        break;
    case west:
        if(vehicle->rotate.z > 90)
        {
            vehicle->rotate.z -= vehicle->speed * 100;
            Animate_Vehicle_Turning(vehicle);
        }
        else
        {
            vehicle->pos.x = vehicle->next_node->pos.x + 0.2;
            vehicle->pos.y = vehicle->next_node->pos.y + 0.5;
            vehicle->rotate.z = 90;

            vehicle->previous_node = vehicle->next_node;
            vehicle->next_node = &road_nodes[vehicle->next_node->connection_south.x][vehicle->next_node->connection_south.y];
            vehicle->turning = false;
        }
        break;
    }
}
void Vehicle_Go_West(Vehicle* vehicle, Node road_nodes[map_width][map_length])
{
    switch(vehicle->previous_facing)
    {
    case west:
        vehicle->previous_node = vehicle->next_node;
        vehicle->next_node = &road_nodes[vehicle->next_node->connection_west.x][vehicle->next_node->connection_west.y];
        vehicle->turning = false;
        break;
    case east:
        if(vehicle->rotate.z < 180)
        {
            vehicle->rotate.z += vehicle->speed * 100;
            Animate_Vehicle_Turning(vehicle);
        }
        else
        {
            vehicle->pos.x = vehicle->next_node->pos.x - 0.5;
            vehicle->pos.y = vehicle->next_node->pos.y + 0.2;
            vehicle->rotate.z = 180;

            vehicle->previous_node = vehicle->next_node;
            vehicle->next_node = &road_nodes[vehicle->next_node->connection_west.x][vehicle->next_node->connection_west.y];
            vehicle->turning = false;
        }
        break;
    case south:
        if(vehicle->rotate.z < 180)
        {
            vehicle->rotate.z += vehicle->speed * 100;
            Animate_Vehicle_Turning(vehicle);
        }
        else
        {
            vehicle->pos.x = vehicle->next_node->pos.x - 0.5;
            vehicle->pos.y = vehicle->next_node->pos.y + 0.2;
            vehicle->rotate.z = 180;

            vehicle->previous_node = vehicle->next_node;
            vehicle->next_node = &road_nodes[vehicle->next_node->connection_west.x][vehicle->next_node->connection_west.y];
            vehicle->turning = false;
        }
        break;
    case north:
        if(vehicle->rotate.z > 180)
        {
            vehicle->rotate.z -= vehicle->speed * 100;
            Animate_Vehicle_Turning(vehicle);
        }
        else
        {
            vehicle->pos.x = vehicle->next_node->pos.x - 0.5;
            vehicle->pos.y = vehicle->next_node->pos.y + 0.2;
            vehicle->rotate.z = 180;

            vehicle->previous_node = vehicle->next_node;
            vehicle->next_node = &road_nodes[vehicle->next_node->connection_west.x][vehicle->next_node->connection_west.y];
            vehicle->turning = false;
        }
        break;
    }
}
void Animate_Vehicle_Turning(Vehicle* vehicle)
{
    vec2 pivot_point;
    switch(vehicle->previous_facing)
    {
    case north:
        pivot_point.y = vehicle->next_node->pos.y + 0.49;

        if(vehicle->facing == south)
        {
            pivot_point.x = vehicle->next_node->pos.x;

            Vehicle_Turn_Left(vehicle);
            vehicle->pos.x = pivot_point.x + (0.2 * sin(vehicle->rotate.z * M_PI/180));
            vehicle->pos.y = pivot_point.y - (0.2 * cos(vehicle->rotate.z * M_PI/180));
        }
        else if(vehicle->facing == east)
        {
            pivot_point.x = vehicle->next_node->pos.x + 0.49;

            Vehicle_Turn_Left(vehicle);
            vehicle->pos.x = pivot_point.x + (0.7 * sin(vehicle->rotate.z * M_PI/180));
            vehicle->pos.y = pivot_point.y - (0.7 * cos(vehicle->rotate.z * M_PI/180));
        }
        else if(vehicle->facing == west)
        {
            pivot_point.x = vehicle->next_node->pos.x - 0.49;
            //
            Vehicle_Turn_Right(vehicle);
            vehicle->pos.x = pivot_point.x - (0.3 * sin(vehicle->rotate.z * M_PI/180));
            vehicle->pos.y = pivot_point.y + (0.3 * cos(vehicle->rotate.z * M_PI/180));
        }
        break;
    case east:
        pivot_point.x = vehicle->next_node->pos.x - 0.49;

        if(vehicle->facing == west)
        {
            pivot_point.y = vehicle->next_node->pos.y;

            Vehicle_Turn_Left(vehicle);
            vehicle->pos.x = pivot_point.x + (0.2 * sin(vehicle->rotate.z * M_PI/180));
            vehicle->pos.y = pivot_point.y - (0.2 * cos(vehicle->rotate.z * M_PI/180));
        }
        else if(vehicle->facing == north)
        {
            pivot_point.y = vehicle->next_node->pos.y - 0.49;
            //
            Vehicle_Turn_Right(vehicle);
            vehicle->pos.x = pivot_point.x - (0.3 * sin(vehicle->rotate.z * M_PI/180));
            vehicle->pos.y = pivot_point.y + (0.3 * cos(vehicle->rotate.z * M_PI/180));
        }
        else if(vehicle->facing == south)
        {
            pivot_point.y = vehicle->next_node->pos.y + 0.49;

            Vehicle_Turn_Left(vehicle);
            vehicle->pos.x = pivot_point.x + (0.7 * sin(vehicle->rotate.z * M_PI/180));
            vehicle->pos.y = pivot_point.y - (0.7 * cos(vehicle->rotate.z * M_PI/180));
        }
        break;
    case south:
        pivot_point.y = vehicle->next_node->pos.y - 0.49;

        if(vehicle->facing == north)
        {
            pivot_point.x = vehicle->next_node->pos.x;

            Vehicle_Turn_Left(vehicle);
            vehicle->pos.x = pivot_point.x + (0.2 * sin(vehicle->rotate.z * M_PI/180));
            vehicle->pos.y = pivot_point.y - (0.2 * cos(vehicle->rotate.z * M_PI/180));
        }
        else if(vehicle->facing == east)
        {
            pivot_point.x = vehicle->next_node->pos.x + 0.49;
            //
            Vehicle_Turn_Right(vehicle);
            vehicle->pos.x = pivot_point.x - (0.3 * sin(vehicle->rotate.z * M_PI/180));
            vehicle->pos.y = pivot_point.y + (0.3 * cos(vehicle->rotate.z * M_PI/180));
        }
        else if(vehicle->facing == west)
        {
            pivot_point.x = vehicle->next_node->pos.x - 0.49;

            Vehicle_Turn_Left(vehicle);
            vehicle->pos.x = pivot_point.x + (0.7 * sin(vehicle->rotate.z * M_PI/180));
            vehicle->pos.y = pivot_point.y - (0.7 * cos(vehicle->rotate.z * M_PI/180));
        }
        break;
    case west:
        pivot_point.x = vehicle->next_node->pos.x + 0.49;

        if(vehicle->facing == east)
        {
            pivot_point.y = vehicle->next_node->pos.y;

            Vehicle_Turn_Left(vehicle);
            vehicle->pos.x = pivot_point.x + (0.2 * sin(vehicle->rotate.z * M_PI/180));
            vehicle->pos.y = pivot_point.y - (0.2 * cos(vehicle->rotate.z * M_PI/180));
        }
        else if(vehicle->facing == north)
        {
            pivot_point.y = vehicle->next_node->pos.y - 0.49;

            Vehicle_Turn_Left(vehicle);
            vehicle->pos.x = pivot_point.x + (0.7 * sin(vehicle->rotate.z * M_PI/180));
            vehicle->pos.y = pivot_point.y - (0.7 * cos(vehicle->rotate.z * M_PI/180));
        }
        else if(vehicle->facing == south)
        {
            pivot_point.y = vehicle->next_node->pos.y + 0.49;
            //
            Vehicle_Turn_Right(vehicle);
            vehicle->pos.x = pivot_point.x - (0.3 * sin(vehicle->rotate.z * M_PI/180));
            vehicle->pos.y = pivot_point.y + (0.3 * cos(vehicle->rotate.z * M_PI/180));
        }
        break;
    }

    //vehicle->pos.x = pivot_point.x;
    //vehicle->pos.y = pivot_point.y;
}
void Vehicle_Turn_Left(Vehicle* vehicle)
{
    vehicle->wheel_turn = 30;
}
void Vehicle_Turn_Right(Vehicle* vehicle)
{
    vehicle->wheel_turn = -30;
}
void Vehicle_Steer_Straight(Vehicle* vehicle)
{
    vehicle->wheel_turn = 0;
}
void Move_Vehicle(Vehicle* vehicle)
{
    vehicle->pos.x += cos(vehicle->rotate.z * M_PI/180) * vehicle->speed;
    vehicle->pos.y += sin(vehicle->rotate.z * M_PI/180) * vehicle->speed;
}
void Vehicle_Accelerate(Vehicle* vehicle)
{
    if(vehicle->speed < vehicle->max_speed)
        vehicle->speed += vehicle->acceleration_rate;
}
void Vehicle_Decelerate(Vehicle* vehicle)
{
    if(vehicle->speed > 0)
        vehicle->speed -= vehicle->acceleration_rate;
}

void Find_Path(Vehicle* vehicle, Node road_nodes[map_width][map_length])
{
    for(int i = 0; i < map_width; i++)
    {
        for(int j = 0; j < map_length; j++)
        {
            road_nodes[i][j].checked = false;
        }
    }

    vehicle->path_nodes[0] = vehicle->next_node;

    int i = 0;
    int depth = 1;
    bool path_found = false;
    while(path_found == false && depth <= 100)
    {
        printf("\n");

        if(road_nodes[vehicle->path_nodes[i]->connection_east.x][vehicle->path_nodes[i]->connection_east.y].exists &&
               road_nodes[vehicle->path_nodes[i]->connection_east.x][vehicle->path_nodes[i]->connection_east.y].checked == false)
        {
            road_nodes[vehicle->path_nodes[i]->connection_east.x][vehicle->path_nodes[i]->connection_east.y].checked = true;
            printf("Kelet vizsgalat. ");

            if(i+1 <depth)
            {
                vehicle->path_nodes[i+1] = &road_nodes[vehicle->path_nodes[i]->connection_east.x][vehicle->path_nodes[i]->connection_east.y];
                i++;
                printf("Kelet hozzaadva. ");
            }
            else if(i+1 == depth && &road_nodes[vehicle->path_nodes[i]->connection_east.x][vehicle->path_nodes[i]->connection_east.y] == vehicle->destination_node)
            {
                vehicle->path_nodes[i+1] = &road_nodes[vehicle->path_nodes[i]->connection_east.x][vehicle->path_nodes[i]->connection_east.y];
                path_found = true;
                printf("Utvonal talalva! ");
            }
        }
        else if(road_nodes[vehicle->path_nodes[i]->connection_north.x][vehicle->path_nodes[i]->connection_north.y].exists &&
               road_nodes[vehicle->path_nodes[i]->connection_north.x][vehicle->path_nodes[i]->connection_north.y].checked == false)
        {
            road_nodes[vehicle->path_nodes[i]->connection_north.x][vehicle->path_nodes[i]->connection_north.y].checked = true;
            printf("Eszak vizsgalat. ");
            if(i+1 <depth)
            {
                vehicle->path_nodes[i+1] = &road_nodes[vehicle->path_nodes[i]->connection_north.x][vehicle->path_nodes[i]->connection_north.y];
                i++;
                printf("Eszak hozzaadva. ");
            }
            else if(i+1 == depth && &road_nodes[vehicle->path_nodes[i]->connection_north.x][vehicle->path_nodes[i]->connection_north.y] == vehicle->destination_node)
            {
                vehicle->path_nodes[i+1] = &road_nodes[vehicle->path_nodes[i]->connection_north.x][vehicle->path_nodes[i]->connection_north.y];
                path_found = true;
                printf("Utvonal talalva! ");
            }
        }
        else if(road_nodes[vehicle->path_nodes[i]->connection_south.x][vehicle->path_nodes[i]->connection_south.y].exists &&
               road_nodes[vehicle->path_nodes[i]->connection_south.x][vehicle->path_nodes[i]->connection_south.y].checked == false)
        {
            road_nodes[vehicle->path_nodes[i]->connection_south.x][vehicle->path_nodes[i]->connection_south.y].checked = true;
            printf("Del vizsgalat. ");
            if(i+1 <depth)
            {
                vehicle->path_nodes[i+1] = &road_nodes[vehicle->path_nodes[i]->connection_south.x][vehicle->path_nodes[i]->connection_south.y];
                i++;
                printf("Del hozzaadva. ");
            }
            else if(i+1 == depth && &road_nodes[vehicle->path_nodes[i]->connection_south.x][vehicle->path_nodes[i]->connection_south.y] == vehicle->destination_node)
            {
                vehicle->path_nodes[i+1] = &road_nodes[vehicle->path_nodes[i]->connection_south.x][vehicle->path_nodes[i]->connection_south.y];
                path_found = true;
                printf("Utvonal talalva! ");
            }
        }
        else if(road_nodes[vehicle->path_nodes[i]->connection_west.x][vehicle->path_nodes[i]->connection_west.y].exists &&
               road_nodes[vehicle->path_nodes[i]->connection_west.x][vehicle->path_nodes[i]->connection_west.y].checked == false)
        {
            road_nodes[vehicle->path_nodes[i]->connection_west.x][vehicle->path_nodes[i]->connection_west.y].checked = true;
            printf("Nyugat vizsgalva. ");
            if(i+1 <depth)
            {
                vehicle->path_nodes[i+1] = &road_nodes[vehicle->path_nodes[i]->connection_west.x][vehicle->path_nodes[i]->connection_west.y];
                i++;
                printf("Nyugat hozzaadva. ");
            }
            else if(i+1 == depth && &road_nodes[vehicle->path_nodes[i]->connection_west.x][vehicle->path_nodes[i]->connection_west.y] == vehicle->destination_node)
            {
                vehicle->path_nodes[i+1] = &road_nodes[vehicle->path_nodes[i]->connection_west.x][vehicle->path_nodes[i]->connection_west.y];
                path_found = true;
                printf("Utvonal talalva! ");
            }
        }
        else
        {
            printf("Egyik node sam szabad. ");
            if(i==0)
            {
                for(int i = 0; i < map_width; i++)
                {
                    for(int j = 0; j < map_length; j++)
                    {
                        road_nodes[i][j].checked = false;
                    }
                }

                depth++;
                printf("Depth noveles. ");
            }
            else
            {
                printf("\nVisszalepes. ");
                i--;
            }
        }
    }

    if(depth >= 100)
    {
        printf("\nUtvonal keresese nem sikerult :C");
    }
    if(path_found == true)
    {
        printf("\nUtvonal talalva!");
        vehicle->current_node_in_path = 0;
    }
}
