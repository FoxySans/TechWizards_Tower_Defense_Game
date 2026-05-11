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
       src/obj/load.c \
       src/obj/draw.c

# Windows (MinGW) fordítás
all:
	gcc -std=gnu99 -Isrc/entities -Iinclude -Iobj/include src/core/app.c src/core/main.c src/core/utils.c src/world/map.c src/world/scene.c src/entities/character.c src/entities/enemy.c src/entities/enemy_manager.c src/render/texture.c src/render/camera.c obj/src/load.c obj/src/draw.c obj/src/model.c obj/src/transform.c obj/src/info.c -lmingw32 -lSDL2main -Llib -lSDL2 -lSDL2_image -lSDL2_ttf -lopengl32 -lm -o origin.exe -Wall -Wextra -Wpedantic
# Linux fordítás (itt is javítottam az elérési utakat a struktúrádnak megfelelően)
linux:
	gcc -std=c99 -Iinclude/ $(SRCS) -lSDL2 -lSDL2_image -lGL -lm -o origin -Wall -Wextra -Wpedantic