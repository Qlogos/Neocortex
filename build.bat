@echo off
gcc *.c -L. -lraylib -lopengl32 -lgdi32 -lwinmm -o viz.exe && viz.exe
