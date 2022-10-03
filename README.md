# voxel-game
minecraft like voxel rendering with extra sexappeal.

This is my own implementation of a Minecraft-clone, but instead of programming all the boring game code I just wrote a renderer. It is written in C++ using OpenGL. This project was just to learn different rendering techniques, therefore most of the code is not really efficient nor well designed.

<img src="https://i.imgur.com/Xnv9rR5.png" width=500/>

# specs
  - specular, diffuse and ambient lighting
  - water reflections & distrotions
    - using a dudvmap
  - shadows
  - bloom
  - volumetric lighting
  - skybox
  - fog
  
# build
  - you can compile & run the project using the Makefile
  - only supports linux
  - simply run: ```make install``` to install everything and ```make run``` to run it
  - output will be ```./bin/output```
  
# sources
  - [Joey de Vries](https://learnopengl.com/) check him out, his blog is amazing.
  - [Alexandre Pestana](https://www.alexandre-pestana.com/volumetric-lights/) awesome tutorial on volumetric lighting.
  - [stb_images.h](https://github.com/nothings/stb/blob/master/stb_image.h) for loading images
  - [glad.c](https://github.com/Dav1dde/glad) GL loader
