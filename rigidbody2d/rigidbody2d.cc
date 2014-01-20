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
    
    gear2d::link<bool> active, iskinematic, nogravity;
    gear2d::link<float> xspeed, yspeed;
    gear2d::link<float> x, y, w, h;
    gear2d::link<float> mass;
    
    struct collisioninfo {
      /* information about the collisor */
      gear2d::link<component::base *> collisor;
      gear2d::link<float> speedx;
      gear2d::link<float> speedy;

      /* side of the collision, relative to this object */
      gear2d::link<int> side;

      /* intersection coordinates */
      gear2d::link<float> ix;
      gear2d::link<float> iy;
      gear2d::link<float> iw;
      gear2d::link<float> ih;
    };
    
    collisioninfo collision;

  public:
    bounce() : dt(0) { }
    virtual component::family family() { return "dynamics"; }
    virtual component::type type() { return "rigidbody2d"; }
    virtual std::string depends() { return "spatial/space2d collider/collider2d kinematics/kinematic2d"; }
    virtual void collisiondetected(parameterbase::id pid, component::base * lastwrite, object::id pidowner) {
      // don't cast it because might be another dynamics component
      component::base * c = read<component::base *>("collider.collision");
      // we do nothing with an object without mass
      if (!c->exists("dynamics.mass")) return;
      if (read<bool>("dynamics.kinematic")) return;
      const float & m1 = mass;
      const float & m2 = c->raw<float>("dynamics.mass");
      
      // detect which side we-re colliding.
      const int & side = collision.side;
      const float & colxspeed =  collision.speedx;
      const float & colyspeed =  collision.speedy;

      // adjust x and y to non-colliding positions
      float resxspeed, resyspeed;
      if (side == left || side == right) {
        resxspeed = ((m1 - m2) / (m1 + m2)) * xspeed + ((2 * m2) / (m1 + m2)) * colxspeed;
        xspeed = resxspeed;
        // if side == right, add minus collision width
        if (side == right) x += -collision.iw;
        
        // if side == left, add collision width
        if (side == left) x += collision.iw;
        //collider.collision.{x|y|w|h}
      }
      if (side == top || side == bottom) {
        resyspeed = ((m1 - m2) / (m1 + m2)) * yspeed + ((2 * m2) / (m1 + m2)) * colyspeed;
        yspeed = resyspeed;
        if (side == top) y += -collision.ih;
        if (side == bottom) y += collision.ih;
      }
    }

    virtual void setup(object::signature & sig) {
      sigparser s(sig, this);
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
      hook("collider.collision", (component::call) &bounce::collisiondetected);
      mass = fetch<float>("dynamics.mass");
      x = fetch<float>("x");
      y = fetch<float>("y");
      w = fetch<float>("w");
      h = fetch<float>("h");
      active = s.init<bool>("dynamics.active", true);
      iskinematic = s.init<bool>("dynamics.kinematic", false);
      nogravity = s.init<bool>("dynamics.nogravity", false);
      xspeed = s.init<float>("x.speed", 0.0f);
      yspeed = s.init<float>("y.speed", 0.0f);
      collision.collisor = fetch<component::base *>("collider.collision");
      collision.ix = fetch<float>("collider.collision.x");
      collision.iy = fetch<float>("collider.collision.y");
      collision.iw = fetch<float>("collider.collision.w");
      collision.ih = fetch<float>("collider.collision.h");
      collision.side = fetch<int>("collider.collision.side");
      collision.speedx = fetch<float>("collider.collision.speed.x");
      collision.speedy = fetch<float>("collider.collision.speed.y");
    }


    virtual void update(float dt) {
      if (!active) return;
      this->dt = dt;
      if (iskinematic || nogravity) return;
      else {
        xspeed += gx * dt;
        yspeed += gy * dt;
      }
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

