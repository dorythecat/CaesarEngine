# Caesar Engine
This engine has been built from the ground up to be a similar to Paradox Game's Clausewitz engine. It is designed for use on a more "fantastical setting" type of game, instead of the gorunded games Clausewitz is known for.

# Why the name?
Both the latin word for emperor, given to not only the rulers of the Roman Empire, but also to some later historical figures of grandure. It is also the name of a character who shall reamin private :3

# Compiling
To compile the engine (in linux), execute the command `bash compile.sh`. This script will use all cores available on your computer (except two, left so you may still use your computer) to compile the engine. The engine will be compiled to the `build` directory, and can be run by executing `./build/CaesarEngine` from the source dir.

# Usage
The following keys can be used to interact with the engine:
(These keybinds can be easily edited in the code, this is just the defaults)
- `F5`: Activate wireframe mode (Debug build only)
- `F6`: Deactivate wireframe mode (Debug build only)
- `T`: Tick the engine (Debug build only)
- `Scroll Wheel`: Zoom in and out
- `WASD` or Arrow Keys: Move the camera


# Acknowledgements
- [GLFW](https://www.glfw.org/) - Window and input handling
- [GLAD](https://glad.dav1d.de/) - OpenGL function
- [MSDFGen](https://github.com/Chlumsky/msdfgen) - Font generation
- [stb_image](https://github.com/nothings/stb) - Image loading

# [Demo Video](https://github.com/dorythecat/CaesarEngine/raw/refs/heads/main/demo.mp4)
