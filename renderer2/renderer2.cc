#include "gear2d.h"
#include "log.h"

#include "renderer2.h"
#include "renderbase.h"
#include "texture.h"

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
  renderbase::add(this);
}

renderer2::~renderer2() { 
}

std::string renderer2::family() { return "renderer"; }
std::string renderer2::type() { return "renderer2"; }

void renderer2::setup(object::signature & s) {
  modinfo("renderer2");
  sigparser sig(s, this);
  
  if (!renderbase::initialized) {
    int width; int height; bool fullscreen;
    width = eval<int>(s["renderer.w"], 640);
    height = eval<int>(s["renderer.h"], 480);
    fullscreen = eval<int>(s["renderer.fullscreen"], 0) == 1;
    renderbase::initialize(width, height, fullscreen);
  }
  
  surfacelist = sig.init("renderer.surfaces");
  size_t pos = string::npos;
  
  /* iterate through surface list */
  for (std::string surfdef : split(surfacelist, ' ')) {
    size_t pos = surfdef.find('=');
    
    // skip erroneous lines
    if (pos == string::npos) {
      trace("Skipping", surfdef, "as it is not a valid surface definition. Must be id=filename.",  log::error);
      continue;
    }
    
    /* get id=name */
    string id = surfdef.substr(0,  pos);
    string filename = surfdef.substr(pos+1);
    string p(id + "."); p.reserve(4);
    
    /* wire texture parameters with links */
    trace("Loading", filename, "as", id);
    textures[id] = renderbase::load(id, filename);
    texture & t = textures[id];
    p[3] = 'x'; t.x = sig.init(p, .0f);
    p[3] = 'y'; t.y = sig.init(p, .0f);
    p[3] = 'z'; t.z = sig.init(p, .0f);
    p[3] = 'w'; t.w = sig.init(p, 0);
    p[3] = 'h'; t.h = sig.init(p, 0);
    
  }
}

void renderer2::update(timediff dt) {
  renderbase::update((float)dt);
}

g2dcomponent(renderer2)
