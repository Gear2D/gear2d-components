#include "touch.h"
#include "SDL.h"
#include "gear2d.h"
#include <unordered_map>

using namespace std;

void touch::setup(gear2d::object::signature & sig) {
  initialize();
  components.insert(this);
  myfingers.resize(10);
  char buf[255];
  for (int i = 0; i < myfingers.size(); i++) {
    sprintf(buf, "%d", i);
    finger & f = myfingers[i];
    string id = "touch."; id += buf;
    f.state = fetch<int>(id, 0);
    f.x = fetch<float>(id + ".x", 0.0f);
    f.y = fetch<float>(id + ".y", 0.0f);
    f.pressure = fetch<float>(id + ".pressure", 0.0f);
  }
}

void touch::update(gear2d::timediff dt) {
  touchupdate();
  
  //modinfo("touch");
  /* walk through all active fingers. If active, update, if not, don't */
  for (int i = 0; i < activefingers.size(); i++) {
    SDL_Finger * f = activefingers[i];
    finger & myf = myfingers[i];
    
    if (f != nullptr) {
      //trace(f->id, "is active. Updating it!");
      // truly an active finger!
      myf.x = f->x;
      myf.y = f->y;
      myf.pressure = f->pressure;
      if (myf.state == 0)
        myf.state = 1; /* this might trigger a pull() */
    } else {
      if (myf.state == 1) {
        /* not an active finger. update its state */
        myfingers[i].pressure = 0.0f;
        myfingers[i].state = 0;
      }
    }      
  }
}

void touch::destroyed() {
  components.erase(this);
}


void touch::initialize() {
  static bool initialized = false;
  moderr("touch");
  if (!initialized) {
    if (!SDL_WasInit(SDL_INIT_EVENTS)) {
      if (!SDL_InitSubSystem(SDL_INIT_EVENTS)) {
        trace("Could not initialize SDL events subsystem!");
        return;
      }
    }
    
    int ndevices = SDL_GetNumTouchDevices();
    if (ndevices <= 0) {
      trace("Couldn't find any touch devices. Touch support might be disabled or added later");
    }
    trace.i("Found", ndevices, "touch device(s)");
    initialized = true;
  }
}

void touch::touchupdate() {
  static unsigned int u = 0;
  u++;
  if (u < components.size()) return;
  if (SDL_GetNumTouchDevices() <= 0) return;
  SDL_TouchID device = SDL_GetTouchDevice(0);
  int nfingers = SDL_GetNumTouchFingers(device);
  
  // maybe we just got a lot of fingers
  if (nfingers > activefingers.capacity())
    activefingers.reserve(nfingers);
    
  if (nfingers == 0) {
    activefingers.assign(activefingers.capacity(), nullptr);
  }
  
  std::unordered_map<SDL_FingerID, SDL_Finger *> readfingers;
  int emptyspot = -1;
  //modinfo("touch");    
  //trace("Got", nfingers);
  for (int i = 0; i < nfingers; i++) {
    // get the corresponding finger
    SDL_Finger * f = SDL_GetTouchFinger(device, i); 
    
    // may be null, ignore it then.
    if (f == nullptr) continue;
      
    //trace("Finger", i, "is ID", f->id);
      
    // store this id, we need to reverse-check;
    readfingers[f->id] = f;
  }
  
  // we need to place this finger somewhere in an empty spot
  emptyspot = -1;
  /* iterate through all of the fingers we got so far */
  for (auto f : readfingers) {
    
    // marks if we found f->id in our activefingers array
    bool found = false;
    
    // look in all previously activefingers for f->id, it might be already stored.
    for (int i = 0; i < activefingers.capacity(); i++) {
      // while we look for this ID, look for an empty spot.
      if (activefingers[i] == nullptr) {
        /* if this is null,  this is an empty spot */
        if(emptyspot == -1) { emptyspot = i; }
        continue;
      } else {
        // see if this finger was dropped.
        if (readfingers.find(activefingers[i]->id) == readfingers.end()) {
          /* a-ha! This is an old finger. Remove it, store a new empty spot */
          //trace(activefingers[i]->id, "not active. Emptying it at.", i, "which is now an empty spot");
          activefingers[i] = nullptr;
          if(emptyspot == -1) { emptyspot = i; }
          continue;
        }
        
        /* this finger is still being watched, stay there. */
        if (activefingers[i]->id == f.first) {
          //trace(f.first, "is already here, found at", i);
          activefingers[i] = f.second;
          found = true;
          break;
        }
      }
    }

    /* we haven't found this finger->id, we need an empty spot for it! */
    if(found == false) {
      if(emptyspot == -1) {
        /* no previous empty spot found, search for it. */
        for(int i = 0; i < activefingers.capacity(); i++) {
          if(activefingers[i] == nullptr) {
            emptyspot = i;
            break;
          }
        }
      }
      // store this new finger.
      //trace("Storing", f.first, "in", emptyspot);
      activefingers[emptyspot] = f.second;
      emptyspot = -1;
    }
  }
  
  u = 0;
}

std::set<touch *> touch::components;
std::vector<SDL_Finger *> touch::activefingers = std::vector<SDL_Finger *>(10);
g2dcomponent(touch, touch, touch)
