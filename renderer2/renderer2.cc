#include "gear2d.h"
#include "log.h"

#include "renderer2.h"
#include "renderbase.h"
#include "texture.h"

#include <string>

using std::string;
using namespace gear2d;

renderer2::renderer2() {

}

void renderer2::destroyed() {
  modinfo("renderer2");
  for (auto textureitr : textures) {
    texture * tp = textureitr.second;
    int e = renderbase::renderorder.erase(zorder(tp->z, tp));
    trace("Removing", tp->id, "from renderer order. this =", this, "Result:", e);
    delete tp;
  }
  renderbase::remove(this);
}

renderer2::~renderer2() { 
}

std::string renderer2::family() { return "renderer"; }
std::string renderer2::type() { return "renderer2"; }
string renderer2::depends() { return "spatial/space2d"; }

void renderer2::surfaceschanged(string pid, component::base * lastwrite, object * owner) {
  // remember that surfacelist is a link, it is already updated
  modinfo("renderer2");
  trace("Surfaces changed for", owner->name(), "now they are", (std::string)surfacelist);
  loadsurfaces(surfacelist);
}

void renderer2::zchanged(string pid, component::base * lastwrite, object * owner) {
  modinfo("renderer2");
  string id = pid.substr(0, pid.find('.'));
  trace("zchanged for", id);
  texture * t = textures[id];
  
  // remove and re-insert in the z new order
  renderbase::renderorder.erase(zorder(t->oldz, t));
  renderbase::renderorder.insert(zorder(t->z, t));
  
  // update oldz value
  t->oldz = t->z;
}



void renderer2::setup(object::signature & s) {
  modinfo("renderer2");
  sig = sigparser(s, this);
  int screenwidth; int screenheight;
  screenwidth = sig.init("renderer.w", 0);
  screenheight = sig.init("renderer.h", 0);
  
  if (!renderbase::initialized) {
    bool fullscreen;
    fullscreen = eval<bool>(s["renderer.fullscreen"], false) == true;
    renderbase::initialize(screenwidth, screenheight, fullscreen, s["imgpath"]);
  }
  
  // write corrected w/h
  write("renderer.w", renderbase::screenwidth);
  write("renderer.h", renderbase::screenheight);

  renderbase::add(this);
  
  textlist = sig.init("renderer.texts");
  surfacelist = sig.init("renderer.surfaces");
  
  hook("renderer.surfaces", (component::call)&renderer2::surfaceschanged);
  
  loadsurfaces(surfacelist);
}

void renderer2::loadsurfaces(const string & surfacelist) {
  /* iterate through surface list,  initializing each one of them */
  modinfo("renderer2");
  size_t pos = string::npos;
  for (std::string surfdef : split<std::list<std::string>>(surfacelist, ' ')) {
    pos = surfdef.find('=');
    
    // skip erroneous lines
    if (pos == string::npos) {
      trace("Skipping", surfdef, "as it is not a valid surface definition. Must be id=filename.");
      continue;
    }
    
    /* get id=name */
    string id = surfdef.substr(0,  pos);
    string filename = surfdef.substr(pos+1);
    trim(filename);
    size_t i = id.size() + 1;
    string p(id + ". ");
    
    trace("Loading", filename, "as", id);
    SDL_Texture * raw = renderbase::load(filename);
    
    texture * tp = nullptr;
    
    /* check if we have that loaded already */
    auto it = textures.find(id);
    if (it != textures.end()) {
      if (it->second->raw == raw) {
        trace("Skipping texture", id, filename, "because its already loaded!");
        continue;
      } else {
        trace("Switching texture id", id, "to", filename);
        tp = it->second;
      }
    }
    else {
      tp = new texture();
      textures[id] = tp;
    }
    
    texture & t = *tp;
    t.id = id;
    p[i] = 'x'; t.x = sig.init(p, .0f);
    p[i] = 'y'; t.y = sig.init(p, .0f);
    p[i] = 'z'; t.z = sig.init(p, .0f);
    t.oldz = t.z;
    
    /* hook texture z */
    hook(p, (component::call)&renderer2::zchanged);
    
    /* user may explicitly define to stretch w and h */
    p[i] = 'w'; t.w = sig.init(p, sig[p] == "renderer.w" ? renderbase::screenwidth : renderbase::querywidth(raw));
    if (sig[p].empty() || sig[p] == "clip.w") t.bindclipw = true;
    p[i] = 'h'; t.h = sig.init(p, sig[p] == "renderer.h" ? renderbase::screenheight : renderbase::queryheight(raw));
    if (sig[p].empty() || sig[p] == "clip.h") t.bindcliph = true;
    t.bind = sig.init(id + ".bind", 1);
    t.objx = sig.init("x", .0f);
    t.objy = sig.init("y", .0f);
    t.objz = sig.init("z", .0f);
    t.rotation = sig.init(id + ".rotation", .0f);
    t.raw = raw;
    t.alpha = sig.init(id + ".alpha", 1.0f);
    t.clip.x = sig.init(id + ".clip.x", 0);
    t.clip.y = sig.init(id + ".clip.y", 0);
    t.clip.w = sig.init(id + ".clip.w", renderbase::querywidth(raw));
    t.clip.h = sig.init(id + ".clip.h", renderbase::queryheight(raw));
    t.render = sig.init(id + ".render", true);
    
    /* hook object z */
    hook("z", (component::call)&renderer2::zchanged);

    renderbase::renderorder.insert(zorder(t.z, tp));
  }
}

void renderer2::update(timediff dt) {
  auto total = renderbase::update();
}

g2dcomponent(renderer2, renderer, renderer2)
