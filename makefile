UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	FLAGS = -lglut -lGL -lGLU -std=c++11 -lSOIL
endif
ifeq ($(UNAME_S),Darwin)
	FLAGS = -framework GLUT -framework OpenGL -std=c++11 -lSOIL -framework CoreFoundation
endif

# OBJS specifies which files to compile as part of the project
OBJS = src/Main.cc src/Model.cc src/View.cc src/Controller.cc src/utils/FastTrackball.cc

# CC specifies which compiler we're using
CC = g++

# Cocoa, IOKit, and CoreVideo are needed for static GLFW3.
# LINKER_FLAGS = -framework OpenGL -lglfw3 -lglew

# OBJ_NAME specifies the name of our exectuable
OBJ_NAME = runThis

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(FLAGS) -o $(OBJ_NAME)
	./$(OBJ_NAME)