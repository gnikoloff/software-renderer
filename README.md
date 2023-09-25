# Software rasterizer dojo

3D renderer written as an exercise to learn C and how graphics APIs such as OpenGL are implemented under the hood.

![Shadow mapping demo preview](https://github.com/gnikoloff/software-renderer/blob/main/previews/shadow-mapping-demo.png?raw=true)

## Features

1. Software rasterization
2. Pure C17 implementation
3. Right hand Y-up coordinate system
4. Uses [SDL](https://www.libsdl.org/) for access to graphics hardware, mouse, keyboard, etc.
5. Supports native and web via [emscripten](https://emscripten.org/)
6. Perspective correct interpolation
7. Support for vertex and fragment shaders
8. Depth buffer
9. Face culling
10. Viewport clipping
11. Loading and parsing Wavefront .OBJ files and PNG images
12. Cube map sampling

## References and readings
1. [3D Computer Graphics Programming](https://pikuma.com/courses/learn-3d-computer-graphics-programming)
2. [Modern C](https://www.manning.com/books/modern-c)
3. [niepp/spbr](https://github.com/niepp/srpbr)
4. [Lode's Computer Graphics Tutorial](https://lodev.org/cgtutor/)

## Building for native

Clone the project and run in the terminal:

```
make build DEMO_NAME=DESIRED_DEMO_NAME
make run DEMO_NAME=DESIRED_DEMO_NAME
```

Where `DESIRED_DEMO_NAME` is the demo you want to build and run.

Here is a full list of the demo names:

```
GEOMETRY_EXAMPLE
SHADOWMAP_EXAMPLE
PHYSICS2D_EXAMPLE
DEPTHBUFFER_EXAMPLE
ENVIRONMENTMAPPING_EXAMPLE
PLASMA_EXAMPLE
TUNNEL_EXAMPLE
```

## Building for web

Clone the project and run in the terminal:

```
make build-emscripten DEMO_NAME=DESIRED_DEMO_NAME
```

To run the web server to view it in a browser run:

```
npm i
npm start
```

This will start a local web server you can open in your browser of choice.

| ![3D Geometry demo preview](https://github.com/gnikoloff/software-renderer/blob/main/previews/geometry-demo.png?raw=true) |
|:--:|
| Geometries |

| ![Depth buffer demo preview](https://github.com/gnikoloff/software-renderer/blob/main/previews/depth-buffer-demo.png?raw=true) |
|:--:|
| Visualised depth buffer |

| ![Environment mapping demo preview](https://github.com/gnikoloff/software-renderer/blob/main/previews/environment-mapping-demo.png?raw=true) |
|:--:|
| Environment mapping |

| ![Plasma demo preview](https://github.com/gnikoloff/software-renderer/blob/main/previews/plasma-demo.png?raw=true) |
|:--:|
| Plasma |

| ![Tunnel demo preview](https://github.com/gnikoloff/software-renderer/blob/main/previews/tunnel-demo.png?raw=true) |
|:--:|
| Tunnel |

| ![2D Physics demo preview](https://github.com/gnikoloff/software-renderer/blob/main/previews/2d-physics-demo.png?raw=true) |
|:--:|
| 2D Physics |