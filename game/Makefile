SRCS = src/core/app.c \
       src/core/main.c \
       src/core/utils.c \
       src/world/map.c \
       src/world/scene.c \
       src/entities/character.c \
       src/entities/enemy.c \
       src/entities/enemy_manager.c \
       src/render/texture.c \
       src/render/camera.c \

all:
	gcc -std=gnu99 -Iinclude/ $(SRCS) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lobj -lopengl32 -lm -o origin.exe -Wall -Wextra -Wpedantic

linux:
	gcc -std=c99 -Iinclude/ src/app.c src/camera.c src/main.c src/scene.c src/utils.c src/enemy.c src/enemy_manager.c -lSDL2 -lGL -lm -o origin -Wall -Wextra -Wpedantic