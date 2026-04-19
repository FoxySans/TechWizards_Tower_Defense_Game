#include "camera.h"
#include "map.h"
#include <GL/gl.h>
#include <math.h>


void init_camera(Camera* camera)
{
     camera->position.x = 0.5;  // center of tile col 0
    camera->position.y = 1.5;  // center of tile row 1
    camera->position.z = 0.5;  // eye height
    camera->rotation.x = 0.0;
    camera->rotation.y = 0.0;
    camera->rotation.z = 0.0;
    camera->speed.x = 0.0;
    camera->speed.y = 0.0;
    camera->speed.z = 0.0;
    camera->isSprinting = false;
}
void update_camera(Camera* camera, Map* map, double time)
{
    double angle = degree_to_radian(camera->rotation.z);
    double side_angle = degree_to_radian(camera->rotation.z + 90.0);
    double sprint = camera->isSprinting ? 2.5 : 1.0;

    float new_x = (float)(camera->position.x + cos(angle)      * camera->speed.y * sprint * time
                                              + cos(side_angle) * camera->speed.x * sprint * time);
    float new_y = (float)(camera->position.y + sin(angle)      * camera->speed.y * sprint * time
                                              + sin(side_angle) * camera->speed.x * sprint * time);

    int col_x = (int)(new_x + (new_x > camera->position.x ? PLAYER_RADIUS : -PLAYER_RADIUS));
    int row_x = (int)camera->position.y;
    Tile* tx = map_get_tile(map, col_x, row_x);
    if (!(tx && tx->type == TILE_WALL && camera->position.z < 1.0f))
        camera->position.x = new_x;

    int col_y = (int)camera->position.x;
    int row_y = (int)(new_y + (new_y > camera->position.y ? PLAYER_RADIUS : -PLAYER_RADIUS));
    Tile* ty = map_get_tile(map, col_y, row_y);
    if (!(ty && ty->type == TILE_WALL && camera->position.z < 1.0f))
        camera->position.y = new_y;

    // apply vertical movement
    camera->position.z += camera->speed.z * sprint * time;

    // map border clamp
    if (camera->position.x < 0.0)          camera->position.x = 0.0;
    if (camera->position.x > MAP_WIDTH-1)   camera->position.x = MAP_WIDTH-1;
    if (camera->position.y < 0.0)          camera->position.y = 0.0;
    if (camera->position.y > MAP_HEIGHT-1)  camera->position.y = MAP_HEIGHT-1;

    // vertical clamp based on tile below
    int col = (int)camera->position.x;
    int row = (int)camera->position.y;
    Tile* below = map_get_tile(map, col, row);

    if (below && below->type == TILE_WALL) {
        if (camera->position.z < 1.5f)
            camera->position.z = 1.5f;
    } else {
        if (camera->position.z < 0.5f)
            camera->position.z = 0.5f;
    }

    if (camera->position.z > 10.0f) camera->position.z = 10.0f;
}

void set_view(const Camera* camera)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(-(camera->rotation.x + 90), 1.0, 0, 0);
    glRotatef(-(camera->rotation.z - 90), 0, 0, 1.0);
    glTranslatef(-camera->position.x, -camera->position.y, -camera->position.z);
}

void rotate_camera(Camera* camera, double horizontal, double vertical, double sensy)
{
    camera->rotation.z -= horizontal * sensy;
    camera->rotation.x -= vertical * sensy;

    if (camera->rotation.x > 90.0)  camera->rotation.x = 90.0;
    if (camera->rotation.x < -90.0) camera->rotation.x = -90.0;

    if (camera->rotation.z < 0.0)   camera->rotation.z += 360.0;
    if (camera->rotation.z > 360.0) camera->rotation.z -= 360.0;
}

void set_camera_speed(Camera* camera, double speed)
{
    camera->speed.y = speed;
}

void set_camera_side_speed(Camera* camera, double speed)
{
    camera->speed.x = speed;
}

void set_camera_vertical_speed(Camera* camera, double speed)
{
    camera->speed.z=speed;
}

void set_camera_sprint(Camera* camera, bool sprinting)
{
    camera->isSprinting = sprinting;
}