/**
 * @file mouse.cc
 * @namespace mouse
 * @brief Provides mouse input.
 * 
 * @b Parameters:
 * @li mouse.{x|y} x and y position for the mouse cursor
 * @li mouse.{1|2|3} buttons one through 3 of the mouse
 * It reports the position where the mouse is and which of the three buttons are clicked.
 */


#include "gear2d.h"
#include "SDL.h"
using namespace gear2d;

class mouse : public component::base {
  public:
    mouse() {
      voters++;
    }
    
    virtual ~mouse() {
      voters--;
    }
    
    virtual component::type type() { return "mouse"; }
    virtual component::family family() { return "mouse"; }
    virtual void setup(object::signature & sig) {
      if (!initialized) initialize();
      write<int>("mouse.x", 0);
      write<int>("mouse.y", 0);
      write<int>("mouse.1", 0);
      write<int>("mouse.2", 0);
      write<int>("mouse.3", 0);
    }
    
    virtual void update(timediff dt) {
      voting++;
      if (voting >= voters) {
        globalupdate(dt);
        voting = 0;
      }

      if (read<int>("mouse.x") != x) write("mouse.x", x);
      if (read<int>("mouse.y") != y) write("mouse.y", y);
      if (read<int>("mouse.1") != bt[0]) { write<int>("mouse.1", bt[0]); write<int>("mouse.left", bt[0]); }
      if (read<int>("mouse.2") != bt[1]) { write<int>("mouse.2", bt[1]); write<int>("mouse.middle", bt[1]); }
      if (read<int>("mouse.3") != bt[2]) { write<int>("mouse.3", bt[2]); write<int>("mouse.right", bt[2]); }
    }
    
  private:
    static int voters;
    static int voting;
    
    /* pos */
    static int x;
    static int y;
    
    static int bt[5];
    static bool initialized;
    
  private:
    
    /* use watchers because mouse x/y is clipped to screen with events, but not with getmousestate */
    static int mousewatcher(void * userdata, SDL_Event * ev) {
      switch (ev->type) {
        case SDL_MOUSEMOTION:
          x = ev->motion.x;
          y = ev->motion.y;
          break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
          x = ev->button.x;
          y = ev->button.y;
          break;
        default:
          break;
      }
      return 1;
    }
    
    static void initialize() {
      if (initialized) return;
      if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
          logerr;
          trace("Event thread initialization failed!", SDL_GetError());
          throw (evil(std::string("(Keyboard Component) Event threat init fail! ") + SDL_GetError()));
          return;
        }
      }
      
      /* add mouse watcher */
      SDL_AddEventWatch(mousewatcher, NULL);
      
      memset(bt, 0, sizeof(bt));
      initialized = true;
    }
    static void globalupdate(timediff dt) {
      if (!initialized) return;
      Uint8 flags = SDL_GetMouseState(NULL, NULL);
      
      /* iterate through buttons */
      for (int i = 0; i < 5; i++) {
        int & b = bt[i];
        if (flags & SDL_BUTTON(i+1)) { /* is it pressed now? */
          if (b == 0) 
            b = 1; /* if it was not clicked, it is now */
          else if (b == 1)
            b = 2; /* if it was clicked, it is now pressed */
        } else { /* is it not-pressed now? */
          if (b == 2)
            b = 3; /* it was not pressed, now is released */
          else if (b == 3)
            b = 0; /* it was released, now its not clicked */
        }
      }
    }
};

int mouse::voters = 0;
int mouse::voting = 0;
int mouse::x = 0;
int mouse::y = 0;
int mouse::bt[5];
bool mouse::initialized = false;

g2dcomponent(mouse, mouse, mouse);