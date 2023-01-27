#include "../dos/dos.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define SCALE_FACTOR 100.0

// Buffer array to hold height map values (1024x1024)
uint8_t *heightmap = NULL;
// Buffer array to hold color map values (1024x1024)
uint8_t *colormap = NULL;

typedef struct {
  float x;       // x position on the map
  float y;       // y position on the map
  float height;  // height of the camera
  float angle;   // camera angle (radians, increases clockwise)
  float horizon; // offset of the horizon position (looking up and down)
  float zfar;    // distance of the camera looking forward
} camera_t;

// Angle: 270 degrees looking up is 1.5 * 3.1415926
camera_t camera = {.x = 512,
                   .y = 512,
                   .height = 150.0,
                   .angle = 1.5 * 3.1415926, // Same as 270 degrees
                   .horizon = 100.0,
                   .zfar = 600};

// processinput processes keyboard input
void processinput() {
  if (keystate(KEY_UP)) {
    camera.x += cos(camera.angle);
    camera.y += sin(camera.angle);
  }
  if (keystate(KEY_DOWN)) {
    camera.x -= cos(camera.angle);
    camera.y -= sin(camera.angle);
  }
  if (keystate(KEY_LEFT)) {
    // Decrease by radians
    camera.angle -= 0.01;
  }
  if (keystate(KEY_RIGHT)) {
    // Increase by radians
    camera.angle += 0.01;
  }
  if (keystate(KEY_E)) {
    camera.height++;
  }
  if (keystate(KEY_D)) {
    camera.height--;
  }
  if (keystate(KEY_Q)) {
    camera.horizon += 1.5;
  }
  if (keystate(KEY_W)) {
    camera.horizon -= 1.5;
  }
}

int oldmain(int argc, char *args[]) {
  setvideomode(videomode_320x200);

  // 256 * 3 colors for RGB components
  uint8_t palette[256 * 3];
  int map_width, map_height, pal_count;

  colormap = loadgif("maps/gif/map0.color.gif", &map_width, &map_height,
                     &pal_count, palette);
  heightmap = loadgif("maps/gif/map0.height.gif", NULL, NULL, NULL, NULL);

  // Set color palette for the DOS-Like framework
  for (int i = 0; i < pal_count; i++) {
    setpal(i, palette[3 * i + 0], palette[3 * i + 1], palette[3 * i + 2]);
  }

  // Set the first color to a specific color for the background
  // since the first color of the palette is the background color in DOS-Like
  setpal(0, 36, 36, 56);

  setdoublebuffer(1);

  // A long array representing SCREEN_WIDTH * SCREEN_HEIGHT bytes
  uint8_t *framebuffer = screenbuffer();

  while (!shuttingdown()) {
    waitvbl();
    clearscreen();

    processinput();

    // Get the x, y coordinates of the left-most and right-most points on FOV
    float sinangle = sin(camera.angle);
    float cosangle = cos(camera.angle);
    // float pl_x = -camera.zfar;
    // float pl_y = +camera.zfar;
    // float pr_x = +camera.zfar;
    // float pr_y = +camera.zfar;
    float pl_x = cosangle * camera.zfar + sinangle * camera.zfar;
    float pl_y = sinangle * camera.zfar - cosangle * camera.zfar;
    float pr_x = cosangle * camera.zfar - sinangle * camera.zfar;
    float pr_y = sinangle * camera.zfar + cosangle * camera.zfar;

    for (int i = 0; i < SCREEN_WIDTH; i++) {
      // dx: delta x
      float dx = (pl_x + (pr_x - pl_x) / SCREEN_WIDTH * i) / camera.zfar;
      // dy: delta y
      float dy = (pl_y + (pr_y - pl_y) / SCREEN_WIDTH * i) / camera.zfar;

      float ray_x = camera.x;
      float ray_y = camera.y;

      // y value grows downward, so lowest is actually the screen height
      float maxheight = SCREEN_HEIGHT;

      for (int z = 1; z < camera.zfar; z++) {
        // Update the ray's slope (note ray_y is subtracted)
        ray_x += dx;
        ray_y -= dy;

        // Find the offset we have to go, fetch the value from the height map
        // Use bitwise AND operator (n&1023) to wrap the mapoffset not to be out
        // of bound
        int mapoffset = ((1024 * ((int)(ray_y)&1023)) + ((int)(ray_x)&1023));

        // We imagine camera height at 100 units above the ground
        // Project the height and find the height on the screen with:
        // y-projec = y / zfar;
        // The farther away from the viewer, the more we scale with zfar
        // And then use a "magic" scaling factor for the heightonscreen
        // And finally shift with the camera's horizon
        int projectedheight =
            (int)((camera.height - heightmap[mapoffset]) / z * SCALE_FACTOR +
                  camera.horizon);

        if (projectedheight < 0) {
          projectedheight = 0;
        }

        if (projectedheight > SCREEN_HEIGHT) {
          projectedheight = SCREEN_HEIGHT - 1;
        }

        // Render terrain pixels if the projected height on the screen is higher
        // than the max_height
        if (projectedheight < maxheight) {
          // Draw pixels from the previous max_height until the new projected
          // height
          for (int y = projectedheight; y < maxheight; y++) {
            framebuffer[(SCREEN_WIDTH * y) + i] = (uint8_t)colormap[mapoffset];
          }
          maxheight = projectedheight;
        }
      }
    }

    framebuffer = swapbuffers();

    if (keystate(KEY_ESCAPE))
      break;
  }

  return EXIT_SUCCESS;
}
