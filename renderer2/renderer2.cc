#include "renderer2.h"
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
      com.init(pid, sig[pid], def);
      return com.fetch<std::string>(pid);
    }
};

renderer2::renderer2() { }

renderer2::~renderer2() { }

std::string renderer2::family() { return "renderer"; }
std::string renderer2::type() { return "renderer2"; }

void renderer2::setup(object::signature & s) {
  sigparser sig(s, this);
  link<std::string> surfaces = sig.init("renderer.surfaces");


  for (std::string surfdef : split(surfaces, ' ')) {
    std::cout << "Output: " << surfdef << std::endl;
  }
}

void renderer2::update(timediff dt) {

}

g2dcomponent(renderer2)
