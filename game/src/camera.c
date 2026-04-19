#include "camera.h"
#include "map.h"
#include <GL/gl.h>
#include <math.h>

void init_camera(Camera* camera)
{
    camera->position.x = 4.0;
    camera->position.y = 4.0;
    camera->position.z = 0.5;
    camera->rotation.x = 0.0;
    camera->rotation.y = 0.0;
    camera->rotation.z = 0.0;
    camera->speed.x = 0.0;
    camera->speed.y = 0.0;
    camera->speed.z = 0.0;
}

void update_camera(Camera* camera, double time)
{
    double angle = degree_to_radian(camera->rotation.z);
    double side_angle = degree_to_radian(camera->rotation.z + 90.0);

    double sprint = camera->isSprinting ? 2.5 : 1.0;
    camera->position.x += cos(angle)      * camera->speed.y * sprint * time;
    camera->position.y += sin(angle)      * camera->speed.y * sprint * time;
    camera->position.x += cos(side_angle) * camera->speed.x * sprint * time;
    camera->position.y += sin(side_angle) * camera->speed.x * sprint * time;

    //stop player movement if outside of map
    if (camera->position.x < 0.0)        camera->position.x = 0.0;
    if (camera->position.x > MAP_WIDTH)   camera->position.x = MAP_WIDTH;
    if (camera->position.y < 0.0)        camera->position.y = 0.0;
    if (camera->position.y > MAP_HEIGHT)  camera->position.y = MAP_HEIGHT;
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
    camera->rotation.z -= horizontal*sensy;
    camera->rotation.x -= vertical*sensy;

    if (camera->rotation.z < 0) {
        camera->rotation.z -= 360.0;
    }

    if (camera->rotation.z > 360.0) {
        camera->rotation.z += 360.0;
    }

    if (camera->rotation.x < 0) {
        camera->rotation.x += 360.0;
    }

    if (camera->rotation.x > 360.0) {
        camera->rotation.x -= 360.0;
    }
}

void set_camera_speed(Camera* camera, double speed)
{
    camera->speed.y = speed;
}

void set_camera_side_speed(Camera* camera, double speed)
{
    camera->speed.x = speed;
}

void set_camera_sprint(Camera* camera, bool sprinting)
{
    camera->isSprinting = sprinting;
}