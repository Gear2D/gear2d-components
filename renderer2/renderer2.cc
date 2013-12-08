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

void renderer2::setup(object::signature & s) {
  modinfo("renderer2");
  sigparser sig(s, this);
  int screenwidth; int screenheight;
  screenwidth = sig.init("renderer.w", 640);
  screenheight = sig.init("renderer.h", 480);
  
  if (!renderbase::initialized) {
    bool fullscreen;
    fullscreen = eval<int>(s["renderer.fullscreen"], 0) == 1;
    renderbase::initialize(screenwidth, screenheight, fullscreen, s["imgpath"]);
  }

  renderbase::add(this);
  
  textlist = sig.init("renderer.texts");
  surfacelist = sig.init("renderer.surfaces");
  size_t pos = string::npos;
  
  /* iterate through surface list */
  /* iterate through surface list,  initializing each one of them */
  for (std::string surfdef : split<std::list<std::string>>(surfacelist, ' ')) {
    pos = surfdef.find('=');
    
    // skip erroneous lines
    if (pos == string::npos) {
      trace("Skipping", surfdef, "as it is not a valid surface definition. Must be id=filename.",  log::error);
      continue;
    }
    
    /* get id=name */
    string id = surfdef.substr(0,  pos);
    string filename = surfdef.substr(pos+1);
    size_t i = id.size() + 1;
    string p(id + ". ");
    
    /* wire texture parameters with links */
    trace("Loading", filename, "as", id);
    SDL_Texture * raw = renderbase::load(filename);
    texture * tp = new texture();
    textures[id] = tp;
    texture & t = *tp;
    t.id = id;
    p[i] = 'x'; t.x = sig.init(p, .0f);
    p[i] = 'y'; t.y = sig.init(p, .0f);
    p[i] = 'z'; t.z = sig.init(p, .0f);
    
    /* user may explicitly define to stretch w and h */
    p[i] = 'w'; t.w = sig.init(p, s[p] == "renderer.w" ? renderbase::screenwidth : renderbase::querywidth(raw));
    if (s[p].empty() || s[p] == "clip.w") t.bindclipw = true;
    p[i] = 'h'; t.h = sig.init(p, s[p] == "renderer.h" ? renderbase::screenheight : renderbase::queryheight(raw));
    if (s[p].empty() || s[p] == "clip.h") t.bindcliph = true;
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

    renderbase::renderorder.insert(zorder(t.z, tp));
  }
}

void renderer2::update(timediff dt) {
  auto total = renderbase::update();
}

g2dcomponent(renderer2, renderer, renderer2)
