# Software rasterizer dojo

3D renderer written in C. I used this project to learn more about C and how graphics APIs such as OpenGL are implemented under the hood.

![3D Geometry demo preview](https://github.com/gnikoloff/software-renderer/blob/main/previews/geometry-demo.png?raw=true)

## Features

1. Software rasterization
2. Pure C17 implementation
3. Right hand Y-up coordinate system
4. Uses [SDL](https://www.libsdl.org/) for access to graphics hardware, mouse, keyboard, etc.
5. Supports native and web via [emscripten](https://emscripten.org/)
5. Perspective correct interpolation
6. Depth buffer
7. Face culling
8. Viewport clipping
9. Loading and parsing:
   8.1. Wavefront .OBJ files
	 8.2. PNG images
10. Cube map sampling

## Building for native

Clone the project and run in the terminal:

```
	make build DEMO_NAME=${DESIRED_DEMO_NAME}
	make run DEMO_NAME=${DESIRED_DEMO_NAME}
```

Where `DESIRED_DEMO_NAME` is the demo you want to build and run.

Here is a full list of the demos:

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
	make build-emscripten DEMO_NAME=${DESIRED_DEMO_NAME}
```

To run the web server to view it in a browser run:

```
	npm i
	npm start
```

This will start a local web server you can open in your browser of choice.

| ![Depth buffer demo preview](https://github.com/gnikoloff/software-renderer/blob/main/previews/depth-buffer-demo.png?raw=true) |
|:--:|
| Visualised depth buffer |

| ![Shadow mapping demo preview](https://github.com/gnikoloff/software-renderer/blob/main/previews/shadow-mapping-demo.png?raw=true) |
|:--:|
| Shadow mapping |

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