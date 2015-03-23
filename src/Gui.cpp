#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "main.hpp"

Gui::Message::Message(const char *text, const TCODColor &col) : text(strdup(text)), col(col) {}

Gui::Message::~Message() {
  free(text);
}

Gui::Gui() {
  con = new TCODConsole(ROOT_WINDOW_WIDTH, GUI_PANEL_HEIGHT);
}

Gui::~Gui() {
  delete con;
  log.clearAndDelete();
}

void Gui::render() {
  con->setDefaultBackground(TCODColor::black);
  con->clear();
  
  // bara hp
  renderBar(1, 1, GUI_BAR_WIDTH, "HP", engine.player->destructible->hp, engine.player->destructible->maxHp, TCODColor::lightRed, TCODColor::darkerRed);
  // message log
  int y = 1;
  float colorCoef = 0.4f; // coeficient culoare
  for(Message **msg = log.begin(); msg != log.end(); msg++) {
    Message *message = *msg;
    con->setDefaultForeground(message->col * colorCoef);
    con->print(GUI_MSG_X, y, message->text);
    y++;
    if(colorCoef < 1.0f) {
      colorCoef += 0.3f;
    }
  }
  TCODConsole::blit(con, 0, 0, ROOT_WINDOW_WIDTH, GUI_PANEL_HEIGHT, TCODConsole::root, 0, ROOT_WINDOW_HEIGHT - GUI_PANEL_HEIGHT);
}

void Gui::renderBar(int x, int y, int width, const char *name, 
                    float value, float maxValue, const TCODColor &barColor, 
                    const TCODColor &backColor) {
  
  int barWidth = (value / maxValue) * width;
  if(barWidth) {
    con->setDefaultBackground(barColor);
    con->rect(x, y, barWidth, 1, false, TCOD_BKGND_SET);
    // textul este afisat si deasupra
    con->setDefaultForeground(TCODColor::white);
    con->printEx(x + width / 2, y, TCOD_BKGND_NONE, TCOD_CENTER, "%s : %g/%g", name, value, maxValue);
  }
}

void Gui::message(const TCODColor &col, const char *text, ...) {
  
  va_list ap;
  char buff[128];
  va_start(ap, text);
  vsprintf(buff, text, ap);
  va_end(ap);
  
  char *lineBegin = buff;
  char *lineEnd;
  do {
    // sterge ultimul mesaj daca nu avem loc pt. ultimul mesaj
    if(log.size() == GUI_MSG_HEIGHT) {
      Message *toRemove = log.get(0);
      log.remove(toRemove);
      delete toRemove;
    }
    lineEnd = strchr(lineBegin, '\n');
    if(lineEnd)
      *lineEnd = 0;
    Message *msg = new Message(lineBegin, col);
    log.push(msg);
    
    // urmatorul rand
    lineBegin = lineEnd + 1;
  }  while(lineEnd);
}
