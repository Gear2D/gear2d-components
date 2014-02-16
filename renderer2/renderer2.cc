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

void renderer2::textstylechanged(string pid, component::base * lastwrite, object * owner) {
  string id = pid.substr(0, pid.find('.'));
  text * textp = texts[id];
  if (textp == nullptr) { return; } /* lolwut? */
  text & t =  *textp;
  modinfo("renderer2");
  trace("Style changed for text", id + '.', "Will recalculate style flags.");
  
  
  string & style = ((string&)t.style);
  t.bold = style.find("bold") != string::npos;
  t.italic = style.find("italic") != string::npos;
  t.underline = style.find("underline") != string::npos;
  t.strikethrough = style.find("strikethrough") != string::npos;
  t.dirty = true;
  SDL_Texture * raw, * oldraw;
  raw = renderbase::fromtext(t);
  oldraw = t.rendered->raw;
  wiretexture(id, raw);

  if (oldraw != nullptr) {
    SDL_DestroyTexture(oldraw);
  }
}


void renderer2::textchanged(string pid, component::base * lastwrite, object * owner) {
  string id = pid.substr(0, pid.find('.'));
  text * textp = texts[id];
  if (textp == nullptr) { return; } /* lolwut? */
  text & t =  *textp;
  modinfo("renderer2");
  trace("Text changed for text", id + '.');

  t.dirty = true;
  SDL_Texture * raw, * oldraw;
  raw = renderbase::fromtext(t);
  oldraw = t.rendered->raw;
  wiretexture(id, raw);

  if (oldraw != nullptr) {
    SDL_DestroyTexture(oldraw);
  }
}


void renderer2::zchanged(string pid, component::base * lastwrite, object * owner) {
  modinfo("renderer2");
  string id = pid.substr(0, pid.find('.'));
  trace("z changed for", id);
  texture * t = textures[id];
  
  // remove and re-insert in the z new order
  renderbase::renderorder.erase(zorder(t->oldz, t));
  renderbase::renderorder.insert(zorder(t->z, t));
  
  // update oldz value
  t->oldz = t->z;
}

void renderer2::camerachanged(string pid, component::base * lastwrite, object * owner) {
  renderbase::updatecamera(camera.x, camera.y, camera.w, camera.h);
}




void renderer2::setup(object::signature & s) {
  modinfo("renderer2");
  sig = sigparser(s, this);
  globalsig = sigparser(s, &globals);
  int screenwidth; int screenheight;
  
  /* initialize screen things */
  screenwidth = globalsig.init("renderer.w", 0);
  screenheight = globalsig.init("renderer.h", 0);
  
  /* initialize camera things */
  camera.w = globalsig.init("camera.w", (float)screenwidth);
  camera.h = globalsig.init("camera.h", (float)screenheight);
  camera.x = globalsig.init<float>("camera.x", 0.0f);
  camera.y = globalsig.init<float>("camera.y", 0.0f);
  
  /* hook on camera changing */
  hook(&globals, "camera.x", (component::call)&renderer2::camerachanged);
  hook(&globals, "camera.y", (component::call)&renderer2::camerachanged);
  hook(&globals, "camera.w", (component::call)&renderer2::camerachanged);
  hook(&globals, "camera.h", (component::call)&renderer2::camerachanged);
  
  if (!renderbase::initialized) {
    bool fullscreen;
    fullscreen = eval<bool>(s["renderer.fullscreen"], false) == true;
    renderbase::initialize(screenwidth, screenheight, fullscreen, s["imgpath"]);
    renderbase::updatecamera(camera.x, camera.y, camera.w, camera.h);
  }
  
  // write corrected w/h
  write("renderer.w", renderbase::screenwidth);
  write("renderer.h", renderbase::screenheight);
  
  renderbase::add(this);
  
  textlist = sig.init("renderer.texts");
  surfacelist = sig.init("renderer.surfaces");
  
  hook("renderer.surfaces", (component::call)&renderer2::surfaceschanged);
  
  loadsurfaces(surfacelist);
  loadtexts(textlist);
}

void renderer2::loadsurfaces(const string & surfacelist) {
  if (surfacelist.empty()) return;
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
    string fname = surfdef.substr(pos+1);
    trim(fname);
    size_t i = id.size() + 1;
    string p(id + ". ");
    
    trace("Loading", fname, "as", id);
    SDL_Texture * raw = renderbase::load(fname);
    if (raw == nullptr) {
      continue;
    }
    
    wiretexture(id, raw);
  }
}

texture * renderer2::wiretexture(const string & id, SDL_Texture * raw) {
  modinfo("renderer2");
  size_t i = id.size() + 1;
  string p(id + ". ");
  bool rewiring = false;

  texture * tp = nullptr;

  /* check if we have that loaded already */
  auto it = textures.find(id);
  if(it != textures.end()) {
    if(it->second->raw == raw) {
      trace("Skipping texture", id, "because its already loaded!");
      return it->second;
    } else {
      trace("Switching texture id", id, "to a new texture");
      tp = it->second;
      rewiring = true;
    }
  } else {
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
  if (!rewiring) {
    /* only re-read sig if rewiring stuff this is a new stuff */
    p[i] = 'w'; 
    t.w = sig.init(p, sig[p] == "renderer.w" ? renderbase::screenwidth : renderbase::querywidth(raw));
    if (sig[p].empty() || sig[p] == "clip.w") t.bindclipw = true;
    p[i] = 'h'; t.h = sig.init(p, sig[p] == "renderer.h" ? renderbase::screenheight : renderbase::queryheight(raw));
    if (sig[p].empty() || sig[p] == "clip.h") t.bindcliph = true;
    t.bind = sig.init(id + ".bind", 1);
    t.objx = sig.init("x", .0f);
    t.objy = sig.init("y", .0f);
    t.objz = sig.init("z", .0f);
    t.rotation = sig.init(id + ".rotation", .0f);
    t.alpha = sig.init(id + ".alpha", 1.0f);
    t.clip.x = sig.init(id + ".clip.x", 0);
    t.clip.y = sig.init(id + ".clip.y", 0);
    t.clip.w = sig.init(id + ".clip.w", (int)t.w);
    t.clip.h = sig.init(id + ".clip.h", (int)t.h);
    t.render = sig.init(id + ".render", true);
  } else {
    bool resetclipw = false, resetcliph = false;
    if (t.clip.w == t.w) resetclipw = true;
    if (t.clip.h == t.h) resetcliph = true;
    p[i] = 'w';
    t.w = sig[p] == "renderer.w" ? renderbase::screenwidth : renderbase::querywidth(raw);
    p[i] = 'h';
    t.h = sig[p] == "renderer.h" ? renderbase::screenheight : renderbase::queryheight(raw);
    if (resetclipw) t.clip.w = t.w;
    if (resetcliph) t.clip.h = t.h;
  }
  t.raw = raw;

  /* hook object z */
  hook("z", (component::call)&renderer2::zchanged);
  renderbase::renderorder.insert(zorder(tp->z, tp));
  return tp;
}


void renderer2::loadtexts(const string & textlist) {
  if (textlist.empty()) return;
  modinfo("renderer2");
  text * textp = nullptr;
  SDL_Texture * raw;
  for (string id : gear2d::split(textlist, ' ')) {
    trace("Generating text id", id);
    if (texts.find(id) != texts.end())
    {
      trace("Not loading", id, "because it was already generated");
      continue;
    }
    
    string p(id + ". ");
    textp = new text();
    texts[id] = textp;
    text & t = *textp;
    t.text = sig.init(id + ".text", string(""));
    t.font.face = sig.init(id + ".font.face", string("freesans.ttf"));
    t.font.size = sig.init(id + ".font.size", 12);
    t.style = sig.init(id + ".style", string(""));
    t.blended = sig.init(id + ".blended", true);
    string style = (string)t.style;
    t.bold = style.find("bold") != string::npos;
    t.italic = style.find("italic") != string::npos;
    t.underline = style.find("underline") != string::npos;
    t.strikethrough = style.find("strikethrough") != string::npos;
    t.color.r = sig.init(id + ".color.r", 0.0f);
    t.color.g = sig.init(id + ".color.g", 0.0f);
    t.color.b = sig.init(id + ".color.b", 0.0f);
    hook(id + ".style", (component::call) &renderer2::textstylechanged);
    hook(id + ".text", (component::call) &renderer2::textchanged);
    
    t.dirty = true;
    raw = renderbase::fromtext(t); 
    t.rendered = wiretexture(id, raw);
  }
}


void renderer2::update(timediff dt) {
  auto total = renderbase::update();
}

g2dcomponent(renderer2, renderer, renderer2)
