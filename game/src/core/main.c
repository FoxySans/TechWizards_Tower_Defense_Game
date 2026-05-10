#include "core/app.h"
#include "world/map.h"
#include "entities/enemy_manager.h"
#include "entities/enemy.h"
#include <stdio.h>

void print_menu_header() {
    printf("\n");
    printf("  ###########################################\n");
    printf("  ##                                       ##\n");
    printf("  ##       T O W E R   D E F E N S E       ##\n");
    printf("  ##                                       ##\n");
    printf("  ###########################################\n");
    printf("\n");
}


int main(int argc, char* argv[]) {
    App app;
    int loaded = 0;
    (void)argc; (void)argv;
	
	   init_app(&app, 1920, 1080); 
	   // Windows esetén ez segít a konzolt fókuszban tartani az inicializálás után
#ifdef _WIN32
    #include <windows.h>
    SetForegroundWindow(GetConsoleWindow());
#endif
    
    while (!loaded) {
        int choice;
        print_menu_header();
        
        printf("  [1]  LEVEL 1 (Konnyu)\n");
        printf("       > 5 Alap ellenseg\n\n");
        
        printf("  [2]  LEVEL 2 (Nehez)\n");
        printf("       > 8 Alap + 2 Tank ellenseg\n\n");
		
		printf("  [3]  CUSTOM MAP - Sajat palya betoltese\n");
        printf("       > 6 Alap + 1 Tank ellenseg\n\n");
        
        printf("  [0]  Kilepes a jatekbol\n");
        printf("\n  Valasztasod: ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); 
            printf("\n  [!] Ervenytelen bemenet! Szamot adj meg!\n");
            continue;
        }
        
        if (choice == 0) {
            destroy_app(&app);
            return 0;
        }

        switch(choice) {
            case 1:
                printf("\n  [Betoltes...] Level 1 elokeszitese...\n");
                loaded = map_load_from_file(&app.scene.map, "maps/level1.txt");
                if (loaded) {
                    spawn_enemy(ENEMY_BASIC, 5);
                }
                break;
            case 2:
                printf("\n  [Betoltes...] Level 2 elokeszitese...\n");
                loaded = map_load_from_file(&app.scene.map, "maps/level2.txt");
                if (loaded) {
                    spawn_enemy(ENEMY_BASIC, 8);
                    spawn_enemy(ENEMY_TANK, 2);
                }
                break;
			case 3: 
                char filename[256];
                printf("\n  Add meg a fajl nevet (pl. maps/sajat.txt): ");
                scanf("%255s", filename);
                
                printf("  [Betoltes...] %s...\n", filename);
                loaded = map_load_from_file(&app.scene.map, filename);
                
                if (loaded) {
                    // Egyedi pályára is küldünk "ajándékot"
                    spawn_enemy(ENEMY_BASIC, 6);
                    spawn_enemy(ENEMY_TANK, 1);
                }
                break;
            default:
                printf("\n  [!] Nincs ilyen palya! Probald ujra.\n");
                break;
        }
        
        if (loaded) {
            printf("  [OK] Sikeres betoltes!\n");
        }
    }

    SDL_RaiseWindow(app.window);
    
    while (app.is_running) {
        handle_app_events(&app);
        update_app(&app);       
        render_app(&app);       
    }
    
    destroy_app(&app);
    return 0;
}