/**
 * @file rigidbody2d.cc
 * @b family dynamics
 * @b type rigidbody2d
 * @namespace dynamics
 * @author Leonardo Guilherme de Freitas
 *
 * This components reacts to collision making speed change
 * as if the object was bouncing.
 *
 * @b depends
 * @li collider, kinematics, spatial
 *
 * @b using
 * @li @c collider.collision Hooking point to know when we collided (from: collider)
 * @li @c collider.collision.side Side where the collision happened (from: collider)
 * @li @c collider.collision.speed.{x|y} As the speed of the other colliding object (from: collider)
 *
 * @b provides
 * @li @c dynamics.nogravity Should this component ignore gravity acceleration? defaults to @c false @b bool (0 or 1)
 * @li @c dynamics.kinematic Should this component not move in reacting to collision (like a wall) or to gravity. (0 or 1) defaults to @c false @b bool
 * @li @c dynamics.mass Mass of this object in kilograms. Defaults to @c 0.1 @b float
 *   Please note that even if kinematic, its mass still makes diference in reaction calculations
 *
 * @b global
 *  Objects cannot override these, shall be set at gear2d.yaml
 * @li @c dynamics.gravity.{x|y} Global gravity vector. Global parameter affecting all objects. @b float
 * @li @c dynamics.drag.{x|y} Global friction vector to go against every body in movement. @b float
 *
 *
 *   Note that this component (as should be with any dynamics component) only
 *   correctly react with objects that have mass defined (so have some sort of dynamics component)
 */

#include <cmath>

#include "gear2d.h"
using namespace gear2d;

#include <limits>
using namespace std;

class bounce : public component::base {
  private:
    enum side {
      left = 0, 
      top = 1, 
      right = 2, 
      bottom = 3
    };
    float dt;
    static float gx;
    static float gy;
    static float dragx;
    static float dragy;
    static bool initialized;
    
    gear2d::link<float> x, y, w, h;

  public:
    bounce() : dt(0) { }
    virtual component::family family() { return "dynamics"; }
    virtual component::type type() { return "rigidbody2d"; }
    virtual std::string depends() { return "spatial/space2d collider/collider2d kinematics/kinematic2d"; }
    virtual void handle(parameterbase::id pid, component::base * lastwrite, object::id pidowner) {
      if (pid == "collider.collision") {
        component::base * c = read<component::base *>("collider.collision");
        // we do nothing with an object without mass
        if (!c->exists("dynamics.mass")) return;
        if (read<bool>("dynamics.kinematic")) return;
        const float & m1 = raw<float>("dynamics.mass");
        const float & m2 = c->raw<float>("dynamics.mass");
        
        // detect which side we-re colliding.
        const float & xspeed = raw<float>("x.speed");
        const float & yspeed = raw<float>("y.speed");
        const int & side = raw<int>("collider.collision.side");
        const float & colxspeed =  raw<float>("collider.collision.speed.x");
        const float & colyspeed =  raw<float>("collider.collision.speed.y");

        // adjust x and y to non-colliding positions
        float resxspeed, resyspeed;
        if (side == left || side == right) {
          resxspeed = ((m1 - m2) / (m1 + m2)) * xspeed + ((2 * m2) / (m1 + m2)) * colxspeed;
          write<float>("x.speed", resxspeed);
          // if side == right, add minus collision width
          if (side == right) add("x", -read<float>("collider.collision.w"));
          
          // if side == left, add collision width
          if (side == left) add("x", read<float>("collider.collision.w"));
          //collider.collision.{x|y|w|h}
        }
        if (side == top || side == bottom) {
          resyspeed = ((m1 - m2) / (m1 + m2)) * yspeed + ((2 * m2) / (m1 + m2)) * colyspeed;
          write<float>("y.speed", resyspeed);
          if (side == top) add("y", -read<float>("collider.collision.h"));
          if (side == bottom) add("y", read<float>("collider.collision.h"));
        }
      }
    }

    virtual void setup(object::signature & sig) {
      if (!initialized) initialize(
          eval(sig["dynamics.gravity.x"], 0.0f),
          eval(sig["dynamics.gravity.y"], 0.0f),
          eval(sig["dynamics.drag.x"], 0.0f),
          eval(sig["dynamics.drag.y"], 0.0f)
        );

      if (sig["dynamics.mass"] == "infinity") {
        write<float>("dynamics.mass", numeric_limits<float>::infinity());
      } else {
        init<float>("dynamics.mass", sig["dynamics.mass"], 1.0f);
      }
      init<bool>("dynamics.kinematic", sig["dynamics.kinematic"], false);
      init<bool>("dynamics.nogravity", sig["dynamics.nogravity"], false);
      hook("collider.collision");
      x = fetch<float>("x");
      y = fetch<float>("y");
      w = fetch<float>("w");
      h = fetch<float>("h");
    }


    virtual void update(float dt) {
      this->dt = dt;
      if (read<bool>("dynamics.kinematic") == true || read<bool>("dynamics.nogravity") == true) return;
      else {
        add("x.speed", gx * dt);
        add("y.speed", gy * dt);
      }

//       add("x.speed", read<float>("x.speed") * -dragx *dt);
    }

  private:
    void initialize(float gvx, float gvy, float dvx, float dvy) {
      if (initialized) return;
      gx = gvx;
      gy = gvy;
      dragx = dvx;
      dragy = dvy;
    }
};

float bounce::gx = 0.0f;
float bounce::gy = 0.0f;
float bounce::dragx = 0.0f;
float bounce::dragy = 0.0f;
bool bounce::initialized = false;

g2dcomponent(bounce, dynamics, rigidbody2d);

