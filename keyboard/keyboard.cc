/**
 * @file keyboard.cc
 * @b family keyboard
 * @b type keyboard
 * @author Leonardo Guilherme de Freitas
 * @namespace keyboard
 * 
 * Default keyboard component. It provides the state of the key
 * as an integer with 4 possible values:
 * 
 * 0 for unpressed
 * 1 for clicked
 * 2 for pressed
 * 3 for released
 * 
 * Keys that match printable characters are identified by them:
 *
 * a - a key
 * b - b key
 * c - c key
 * 1 - 1 key
 * 2 - 2 key
 * 
 * and so on.
 * 
 * For those non-printable:
 * 
 * arrow-up, arrow-down, arrow-right, arrow-left for the arrows
 * ctrl-left - left control
 * ctrl-right - right control
 * alt-left - left alt
 * alt-right - right alt
 * shift-left - left shift
 * shift-right - right shift
 * tab - tab key
 * backspace - backspace key
 * f{1-12} - function keys 1 through 12
 * (space char) - space
 * return - return key
 * 
 * @b Parameters:
 * @c keyboard.interested: keys that you are interested in knowing its state. They will be provided as "key.{key}" in parameters (like key.c for c key). @b string
 * @c key.{key}: State of the key in that frame. You need to have that key in keyboard.interested if you want to know about it. @b integer
 * @c keyboard.text: Unicode characters read in this frame @b wstring
 */

#include "gear2d.h"
using namespace gear2d;

#include <iostream>
#include <set>
#include <vector>
using namespace std;

#include "SDL.h"

class keyboard : public component::base {
  private:
    enum keystate {
      UNPRESSED,
      CLICKED,
      PRESSED,
      RELEASED
    };

	typedef struct {
		int      state;
		bool     changed;
	} keystatus;
  private:
    // array with keystates
    static const Uint8 * kbstate;
    
    // A map between a SDL_Keycode and keystatus
    static map<SDL_Scancode, keystatus> keys;
    
    // set of all keys we have to check
    static std::set<SDL_Scancode> usedkeys;
    
    // all kb components
    static set<keyboard *> kbcomponents;
    
    // how many components asked the update
    static int updated;
    
    // total of components of this kind
    static int updaters;
    
    // initialized?
    static bool initialized;
    
  private:
    set<string> mykeys;
    
  public:
    keyboard() {
    };
    
    virtual ~keyboard() { 
      updaters--;
      kbcomponents.erase(this);
    }
    virtual component::type type() { return component::type("keyboard"); }
    virtual component::family family() { return component::family("keyboard"); }
    virtual void handleinterest(parameterbase::id pid, component::base * lastwrite, object::id owner) {
      loginfo;
      if (pid == "keyboard.interested") {
        interest(read<string>("keyboard.interested"));
        trace("keyboard.interested ", read<string>("keyboard.interested"));
      }
    }
    
    virtual void setup(object::signature & sig) {
      if (!initialized) initialize();
      // check the interested keys
      string interested = sig["keyboard.interested"];
      write("keyboard.interested", std::string(""));
      hook("keyboard.interested", (component::call)&keyboard::handleinterest);
      write<string>("keyboard.interested", sig["keyboard.interested"]);
      write<std::wstring>("keyboard.text", std::wstring());
      updaters++;
      kbcomponents.insert(this);
    }
    
    virtual void update(timediff dt) {
      updated++;
      if (updated >= updaters) {
        doupdate();
        updated = 0;
      }
    }
    
  private:
    // this is called so each kb components gets a chance to warn
    // their friends of kb changes.
    void kbupdate() {
      for (std::set<string>::iterator it = mykeys.begin(); it != mykeys.end(); it++) {
        SDL_Scancode key = SDL_GetScancodeFromName(it->c_str());
        // values has changed, indeed. notify
        if (keys[key].changed == true) {
          write(string("key.") + *it, keys[key].state);
        }
      }
    }
    
    void interest(string interested) {
      modinfo(type());
      std::set<string> keylist;
      split(keylist, interested, ' ');
      trace("Interested keys:", interested);
      for (std::set<string>::iterator it = keylist.begin(); it != keylist.end(); it++) {
        // build keyname
        string keyname = *it;
        SDL_Scancode scode = SDL_GetScancodeFromName(keyname.c_str());
        if (scode == SDL_SCANCODE_UNKNOWN) {
          trace("Key", keyname, "does not have a name. Dropping it.", log::warning);
          continue;
        }

        // keep our own list of keys
        mykeys.insert(keyname);
        
        // add the 'key.' prefix to keyname before binding it.
        keyname.insert(0, "key.");

        // initialize the key
        write<int>(keyname, (int)UNPRESSED);
        
        // tell the component system to check for these keys
        usedkeys.insert(scode);
      }
    }
  private:
    static void doupdate() {
      if (usedkeys.size() == 0) return;
      
      for (auto it = usedkeys.begin(); it != usedkeys.end(); it++) {
        SDL_Scancode key = *it;
        int rawstate = kbstate[key]; // pressed or not-pressed
        int status = keys[key].state; // unpressed, clicked, pressed, released

        keys[key].changed = false;

        // unpressed
        if (rawstate == 0) {
          // status is clicked or pressed, next: released
          if (status == CLICKED || status == PRESSED) {
            keys[key].state = (int)RELEASED;
            keys[key].changed = true;
          }
      
          // status is relesed, next: unpressed
          else if (status == RELEASED) {
            keys[key].state = (int)UNPRESSED;
            keys[key].changed = true;
          }
        }
        // pressed
        else if (rawstate == 1) {
          // if its on a non-pressing state, set as clicked
          if (status == UNPRESSED || status == RELEASED) {
            keys[key].state = (int)CLICKED;
            keys[key].changed = true;
          }
          
          // status is clicked, next: pressed
          else if (status == CLICKED) {
            keys[key].state = (int)PRESSED;
            keys[key].changed = true;
          }
        }
      }
      for (set<keyboard *>::iterator i = kbcomponents.begin(); i != kbcomponents.end(); i++) {
        (*i)->kbupdate();
      }
    }

    static void initialize() {
      if (!SDL_WasInit(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
          logerr;
          trace("Event thread initialization failed!", SDL_GetError());
          throw (evil(string("(Keyboard Component) Event threat init fail! ") + SDL_GetError()));
        }
      }
      kbstate = SDL_GetKeyboardState(NULL);
      SDL_NUM_SCANCODES;
      
      // initialize keynames...
      initialized = true;
    }
};

int keyboard::updated = 0;
int keyboard::updaters = 0;
bool keyboard::initialized = false;
const Uint8 * keyboard::kbstate = 0;
std::set<SDL_Scancode> keyboard::usedkeys;
std::map<SDL_Scancode, keyboard::keystatus> keyboard::keys;
set<keyboard *> keyboard::kbcomponents;

g2dcomponent(keyboard, keyboard, keyboard);
