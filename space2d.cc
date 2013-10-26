/**
 * @file space2d.cc
 * @b type space2d
 * @b family spatial
 * @author Leonardo Guilherme de Freitas
 * @namespace spatial
 * Spatial component that handles and process positioning,
 * size and scale of objects
 * 
 * Parameters:
 */

#include "gear2d.h"
using namespace gear2d;

#include <iostream>
using namespace std;

class spatial : public component::base {
  private:
    gear2d::link<float> x, y, z, w, h, d, cx, cy, cz;
    
  public:
    spatial() {
    };
    
    virtual ~spatial() { }
    virtual component::type type() { return "space2d"; }
    virtual component::family family() { return "spatial"; }
    virtual void handle (parameterbase::id pid, base * lastwrite, object::id owner) {
      modinfo("space2d");
      if (lastwrite == this) return;
      trace(pid, "for", this->owner->name(), "has changed!");
      
      /* not an update on the center */
      if (pid == "x" || pid == "w")
        cx = x + (w/2);
      
      if (pid == "y" || pid == "h")
        cy = y + (h/2);
      
      if (pid == "z" || pid == "d")
        cz = z + (h/2);
      
      if (pid == "cx")
        x = cx - (w/2);
      
      if (pid == "cy")
        y = cy - (w/2);
      
      if (pid == "cz")
        z = cz - (w/2);
    }
    
    virtual void setup(object::signature & sig) {
      init("x", sig["x"], 0.0f);
      x = fetch<float>("x");
      
      init("y", sig["y"], 0.0f);
      y = fetch<float>("y");
      
      init("z", sig["z"], 0.0f);
      z = fetch<float>("z");
      
      init("w", sig["w"], 0.0f);
      w = fetch<float>("w");
      
      init("h", sig["h"], 0.0f);
      h = fetch<float>("h");
      
      init("d", sig["d"], 0.0f);
      d = fetch<float>("d");
      
      init("cx", sig["cx"], 0.0f);
      cx = fetch<float>("cx");
      
      init("cy", sig["cy"], 0.0f);
      cy = fetch<float>("cy");
      
      init("cz", sig["cz"], 0.0f);
      cz = fetch<float>("cz");
      
      if (x != 0.0f)
        cx = x + (w/2);
      else if (cx != 0.0f)
        x = cx - (w/2);
      
      if (y != 0.0f)
        cy = y + (h/2);
      else if (cy != 0.0f)
        y = cy - (h/2);
      
      if (z != 0.0f)
        cz = z + (d/2);
      else if (cz != 0.0f)
        z = cz - (d/2);
#if 0
      hook("x"); hook("y"); hook("z"); hook("w"); hook("h"); hook("d");
      hook("cx"); hook("cy"); hook("cz");
#endif
    }      
    
    virtual void update(timediff dt) {
    }
};

g2dcomponent(spatial);