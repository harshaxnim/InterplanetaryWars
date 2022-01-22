# Interplanetary Wars\*
*\* I do not endorse war. Make love, not War!*\
This is a game entirely built on OpenGL using C++ with the help of sound and texturing libraries. More importantly, this is also an exercise in Design Patterns for Applications with a Graphical Interfaces. The project employs the MVC Architecture: Model, View and Controller. See it in action here:
[![Link to the Youtube Video](https://img.youtube.com/vi/jJTryPLwkAM/0.jpg)](https://www.youtube.com/watch?v=jJTryPLwkAM)

## Structure

The root directory contains the following folders:

`src`: Contains the source code of the program

`doc`: Contains the documentation of the program

`dependencies`: Contains the dependencies (SoIL in this case)

`models`: assets of the program, containin the 3D models

`textures`: assets of the program, containin the textures required


## Make
Open Terminal, go to the root folder of the project. Run `make`.

## Dependencies
`SoIL` Image Library. Get it [here](http://www.lonesock.net/files/soil.zip).

## Run
`runThis` will be created as an executable after `make`. Run the program by `./runThis`.

## Misc
The code has been implemented and tested on the Mac OSX and has not been tested on a Linux Machine. However, I have made the code and makefile portable for both Linux and OSX and I believe should run on both, with varying functionality. When it was quickly, all functionality proposed in DOC worked on the OSX and only a subset on Ubuntu 14.04.
