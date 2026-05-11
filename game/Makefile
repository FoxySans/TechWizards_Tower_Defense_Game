CC = C:/Users/Yopotka/Documents/Gina/egyetem/grafika/c_sdk/MinGW/bin/gcc.exe
SDL2_INCLUDE = C:/Users/Yopotka/Documents/Gina/egyetem/grafika/c_sdk/MinGW/include/SDL2
SDL2_LIB = C:/Users/Yopotka/Documents/Gina/egyetem/grafika/c_sdk/MinGW/lib

all:
	$(CC) -std=gnu99 -Iinclude/ -I$(SDL2_INCLUDE) src/app.c src/camera.c src/main.c src/scene.c src/map.c src/menu.c src/button.c src/utils.c src/music.c src/slider.c -L$(SDL2_LIB) -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lopengl32 -lm -o origin.exe -Wall -Wextra -Wpedantic
	
linux:
	gcc -std=c99 -Iinclude/ src/app.c src/camera.c src/main.c src/scene.c src/utils.c src/menu.c -lSDL2 -lGL -lm -o origin -Wall -Wextra -Wpedantic