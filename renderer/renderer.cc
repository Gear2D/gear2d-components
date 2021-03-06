/**
 * @file renderer.cc
 * @author Leonardo Guilherme de Freitas
 *
 * @page renderer/renderer2d renderer/renderer2d
 * - @b Family: renderer
 * - @b Type: renderer2d
 *
 * Renderer component that can work with images,
 * meshes&textures, primitives, etc...
 *
 * @b parameters
 * @li <tt>renderer.w</tt>: Width of the render screen. Global.  Shall be set at the game config file. Defaults to 800. @b int
 * @li <tt>renderer.h</tt>: Height of the render screen. Global. Shall be set at the game config file. Defaults to 600. @b int
 * @li <tt>camera.{x|y|z}</tt>: Camera x, y and z position for rendering calculations. Global. Does nothing on object signature. Defaults to 0. @b float
 * @li <tt>renderer.surfaces</tt>: list of surfaces to load and work with, in the form of id=file separated by spaces. Loaded at setup and whenever changed.  @b string
 *     @warning double warning on this one. If you want to load a new surface, change renderer.surfaces BEFORE setting its other attributes or the
 *    binding will fail so bad you won't be able to hide. Be prepared for evil if you do.
 * @li <tt>\<id\>.bind</tt>: Tells if the position of this image shall follow spatial info (x, y). @b bool. Defaults to 1 (true)
 * @li <tt>\<id\>.position.absolute</tt>: Tells if camera should be taken in account when rendering this. @b bool. 1 (true)
 * @li <tt>\<id\>.position.{x|y|z}</tt>: Specifics about the position of the image. If position.bind is specified, it will become an offset. @b float
 * @li <tt>\<id\>.position.{w|h}</tt>: Width and height of the image. Note that this does not scale the image, see "scale" below. @b int
 * @li <tt>\<id\>.clip.{x|y|w|h}</tt>: Clipping frame for the image. Defaults to whole image. @b float
 * @li <tt>\<id\>.alpha</tt>: Alpha value for the surface. 0 is transparent, 1 is opaque. @b float
 * @li <tt>\<id\>.render</tt>: Sets if this surface shall be rendered. 0 is false (don't render) and 1 is true (render). Defaults to true.
 * @li <tt>\<id\>.zoom</tt>: Scale this image zoom by a factor. @b float. Natural scale is 1.0. Less than 1 will shrink, bigger than 1 will enlarge. Defaults to 1.0.
 * @li <tt>renderer.texts</tt>: list of ids to be treated like text
 * @li <tt>\<id\>.text</tt>: Text to be rendered. This only works if @c <id> was declared at renderer.texts
 * @li <tt>\<id\>.blended</tt>: Shall the text be rendered in blend mode (alpha blending)? Defaults to 0 @b bool. Do not use for text that changes much.
 * @li <tt>\<id\>.font</tt>: Font-file to be used. @b string Defaults to arial.ttf
 * @li <tt>\<id\>.font.size</tt>: Size to be rendered, in pixels
 * @li <tt>\<id\>.font.{r|g|b}</tt>: Red/green/blue color to be displayed, ranging from 0 to 1. @b float Defaults to 0
 * @li <tt>\<id\>.font.style</tt>: List of styles to be displayed, separated by white-space (bold italic underline strikethru) @b string
 *
 *
 * @b globals:
 * @li <tt>imgpath</tt> Path where to look for image files. Shall be set at the main config file
 *
 */

#include "gear2d.h"
using namespace gear2d;

#include <set>
#include <string>
using namespace std;

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_rotozoom.h"

// this handles surface parameters
struct surface {
  // id of this surface
  string id;
  component::base * parent;

  // parameters to be referenced
  float x;
  float y;
  float z;
  float rotation, oldrotation;
  float zoom, oldzoom;
  int w;
  int h;
  float oldz;
  float alpha;
  bool bind;
  bool absolute;
  bool render;
  SDL_Rect clip;

  // raw surface
  SDL_Surface * raw;

  // rotozoomed surface
  SDL_Surface * rotozoomed;

  /* if dirty, rotozoom shall take place */
  bool dirty;

  bool operator<(const surface & other) const {
    // only zordering really matters.
    return (z < other.z);
  }

  bool operator==(const surface & other) const {
    if(this == &other) return true;
    if(
      id == other.id &&
      z == other.z &&
      x == other.x &&
      y == other.y &&
      raw == other.raw
    ) { return true; }
    return false;
  }

  surface(component::base * parent, string id, SDL_Surface * raw)
    : parent(parent)
    , id(id)
    , x(0.0f)
    , y(0.0f)
    , z(0.0f)
    , rotation(0.0f)
    , oldrotation(rotation)
    , zoom(1.0f)
    , oldzoom(zoom)
    , w(0)
    , h(0)
    , oldz(z)
    , alpha(1.0f)
    , bind(true)
    , render(true)
    , absolute(false)
    , raw(raw)
    , rotozoomed(NULL)
    , dirty(false) {
    clip.x = clip.y = clip.w = clip.h = 0;
    if(raw) {
      clip.w = raw->w;
      clip.h = raw->h;
    }
  }

  virtual ~surface() { }
};


class renderer : public component::base {
  private:
    // this is the class for a text definition (how should it be rendere
    class textdef {
      public:
        string id;
        component::base * parent;
        string text;
        bool blended;
        string font;
        int fontsz;
        float r;
        float g;
        float b;
        string styles;

        textdef(const component::base * parent, string id)
          : id(id)
          , parent(0)
          , text("")
          , blended(0)
          , font("arial.ttf")
          , fontsz(10)
          , r(0)
          , g(0)
          , b(0)
          , styles("") {

        }

        SDL_Surface * render() {
          modinfo("renderer2d/textdef");
          trace("Rendering text", id);

          char sz[4];
          sprintf(sz, "%d", fontsz);
          TTF_Font *& f = renderer::fonts[font + '@' + sz];
          if(f == 0) {
            f = TTF_OpenFont(font.c_str(), fontsz);
          }
          if(f == 0) {
            trace("Cannot render text", id, "because I cannot find font", font, log::error);
            return 0;
          }

          // calculate styles
          int styleflag = 0;
          set<string> styleset;
          while(!styleset.empty()) {
            string style = *styleset.begin();
            if(style == "bold") styleflag |= TTF_STYLE_BOLD;
            if(style == "italic") styleflag |= TTF_STYLE_ITALIC;
            if(style == "underline") styleflag |= TTF_STYLE_UNDERLINE;
            //if (style == "strikethru") styleflag |= TTF_STYLE_STRIKETHROUGH;
            styleset.erase(style);
          }

          TTF_SetFontStyle(f, styleflag);
          SDL_Surface * textsurface = 0;
          SDL_Color fg;
          fg.r = r * 255;
          fg.g = g * 255;
          fg.b = b * 255;
          if(!blended) textsurface = TTF_RenderText_Solid(f, text.c_str(), fg);
          else textsurface = TTF_RenderText_Blended(f, text.c_str(), fg);
          return textsurface;
        }
    };

  private:
    typedef pair<float, surface *> zorder;

    static map<string, SDL_Surface *> rawbyfile;
    static map<string, TTF_Font *> fonts;

    static SDL_Surface * screen;
    static bool initialized;
    static set<zorder> zordered;
    static int rendervotes;

    static list<renderer *> renderers;

  private:
    map<string, surface *> surfacebyid;
    map<string, textdef *> textdefbyid;
    string surfaces;
    string texts;

  public:
    renderer() {
      renderers.push_back(this);
    }
    virtual ~renderer() {
      renderers.remove(this);
      for(map<string, surface *>::iterator i = surfacebyid.begin(); i != surfacebyid.end(); i++) {
        surface * s = i->second;
        // remove from rendering list
        zordered.erase(zorder(s->z, s));
      }
    }

    virtual component::type type() { return "renderer"; }
    virtual component::family family() { return "renderer"; }
    virtual string depends() { return "spatial/space2d"; }

    /* hooks */
    virtual void zhandler(parameterbase::id pid, component::base * lastwrite, object::id owner) {
      int p = pid.find(".position.z");
      string surfid = pid.substr(0, p);
      surface * s = surfacebyid[surfid];
      if(s == 0) return;
      // right, the very same parameter we're interested
      zordered.erase(zorder(s->oldz, s));
      s->oldz = s->z;
      zordered.insert(zorder(s->z, s));
    }

    virtual void zoomhandler(parameterbase::id pid, component::base * lastwrite, object::id owner) {
      int p = pid.find(".zoom");
      string surfid = pid.substr(0, p);
      surface * s = surfacebyid[surfid];
      if(s == 0) return;
      if(s->zoom != s->oldzoom) { s->dirty = true; s->oldzoom = s->zoom; }
    }

    virtual void rotationhandler(parameterbase::id pid, component::base * lastwrite, object::id owner) {
      modinfo(type());
      int p = pid.find(".position.rotation");
      string surfid = pid.substr(0, p);
      trace("Rotating", surfid);
      surface * s = surfacebyid[surfid];
      if(s == 0) return;
      if(s->rotation != s->oldrotation) { s->dirty = true; s->oldrotation = s->rotation; }
    }

    virtual void texthandler(parameterbase::id pid, component::base * lastwrite, object::id owner) {
      modinfo(type());
      int p = pid.find('.');
      string textid = pid.substr(0, p);

      trace("Text for", textid, "changed");

      // changing some text we own
      if(textdefbyid[textid] != 0) {
        surface *& s = surfacebyid[textid];
        textdef *& t = textdefbyid[textid];
        if(t == 0 || s == 0) return;
        SDL_FreeSurface(s->raw);
        s->raw = textdefbyid[textid]->render();
        s->w = s->raw->w;
        s->h = s->raw->h;
      }
    }

    virtual void handle(parameterbase::id pid, component::base * lastwrite, object::id owner) {
      int p;
      if(pid == "renderer.surfaces") {
        loadsurfaces(surfaces, read<string>("imgpath"));
      }

      /* TODO: Add support for new texts... */
      else if(pid == "renderer.texts") {
        //frak...
      }
    }
    virtual void setup(object::signature & sig) {
      int w = eval(sig["renderer.w"], 800);
      int h = eval(sig["renderer.h"], 600);
      if(!initialized) initialize(w, h);
      write("renderer.w", w);
      write("renderer.h", h);
      write("imgpath", sig["imgpath"]);

      // initial text rendering
      string textlist = sig["renderer.texts"];
      bind("renderer.texts", texts);
      hook("renderer.texts");

      // prepare these signatures first...
      /*
       * @li @c <id>.blended Shall the text be rendered in blend mode (alpha blending)? Defaults to 0 @b bool. Do not use for text that changes much.
       * @li @c <id>.font Font-file to be used. @b string Defaults to arial.ttf
       * @li @c <id>.font.size Size to be rendered, in pixels
       * @li @c <id>.font.{r|g|b} Red/green/blue color to be displayed, ranging from 0 to 1. @b float Defaults to 0
       * @li @c <id>.font.style List of styles to be displayed, separated by white-space (bold italic underline strikethru) @b string
       * */
      if(textlist != "") {
        set<string> texts;
        split(texts, textlist, ' ');
        string pid;
        while(!texts.empty()) {
          const string & id = *texts.begin();
          textdef * t = new textdef(this, id);
          loadtext(id, t, sig);
          textdefbyid[id] = t;
          prepare(id, t->render());
          texts.erase(id);
        }
      }

      string surfacelist = sig["renderer.surfaces"];
      bind("renderer.surfaces", surfaces);
      hook("renderer.surfaces");

      if(surfacelist != "") {
        loadsurfaces(surfacelist, sig["imgpath"]);
      }

      // initialize with signature;
      // we do not need to rerun this because if the user loads a surface
      // after setup phase, probably he'll load parameters by himself.
      for(map<string, surface *>::iterator i = surfacebyid.begin(); i != surfacebyid.end(); i++) {
        string id = i->first;
        surface * s = i->second;

        // pid was set by bind() of by surface() in worst case.
        write(id + ".position.x", eval(sig[id + ".position.x"], 0.0f));
        write(id + ".position.y", eval(sig[id + ".position.y"], 0.0f));
        write(id + ".position.z", eval(sig[id + ".position.z"], 0.0f));
        write(id + ".position.w", eval(sig[id + ".position.w"], s->raw->w));
        write(id + ".position.h", eval(sig[id + ".position.h"], s->raw->h));
        write(id + ".position.rotation", eval(sig[id + ".position.rotation"], 0.0f));
        write(id + ".zoom", eval(sig[id + ".zoom"], 1.0f));
        write(id + ".clip.x", eval<int>(sig[id + ".clip.x"], 0));
        write(id + ".clip.y", eval<int>(sig[id + ".clip.y"], 0));
        write(id + ".clip.w", eval<int>(sig[id + ".clip.w"], s->raw->w));
        write(id + ".clip.h", eval<int>(sig[id + ".clip.h"], s->raw->h));
        write(id + ".bind", eval(sig[id + ".bind"], true));
        write(id + ".absolute", eval(sig[id + ".absolute"], true));
        write(id + ".alpha", eval<float>(sig[id + ".alpha"], 1.0f));
        write(id + ".render", eval<bool>(sig[id + ".render"], true));

        zordered.insert(zorder(s->z, s));
      }
    }

    virtual void update(float dt) {
      rendervotes++;
      if(rendervotes >= renderers.size()) {
        render();
        rendervotes = 0;
      }
    }
  private:

    /* load fonts according to signature */
    void loadtext(string id, textdef * t, object::signature & sig) {
      string pid;
      pid = id + ".text";
      bind(pid, t->text);
      t->text = sig[pid];
      hook(pid, (component::call)&renderer::texthandler);

      pid = id + ".blended";
      bind(pid, t->blended);
      t->blended = eval(sig[pid], false);
      hook(pid, (component::call)&renderer::texthandler);

      pid = id + ".font";
      bind(pid, t->font);
      t->font = sig[pid];
      hook(pid, (component::call)&renderer::texthandler);

      pid = id + ".font.size";
      bind(pid, t->fontsz);
      t->fontsz = eval<int>(sig[pid], 10);
      hook(pid, (component::call)&renderer::texthandler);

      pid = id + ".font.r";
      bind(pid, t->r);
      t->r = eval(sig[pid], 0.0f);
      hook(pid, (component::call)&renderer::texthandler);

      pid = id + ".font.g";
      bind(pid, t->g);
      t->g = eval(sig[pid], 0.0f);
      hook(pid, (component::call)&renderer::texthandler);

      pid = id + ".font.b";
      bind(pid, t->b);
      t->b = eval(sig[pid], 0.0f);
      hook(pid, (component::call)&renderer::texthandler);

      pid = id + ".styles";
      bind(pid, t->styles);
      t->styles = sig[pid];
      hook(pid, (component::call)&renderer::texthandler);
    }

    // split and iterates through the surface list, loading them
    // and binding id and position parameters
    void loadsurfaces(string surfacelist, string imgpath = "") {
      set<string> surfaces;
      set<string>::iterator surfacedef;
      split(surfaces, surfacelist, ' ');
      for(surfacedef = surfaces.begin(); surfacedef != surfaces.end(); surfacedef++) {
        int p = surfacedef->find('=');
        string id = surfacedef->substr(0, p);
        string file = surfacedef->substr(p + 1);
        if(surfacebyid.find(id) != surfacebyid.end()) continue;
        SDL_Surface * raw = getraw(imgpath + file);
        surface * s = prepare(id, raw);
      }
    }

    void loadtexts(string textlist) {

    }

    surface * prepare(string id, SDL_Surface * raw) {
      if(raw == 0) return 0;
      surface * s = new surface(this, id, raw);
      surfacebyid[id] = s;

      // these will bind param names to our data
      bind(id + ".position.x", s->x);
      bind(id + ".position.y", s->y);
      bind(id + ".position.z", s->z);
      bind(id + ".position.w", s->w);
      bind(id + ".position.h", s->h);
      bind(id + ".position.rotation", s->rotation);
      bind(id + ".clip.x", s->clip.x);
      bind(id + ".clip.y", s->clip.y);
      bind(id + ".clip.w", s->clip.w);
      bind(id + ".clip.h", s->clip.h);
      bind(id + ".bind", s->bind);
      bind(id + ".render", s->render);
      bind(id + ".absolute", s->absolute);
      bind(id + ".alpha", s->alpha);
      bind(id + ".zoom", s->zoom);

      hook(id + ".position.z", (component::call)&renderer::zhandler);
      hook(id + ".zoom", (component::call)&renderer::zoomhandler);
      hook(id + ".position.rotation", (component::call)&renderer::rotationhandler);

      return s;
    }


    static SDL_Surface * getraw(string file, bool reload = false) {
      modinfo("renderer2d");
      SDL_Surface * tmp, *s;

      /* return the same already-loaded surface */
      std::map<std::string, SDL_Surface *>::iterator it;
      if((it = rawbyfile.find(file)) != rawbyfile.end()) {
        s = it->second;
        return s;
      }

      tmp = IMG_Load(file.c_str());
      if(tmp == 0) {
        trace("Error loading file", file, "[", SDL_GetError(), "]", log::error);
        return 0;
      }
      if(tmp->format->Amask != 0) {
        SDL_SetAlpha(tmp, SDL_RLEACCEL | SDL_SRCALPHA, 255);
        s = SDL_DisplayFormatAlpha(tmp);
      } else {
        SDL_SetColorKey(tmp, SDL_RLEACCEL | SDL_SRCCOLORKEY, SDL_MapRGB(tmp->format, 0xff, 0, 0xff));
        s = SDL_DisplayFormat(tmp);
      }
      SDL_FreeSurface(tmp);
      rawbyfile[file] = s;
      return s;
    }

  private:
    static void render() {
      if(zordered.size() == 0) return;
//       modinfo("renderer");
      SDL_Rect dstrect;
      SDL_Rect srcrect;
      SDL_Surface * target;
      for(set<zorder>::iterator i = zordered.begin(); i != zordered.end(); i++) {
        surface * s = i->second;
        dstrect.x = dstrect.y = 0;
        if(s->render == false) continue;
        if(s->bind) {
          const float & x = s->parent->raw<float>("x");
          const float & y = s->parent->raw<float>("y");
          dstrect.x = x;
          dstrect.y = y;
        }
        dstrect.x += s->x;
        dstrect.y += s->y;
        srcrect = s->clip;
        if(s->rotozoomed != NULL)  target = s->rotozoomed;
        else target = s->raw;
        SDL_SetAlpha(target, SDL_SRCALPHA | SDL_RLEACCEL, s->alpha * 255);
        if(s->dirty) {
          if(s->rotozoomed != NULL) SDL_FreeSurface(s->rotozoomed);
          s->rotozoomed = rotozoomSurface(s->raw, s->rotation, s->zoom, 1);
          dstrect.x = (s->w / 2 - s->rotozoomed->w / 2) + dstrect.x;
          dstrect.y = (s->h / 2 - s->rotozoomed->h / 2) + dstrect.y;
          s->x = dstrect.x - (s->bind ? s->parent->raw<float>("x") : 0);
          s->y = dstrect.y - (s->bind ? s->parent->raw<float>("y") : 0);
          srcrect.w = s->rotozoomed->w;
          srcrect.h = s->rotozoomed->h;
          s->clip = srcrect;
          target = s->rotozoomed;
          s->dirty = false;
        }
//         trace("Rendering", s->id, "at", dstrect.x, dstrect.y);
        SDL_BlitSurface(target, &srcrect, screen, &dstrect);
      }
      SDL_Flip(screen);
      SDL_Delay(1);
    }
    static void initialize(int w, int h) {
      modinfo("renderer2d");
      if(initialized == true) return;
      if(!SDL_WasInit(SDL_INIT_VIDEO)) {
        trace("Initializing screen", log::info);
        if(SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
          throw evil(string("(Rendering component) Failed initializing SDL: ") + SDL_GetError());
        }
      }

      if(screen == 0) screen = SDL_SetVideoMode(w, h, 32, SDL_DOUBLEBUF);
      if(screen == 0) {
        trace("Failure to initialize video", "[", SDL_GetError(), "]", log::error);
        throw evil(string("(Rendering component) Failed initializing video"));
      }

      if(!TTF_WasInit()) {
        if(TTF_Init() != 0) {
          trace("Failure to initialize font rendering", "[", TTF_GetError(), "]", log::error);
          throw evil(string("(Rendering component) Failed initializing font renderer: "));
        }
      }
      initialized = true;
    }
};

map<string, SDL_Surface *> renderer::rawbyfile;
set<renderer::zorder> renderer::zordered;
SDL_Surface * renderer::screen = 0;
bool renderer::initialized = false;
int renderer::rendervotes = 0;
list<renderer *> renderer::renderers;
map<string, TTF_Font *> renderer::fonts;

g2dcomponent(renderer)
