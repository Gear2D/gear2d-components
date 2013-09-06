#include "renderer2.h"
#include "renderbase.h"
#include "gear2d.h"
#include "log.h"
#include <string>

using namespace gear2d;

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
      com.init(pid, sig[pid], def);
      return com.fetch<datatype>(pid);
    }

    link<std::string> init(std::string pid, std::string def = std::string("")) {
      com.write(pid, sig[pid]);
      return com.fetch<std::string>(pid);
    }
};

renderer2::renderer2() { 
  renderbase::
}

renderer2::~renderer2() { 
}

std::string renderer2::family() { return "renderer"; }
std::string renderer2::type() { return "renderer2"; }

void renderer2::setup(object::signature & s) {
  sigparser sig(s, this);
  surfaces = sig.init("renderer.surfaces");
  moderr("renderer2");
  size_t pos = string::npos;
  
  for (std::string surfdef : split(surfaces, ' ')) {
    size_t pos = surfdef.find('=');
    
    // skip erroneous lines
    if (pos == string::npos) {
      trace("Skippint", surfdef, "as it is not a valid surface definition. Must be id=filename.");
      continue;
    }
    
    string id = surfdef.substr(0,  pos);
    string filename = surfdef.substr(pos+1);
    
    trace("Loading", filename, "as", id, log::info);
  }
}

void renderer2::update(timediff dt) {
}

g2dcomponent(renderer2)
