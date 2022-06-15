# D3D9 OBJ Loader
Just a simple class that I've made to load .obj models into Direct3D9.

# How to use it
Just drag the OBJ.cpp and OBJ.hpp files into your project, include "OBJ.hpp" and create a new object of type OBJ or OBJ*.

You can choose between 2 contructors: both takes a pointer to an **IDirect3DDevice9** (your D3D Device interface) and a **const char*** string with the path of the .obj file.
The third parameter differ between the constructors, so you can choose to put in the texture path as a **const char*** string or a pointer to an already existing **IDirect3DTexture9** interface.

When you want to render your model you MUST call **OBJ::Render()** between **IDirect3DDevice9::BeginScene()** and **IDirect3DDevice9::EndScene()**.

# Notes
This OBJ Loader supposes you didn't syntactically modified your .obj file, what this means is that you can't add spaces and/or other characters to crucial lines (the ones starting with 'v', 'vt', 'vn' and 'f').

This OBJ Loader is very simple and its only purpose is to read vertexes and render them in the right order. You can't use or interact with groups, smooth shading, materials, etc.
