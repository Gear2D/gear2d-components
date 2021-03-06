/**
 * @file collider2d.cc
 * @b family collider
 * @b type collider
 * @author Leonardo Guilherme de Freitas
 * @namespace collider
 * 
 * Calculates collision between all colliders, informing side.
 * <b>provides/acts upon</b>
 * @li @c collider.usez Use z in collision calculations (objects with different z's won't collide). Defaults to 0 @b bool (0 or 1)
 * @li @c collider.bind Moves along with spatial's x and y. Defaults to 1 @b bool
 * @li @c collider.tag Tag of this collider @b string
 * @li @c collider.avoid List of colliding tags names to ignore collision, separated by white-space @b string
 * @li @c collider.aabb.{x|y|w|h} Axis aligned bounding box for this object. Defaults to spatial's w,h. x and y defaults to 0. @b float
 * @li @c collider.collision Set whenever this component collides with another.
 *   May be set multiple times on a frame. You want to hook on this one.
 * <b>collider casted as gear2d::component::base*</b>
 * @li @c collider.collision.side Side where the collision occurs more severely. Integer from 0 to 3, starting at left, clockwise. @b int
 * @verbatim 0: left, 1: up, 2: right, 3: down, -1: not collision. @endverbatim
 * @li @c collider.collision.speed.{x|y} Speed of the object that collided with yours. THE OTHER OBJECT, NOT YOURS. DUMBASS.
 * @li @c collider.collision.{x|y|w|h} Interception rectangle for the two bounding boxes
 */
#include <cmath>
#include "component.h"
using namespace gear2d;

#include <string>
using namespace std;

class collider : public component::base {
  private:
    struct linkrect {
      gear2d::link<float> x;
      gear2d::link<float> y;
      gear2d::link<float> w;
      gear2d::link<float> h;
    };

    /*
      write<component::base *>("collider.collision", 0);
      write("collider.collision.side", -1);

      write<float>("collider.collision.speed.x", 0);
      write<float>("collider.collision.speed.y", 0);

      write<float>("collider.collision.x", 0);
      write<float>("collider.collision.y", 0);
      write<float>("collider.collision.w", 0);
      write<float>("collider.collision.h", 0);
      */
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

    struct rect {
      float x, y, w, h;
    };

    collisioninfo collision;
    linkrect aabb;

    /* spatial stuff. we store z and d because
     * we may want to consider them for collisions */
    struct {
      gear2d::link<float> x;
      gear2d::link<float> y;
      gear2d::link<float> z;
      gear2d::link<float> w;
      gear2d::link<float> h;
      gear2d::link<float> d;
    } spatial;

    struct {
      gear2d::link<float> speedx;
      gear2d::link<float> speedy;
    } kinematics;

    
    gear2d::link<string> tag;
    gear2d::link<string> ignore;
    gear2d::link<bool> binded;
    gear2d::link<std::string> coltype;
    gear2d::link<bool> active;
    
  public:
    virtual ~collider() { colliders.erase(this); activecolliders.erase(this); }
    virtual component::type type() { return "collider2d"; }
    virtual component::family family() { return "collider"; }
    virtual string depends() { return "spatial/space2d kinematics/kinematic2d"; }
    virtual void activechanged(parameterbase::id pid, base* lastwrite, object::id owner) {
      // maintain a list of active colliders.
      if (!active) {
        if (!calculating) {
          activecolliders.erase(this);
        } else {
          toremove.insert(this);
        }
      } else {
        auto wasinserted = activecolliders.insert(this);
        if (!wasinserted.second)
          toremove.erase(this);
      }
    }
    
    virtual void setup(object::signature & sig) {
      sigparser s(sig, this);

      coltype = s.init("collider.type", string("aabb"));
      binded = s.init("collider.bind", true);
      active = s.init("collider.active", true);
      hook("collider.active", (component::call) &collider::activechanged);

      float x, y, w, h;
      read("x", x); read("y", y), read("w", w), read("h", h);
      hook("w"); hook("h");

      aabb.x = s.init("collider.aabb.x", 0.0f);
      aabb.y = s.init("collider.aabb.y", 0.0f);
      aabb.w = s.init("collider.aabb.w", w);
      aabb.h = s.init("collider.aabb.h", h);

      spatial.x = fetch<float>("x");
      spatial.y = fetch<float>("y");
      spatial.z = fetch<float>("z");
      spatial.w = fetch<float>("w");
      spatial.h = fetch<float>("h");
      spatial.d = fetch<float>("d");

      kinematics.speedx = fetch<float>("x.speed");
      kinematics.speedy = fetch<float>("y.speed");

      collision.collisor = fetch<component::base *>("collider.collision");
      //write<component::base *>("collider.collision", 0);

      collision.side = s.init("collider.collision.side", -1);
//      write("collider.collision.side", -1);

      collision.speedx = s.init("collider.collision.speed.x", 0.0f);
      collision.speedy = s.init("collider.collision.speed.y", 0.0f);
      //write<float>("collider.collision.speed.x", 0);
      //write<float>("collider.collision.speed.y", 0);

      collision.ix = s.init("collider.collision.x", .0f);
      collision.iy = s.init("collider.collision.y", .0f);
      collision.iw = s.init("collider.collision.w", .0f);
      collision.ih = s.init("collider.collision.h", .0f);
      //write<float>("collider.collision.x", 0);
      //write<float>("collider.collision.y", 0);
      //write<float>("collider.collision.w", 0);
      //write<float>("collider.collision.h", 0);

      tag = s.init<std::string>("collider.tag");
      //bind("collider.tag", tag);
      ignore = s.init<std::string>("collider.ignore");
      //bind("collider.ignore", ignore);
      //ignore = sig["collider.ignore"];
      //tag = sig["collider.tag"];
      if ((string)tag == "") tag = sig["name"];
      
      colliders.insert(this);
      if (active)
        activecolliders.insert(this);
    }
    
    virtual void update(float dt) {
      if (++voteforupdate >= colliders.size()) {
        calculate();
        voteforupdate = 0;
      }
    }
  
  private:
    
    
  private:
    
    static void initialize() {
    }
    
    static void calculate() {
      if (activecolliders.size() <= 1) return;
      calculating = true;
      for (std::set<collider *>::iterator i = activecolliders.begin(); i != activecolliders.end(); i++) {
        
        collider * first = *i;
        if (first->active == false) continue;
        // tests from this to the rest
        std::set<collider *>::iterator j = i;
        j++;
        for (; j != activecolliders.end(); j++) {
          collider * second = *j;
          //const string & ftype = first->raw<string>("collider.type");
          //const string & stype = second->raw<string>("collider.type");
          
          if (second->active == false) continue;
            
          // ignopr
          if ((((string)second->ignore).find(first->tag) != string::npos) && (((string)first->ignore).find(second->tag) != string::npos)) {
            continue;
          }

          const bool & fbind = first->binded;
          //const bool & fbind = first->raw<bool>("collider.bind");
          const bool & sbind = second->binded;
          //const bool & sbind = second->raw<bool>("collider.bind");

          rect faabb = { first->aabb.x, first->aabb.y, first->aabb.w, first->aabb.h };
          rect saabb = { second->aabb.x, second->aabb.y, second->aabb.w, second->aabb.h };
          
          if (fbind) {
            faabb.x += first->spatial.x;
            faabb.y += first->spatial.y;
            // todo: z
          }
          
          if (sbind) {
            saabb.x += second->spatial.x;
            saabb.y += second->spatial.y;
          }
          
          if (testaabb(faabb, saabb)) {
            
            // calculates intersection
            rect inter;
            inter.x = max(faabb.x, saabb.x);
            inter.y = max(faabb.y, saabb.y);
            inter.w = min(faabb.x + faabb.w, saabb.x + saabb.w) - max(faabb.x, saabb.x);
            inter.h = min(faabb.y + faabb.h, saabb.y + saabb.h) - max(faabb.y, saabb.y);
            
            
            
            int fc, sc;
            
            // when h bigger than w, side collision first
            if (inter.h > inter.w) {
              if (inter.x > faabb.x) { fc = 2; sc = 0; }
              else { fc = 0; sc = 2; }
            } else {
              if (inter.y > faabb.y) { fc = 1; sc = 3; }
              else { fc = 3; sc = 1; }
            }
            
            if (((string)first->ignore).find(second->tag) == string::npos) {
              first->collision.ix = inter.x;
              first->collision.iy = inter.y;
              first->collision.iw = inter.w;
              first->collision.ih = inter.h;
              //first->write<float>("collider.collision.x", inter.x);
              //first->write<float>("collider.collision.y", inter.y);
              //first->write<float>("collider.collision.w", inter.w);
              //first->write<float>("collider.collision.h", inter.h);

              first->collision.side = fc;
              //first->write<int>("collider.collision.side", fc);

              first->collision.speedx = second->kinematics.speedx;
              first->collision.speedy = second->kinematics.speedy;
              //first->write<float>("collider.collision.speed.x", second->read<float>("x.speed"));
              //first->write<float>("collider.collision.speed.y", second->read<float>("y.speed"));
            }
            
            if (((string)second->ignore).find(first->tag) == string::npos) {
              second->collision.ix = inter.x;
              second->collision.iy = inter.y;
              second->collision.iw = inter.w;
              second->collision.ih = inter.h;
              //second->write<float>("collider.collision.x", inter.x);
              //second->write<float>("collider.collision.y", inter.y);
              //second->write<float>("collider.collision.w", inter.w);
              //second->write<float>("collider.collision.h", inter.h);

              second->collision.side = sc;
              //second->write<int>("collider.collision.side", sc);

              second->collision.speedx = first->kinematics.speedx;
              second->collision.speedy = first->kinematics.speedy;
              //second->write<float>("collider.collision.speed.x", first->read<float>("x.speed"));
              //second->write<float>("collider.collision.speed.y", first->read<float>("y.speed"));
            }
            
            if (((string)first->ignore).find(second->tag) == string::npos) {
              first->collision.collisor = second;
            }
              
            if (((string)second->ignore).find(first->tag) == string::npos) {
              second->collision.collisor = first;
            }
          }
        }
      }
      for (collider * c : toremove) {
        if (!c->active)
          activecolliders.erase(c);
      }
      toremove.clear();
      calculating = false;
    }
    static bool testaabb(rect & a, rect & b) {
      return (
        (b.x < a.x + a.w) &&
        (b.x + b.w > a.x) &&
        (b.y < a.y + a.h) &&
        (b.y + b.h > a.y) //
      );
    }

  private:
    static int voteforupdate;
    static std::set<collider *> colliders;
    static std::set<collider *> activecolliders;
    static std::set<collider *> toremove;
    static bool calculating;
};

int collider::voteforupdate = 0;
std::set<collider*> collider::colliders;
std::set<collider*> collider::activecolliders;
std::set<collider*> collider::toremove;
bool collider::calculating;

g2dcomponent(collider, collider, collider2d);
