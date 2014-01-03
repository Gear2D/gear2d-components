#ifndef touch_h
#define touch_h

#include "gear2d.h"
#include "SDL.h"
#include <set>
#include <unordered_map>

#define MAX_FINGERS 10

// Represents fingers
struct finger {
  gear2d::link<int> state;
  gear2d::link<float> x;
  gear2d::link<float> y;
  gear2d::link<float> pressure;
};

/**
 * @brief Handles touch devices
 */
class touch : public gear2d::component::base {
  public:
    virtual gear2d::component::type type() { return "touch"; }
    virtual gear2d::component::family family() { return "touch"; }

    virtual void update(gear2d::timediff dt);
    virtual void setup(gear2d::object::signature & sig);
    virtual void destroyed();
    
  private:
    sigparser sigp;
    std::vector<finger> myfingers;
    
  private:
    static void initialize();
    static void touchupdate();
    
  private:
    static std::set<touch *> components;
    static std::vector<SDL_Finger *> activefingers;
};

#endif