#include "Gui.hpp"

class Engine {
public:
  enum GameStatus {
    STARTUP,
    IDLE,
    NEW_TURN,
    VICTORY,
    DEFEAT
  } gameStatus;

  TCODList<Actor *> actors;
  Actor *player;
  Map *map;
  int fovRadius;
  TCOD_key_t lastKey;
  TCOD_mouse_t mouse;
  int width, height;
  Gui *gui;
  
  Engine(int width, int height);
  ~Engine();
  void update();
  void render();
  void sendToBack(Actor *actor);
  bool pickAtTile(int *x, int *y, float maxRange = 0.0f);
  Actor* getClosestMonster(int x, int y, float range) const;

private:
  Actor* createPlayer(char playerName[]);
  void endGame();
  bool initMusic();
  bool freeMusic();
  void fadePrint(const char *msg);
};

extern Engine engine;

