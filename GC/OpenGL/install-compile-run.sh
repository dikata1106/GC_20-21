#!/bin/bash

sudo apt-get update
sudo apt-get install g++ libglu1-mesa-dev freeglut3-dev mesa-common-dev
g++ main.cpp -o firstOpenGlApp -lglut -lGLU -lGL
./firstOpenGlApp
