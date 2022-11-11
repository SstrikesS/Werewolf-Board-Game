#OBJS specifies which files to compile as part of the project
OBJS = test.c 
#CC specifies which compiler we're using
CC = gcc
#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I src\include
#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L src\lib
#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
# -Wall get all warnings (use when dev game)
COMPILER_FLAGS = -Wall #-w -Wl,-subsystem,windows
#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lws2_32 -lSDL2_image
#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = test
#This is the target that compiles our executable
all : $(OBJS) 
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
clean:
	del test.exe