set isdl=C:\Libraries\SDL2-2.26.0\include
set isdlimage=C:\Libraries\SDL2_image-2.6.3\include
set lsdl=C:\Libraries\SDL2-2.26.0\lib\x64
set lsdlimage=C:\Libraries\SDL2_image-2.6.3\lib\x64
set sources=src\bitbox.cpp src\device.cpp src\ui.cpp
g++ -g -Iinclude -I%isdl% -I%isdlimage% %sources% -o bitbox.exe -L%lsdl% -L%lsdlimage% -lSDL2 -lSDL2_image