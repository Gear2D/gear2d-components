/**
 * @file spritesheet.cc
 * @author Leonardo Guilherme de Freitas
 * @page animation/spritesheet animation/spritesheet
 * 
 * @section animation-spritesheet-information General information
 * - @b Family: animation
 * - @b Type: spritesheet
 * - @b Full @b name: animation/spritesheet
 * 
 * @section Description
 * 
 * The spritesheet kind of animation manpulates the clipping rectangle of a surface
 * to create the illusion of movement. For it to work you need to first specify
 * correctly the clipping rectangle in a @b renderer component (for instance,
 * @ref renderer/renderer2d).
 * 
 * Currently it only supports moving horizontally (that is, changing the clipping
 * position to the right or to the left, using <tt>&lt;surface-id&gt;.clip.x</tt>),
 * but it behaves correctly if you changethe vertical position of the clipping
 * rectangle <tt>(&lt;surface-id&gt;.clip.y)</tt>.
 * 
 * @section Dependencies
 * 
 * - A renderer component that handles clipping rectangles.
 * 
 @section Parameters
 <dl>
 <dt class="note">@c animations: </dt>
  <dd>List of surface ids to be used as animation sprites. Those ids will be
  enhanced with parameters that the spritesheet component is sensible to.
      - @c type: List of strings of surface-ids separated by space
      - @c default: empty
  </dd>
  <dt class="note"><tt>&lt;surface-id&gt;.animation.frame.size</tt></dt>
  <dd>
    Width of the frame.
  </dd>
  <dt class="note">&lt;surface-id&gt;.animation.frame</dt>
  <dd>
  </dd>
  
 </dl>
 * @li @c <surface-id>.animation.frame.size Width of the sprite frame @b int
 * @li @c <id>.animation.frame Current animation frame @b int
 * @li @c <id>.animation.finished Listen to this if you want to know when the animation has finished @b bool
 * @li @c <id>.animation.wrap Shall the animation wrap? @b bool
 * @li @c <id>.animation.play Play or stop the current animation @b bool
 * @li @c <id>.animation.speed Speed of animation (seconds between frames) @b float
 * 
 * @see @ref renderer/renderer2d
 * 
 */

#include "gear2d.h"
using namespace gear2d;

#include <string>
using namespace std;

class animation {
  public:
    string id;
    int framew;
    int frameh;
    int spritew;
    int spriteh;
    int currframe;
    bool wrap;
    bool play;
    bool finished;
    float speed;
    float left; // time left
    
    
    animation(string id) : id(id)
    , framew(0)
    , frameh(0)
    , spritew(0)
    , spriteh(0)
    , currframe(0)
    , wrap(false)
    , play(false)
    , finished(false)
    , speed(0)
    , left(0) {
      
    }
};

class spritesheet : public component::base {
  private:
    map<string, animation *> anims;
    
  public:
    virtual component::family family() { return "animation"; }
    virtual component::type type() { return "spritesheet"; }
    virtual string depends() { return "renderer"; }
    virtual void handle(parameterbase::id pid, component::base * last, object::id owns) {
      int p;
      if ((p = pid.find(".animation.frame")) != string::npos && p != -1) {
        string id = pid.substr(0, p);
        animation * a = anims[id];
        if (a != 0) {
          write(id + ".clip.x", a->currframe * a->framew);
        }
        return;
      }
      
      // update spriew and spriteh in case it changes
      if ((p = pid.find(".position.w")) != string::npos && p != -1) {
        string id = pid.substr(0, p);
        animation * a = anims[id];
        if (a != 0) {
          a->spritew = read<int>(id + ".position.w");
        }
        return;
      }
      if ((p = pid.find(".position.h")) != string::npos && p != -1) {
        string id = pid.substr(0, p);
        animation * a = anims[id];
        if (a != 0) {
          a->spriteh = read<int>(id + ".position.h");
        }
        return;
      }
      
      if ((p == pid.find(".animation.frame.size")) != string::npos && p != -1) {
        string id = pid.substr(0, p);
        animation * a = anims[id];
        //recalculate frames
        write(id + ".clip.x", a->currframe * a->framew);
        return;
      }
      
      if ((p == pid.find(".animation.play")) != string::npos && p != -1) {
        string id = pid.substr(0, p);
        cout << "id:  " << id << endl;
        animation * a = anims[id];
        if (a->play == true) {
          if (a->finished == true) {
            write(id + ".animation.finished", false);
          }
        }
      }
      
    }
    
    virtual void setup(object::signature & sig) {
      modinfo(type());
      string animline = sig["animations"];
      set<string> animations;
      if (!animline.empty()) {
        animations = split(animations, animline, ' ');
        string id;
        for (set<string>::iterator i = animations.begin(); i != animations.end(); i++) {
          id = *i;
          if (sig["renderer.surfaces"].find(id) == string::npos) {
            trace("Unable to load animation ", id, "because its not on the surface list", log::warning);         
            continue;
          }
          animation * a = loadanim(id);
          string pid;
          
          a->spritew = read<int>(id + ".position.w");
          a->spriteh = read<int>(id + ".position.h");
          hook(id + ".position.w");
          hook(id + ".position.h");
          
          pid = id + ".animation.frame.size";
          write(pid, eval(sig[pid], a->spritew));
          hook(id + ".animation.frame.size");
          
          pid = id + ".animation.frame";
          hook(pid); // bind has already initialized it.
          write(pid, eval(sig[pid], 0));
          
          pid = id + ".animation.wrap";
          write(pid, eval(sig[pid], false));
          
          pid = id + ".animation.play";
          write(pid, eval(sig[pid], false));
          hook(pid);
          
          pid = id + ".animation.speed";
          write(pid, eval(sig[pid], 0.0f));
          a->left = a->speed;
          
          write(id + ".animation.finished", false);
        }
      }
    }
    
    virtual void update(float dt) {
      animation * a;
      for (map<string, animation *>::iterator i = anims.begin(); i != anims.end(); i++) {
        a = i->second;
        if (!a->play) continue;
        a->left -= dt;
        if (a->left <= 0) {
          int frame = a->currframe;
          frame++;
          if (a->framew*frame >= a->spritew) {
            if (a->wrap) frame = 0;
            else {
              frame--;
              write(a->id + ".animation.finished", true);
            }
          }
          if (frame != a->currframe) write(a->id + ".animation.frame", frame);
          a->left = a->speed;
        }
        
      }
    }

  private:
    animation * loadanim(string anim) {
      animation * a = anims[anim];
      if (a != 0) return a;
      /*<id>.sheet.frame.w Width of the sprite frame @b int
      * @li @c <id>.sheet.frame.h Height of the sprite frame @b int
      * @li @c <id>.animation.frame Current animation frame @b int
      * @li @c <id>.animation.wrap Shall the animation wrap? @b bool
      * @li @c <id>.animation.play Play or stop the current animation @b bool
      * @li @c <id>.animation.speed Speed of animation (seconds between frames) @b float
      */
      a = new animation(anim);
      bind(anim + ".animation.frame.size", a->framew);
      bind(anim + ".animation.frame", a->currframe);
      bind(anim + ".animation.wrap", a->wrap);
      bind(anim + ".animation.play", a->play);
      bind(anim + ".animation.speed", a->speed);
      bind(anim + ".animation.finished", a->finished);
      anims[anim] = a;
      return a;
    }
};

extern "C" {
  component::base * build() { return new spritesheet; }
}