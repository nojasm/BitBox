g++ -g -Iinclude -Ieffects $(sdl2-config --cflags) src/bitbox.cpp src/device.cpp src/ui.cpp -o bitbox -lSDL2 -lSDL2_image