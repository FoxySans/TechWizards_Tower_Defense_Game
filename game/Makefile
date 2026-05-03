all:
	gcc -std=gnu99 -Iinclude/ src/app.c src/camera.c src/main.c src/scene.c src/map.c src/menu.c src/utils.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lopengl32 -lm -o origin.exe -Wall -Wextra -Wpedantic
	
linux:
	gcc -std=c99 -Iinclude/ src/app.c src/camera.c src/main.c src/scene.c src/utils.c src/menu.c -lSDL2 -lGL -lm -o origin -Wall -Wextra -Wpedantic