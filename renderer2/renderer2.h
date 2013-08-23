#ifndef renderer2_h
#define renderer2_h

#include "gear2d.h"
#include <string>

using namespace gear2d;

class renderer2 : public component::base {
  public:
	  renderer2();
	  virtual std::string family();
	  virtual std::string type();
	  virtual void setup(const map<std::string, std::string> & sig);
	  virtual void update(timediff dt);
	  virtual ~renderer2();
};

#endif