/**
 * @file kinematic2d.cc
 * @namespace kinematic
 * @b family kinematic
 * @b type kinematic2d
 * @author Leonardo Guilherme de Freitas
 * 
 * This components apply acceleration, speed, rotational speed and
 * such to any spatial component (this is a 2d basic kinetic simulation)
 *
 * @b depends
 * @li spatial
 * 
 * @b using
 * @li @c x: x position where the result of transformation will be stored (from spatial) @b float
 * @li @c y: y position where the result of transformation will be stored (from spatial) @b float
 * 
 * @b provides
 * @li @c x.accel Acceleration for the x axis @b float
 * @li @c x.accel.{min|max} Minimum and maximum for acceleration on x axis @b float
 * @li @c y.accel Acceleration for the y axis @b float
 * @li @c y.accel.{min|max} Minimum and maximum for acceleration on y axis @b float
 * 
 * @li @c x.speed Speed for the x axis @b float
 * @li @c x.speed.{min|max} Minimum and maximum for speed on x axis @b float
 * @li @c y.speed Speed for the y axis @b float
 * @li @c y.speed.{min|max} Minimum and maximum for speed on y axis @b float
 * 
 * @warning Minimum and maximum (speed and accel) works like limits for the positive
 * and neegative values. If you want your object to have maximum speed at 100pixels/s 
 * and minimum -150pixels/s, you need to set them both. Maximum defaults do infinity and minimum
 * defaults to minus maximum. That is, if you set maximum to be 100p/s, minimum will be
 * -100p/s by default. If you do not set any, maximum will be infinity, minimum will be
 * minus infinity.
 * 
 */

#include "gear2d.h"
using namespace gear2d;

#include <iostream>
#include <limits>

class sigparser {
  private:
    object::signature & sig;
    component::base & com;

  public:
    sigparser(object::signature & sig, component::base * com)
      : sig(sig)
      , com(*com)
    { }

    template <typename datatype>
    link<datatype> init(std::string pid, const datatype & def = datatype()) {
      return com.fetch<datatype>(pid, eval<datatype>(sig[pid], def));
    }

    link<std::string> init(std::string pid, std::string def = std::string("")) {
      auto it = sig.find(pid);
      if (it != sig.end()) def = it->second;
      return com.fetch<std::string>(pid, def);
    }
};

class kinetic2d : public component::base {
  private:
    template<typename T>
    struct linkvec {
      link<T> x;
      link<T> y;
    };
    
    linkvec<float> speed;
    linkvec<float> speedmin;
    linkvec<float> speedmax;
    linkvec<float> accel;
    linkvec<float> accelmin;
    linkvec<float> accelmax;
    linkvec<float> pos;
    
  public:
    kinetic2d() {
    };
    
    virtual ~kinetic2d() { }
    virtual component::type type() { return "kinematic2d"; }
    virtual component::family family() { return "kinematics"; }
    virtual std::string depends() { return "spatial/space2d"; }
    
    virtual void setup(object::signature & sig) {
      sigparser s(sig, this);
      speed.x = s.init("x.speed", 0.0f);
      speedmax.x = s.init("x.speed.max", std::numeric_limits<float>::infinity());
      speedmin.x = s.init("x.speed.min", speedmax.x * -1.0f);
      speed.y = s.init("y.speed", 0.0f);
      speedmax.y = s.init("y.speed.max", std::numeric_limits<float>::infinity());
      speedmin.y = s.init("y.speed.min", speedmax.y * -1.0f);

      accel.x = s.init("x.accel", 0.0f);
      accelmax.x = s.init("x.accel.max", std::numeric_limits<float>::infinity());
      accelmin.x = s.init("x.accel.min", accelmax.x * -1.0f);
      accel.y = s.init("y.accel", 0.0f);
      accelmax.y = s.init("y.accel.max", std::numeric_limits<float>::infinity());
      accelmin.y = s.init("y.accel.min", accelmax.y * -1.0f);
      
      pos.x = fetch<float>("x");
      pos.y = fetch<float>("y");
    }
    
    virtual void update(timediff dt) {
      modinfo("kinematic2d");
      clamp(accel.x, accelmin.x, accelmax.x);
      clamp(accel.y, accelmin.y, accelmax.y);

      pos.x += speed.x * dt;
      pos.y += speed.y * dt;

      /* update speed */
      speed.x += accel.x *dt;
      speed.y += accel.y *dt;
      clamp(speed.x, speedmin.x, speedmax.x);
      clamp(speed.y, speedmin.y, speedmax.y);
      trace(speed.x, accel.x, speed.y, accel.y, owner->name());
    }
};

g2dcomponent(kinetic2d, kinematics, kinematic2d)
