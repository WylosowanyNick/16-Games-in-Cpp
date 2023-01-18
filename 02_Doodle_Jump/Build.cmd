@echo off
g++ -I../SFML/include main.cpp ^
 -L../SFML/bin -lsfml-audio-2 -lsfml-audio-d-2 -lsfml-graphics-2 -lsfml-graphics-d-2 -lsfml-network-2 -lsfml-network-d-2 -lsfml-system-2 -lsfml-system-d-2 -lsfml-window-2 -lsfml-window-d-2 ^
 -o main.exe
