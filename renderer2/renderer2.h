#ifndef renderer2_h
#define renderer2_h

#include "gear2d.h"
#include <string>

using namespace gear2d;
using std::string;

class renderer2 : public component::base {
  public:
    renderer2();
    virtual std::string family();
    virtual std::string type();
    virtual void setup(object::signature & sig);
    virtual void update(timediff dt);
    virtual ~renderer2();
    
  private:
    link<string> surfacelist;
};

#endif
