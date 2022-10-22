BUILD_DIR 	= ./bin/
SRC_DIR 	= ./src/
OUTPUT_FILE	= output

CC      	= g++
CFLAGS  	= -std=c++17 -Wall -Wextra -D_FORTIFY_SOURCE=2 -pipe -Werror=format-security -O2
LIBS 		= -lglfw -lGLU -lGL -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lrt -ldl -pthread

SRCS	 	= $(wildcard $(SRC_DIR)*.cpp) $(wildcard $(SRC_DIR)*.c)
C_FILES 	= $(SRCS:%.c=%.o)
OBJS	 	= $(C_FILES:%.cpp=%.o)

run: compile
	$(BUILD_DIR)$(OUTPUT_FILE)

install:
	sudo apt install libglfw3 libglfw3-dev libxxf86vm-dev libxi-dev libxinerama-dev freeglut3-dev libassimp-dev libglm-dev

setup:
ifeq (,$(wildcard ./bin))
	mkdir $(BUILD_DIR)
endif

compile: setup $(OBJS)
	$(CC) -o $(BUILD_DIR)$(OUTPUT_FILE) $(OBJS) $(CFLAGS) $(LIBS)
	rm $(SRC_DIR)*.o

%.o: %.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

clear:
	rm $(SRC_DIR)*.o

valgrind: compile
	valgrind --track-origins=yes --leak-check=full $(BUILD_DIR)$(OUTPUT_FILE)
