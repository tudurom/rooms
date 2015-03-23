#include "libtcod.hpp"
#define RELEASE 0

// Window
#define ROOT_WINDOW_WIDTH 80
#define ROOT_WINDOW_HEIGHT 50
#define ROOT_WINDOW_TITLE "In catacombele scolii"

// Map
#define MAP_WIDTH 80
#define MAP_HEIGHT 43
#define ROOM_MAX_SIZE 12
#define ROOM_MIN_SIZE 6
#define MAX_ROOM_MONSTERS 3
#define MAX_ROOM_ITEMS 2

#define FOV_RADIUS 0
#define COMPUTE_FOV true

// Monsters
#define MONSTER_TRACKING_TURNS 3

// GUI
#define GUI_PANEL_HEIGHT 7
#define GUI_BAR_WIDTH 20
#define GUI_MSG_X (GUI_BAR_WIDTH + 2)
#define GUI_MSG_HEIGHT (GUI_PANEL_HEIGHT - 1)

// Inventory
#define INVENTORY_WIDTH 50
#define INVENTORY_HEIGHT 28
