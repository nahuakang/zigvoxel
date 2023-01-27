# Voxel Space

## Requirements

The following libraries are required for the Voxel engine
and the `dos-like` library to work and to build.

```sh
# NOTE: The libraries might be differently named on OS X
$ sudo apt install mesa-utils libsdl2-dev libglew-dev libglfw3-dev 
```

Note
```
# The following works for Macbooks
# gcc voxel.c dos/*.c `sdl2-config --libs --cflags` -lGLEW -framework OpenGL -lpthread -o voxel
```	
