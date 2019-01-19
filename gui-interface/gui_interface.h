/*MIT License
 * 
 * Copyright (c) 2019 NeonTechnologies
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
#ifndef GUI_INTERFACE_H
#define GUI_INTERFACE_H 1
#include "boost/date_time/posix_time/posix_time.hpp"
#include "media_object.h"
#include "utilities.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#endif
#include "SDL.h"
#include <OgreHardwarePixelBuffer.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgrePass.h>
#include <OgreTechnique.h>
#include <OgreWindowEventUtilities.h>
#include <boost/algorithm/string.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/range/iterator_range.hpp>
#ifdef USE_BOOST_THREAD
  #include <boost/thread.hpp>
#else
  #include <thread>
#endif
#include <boost/timer/timer.hpp>
#include "cef_osr/browser_client.h"
#include "cef_osr/cefv8_handler.h"
#include <chrono>
#include <iomanip>
#include <map>
#include <string>


class ImguiExample : public OgreBites::ApplicationContext,
                     public OgreBites::InputListener {
public:
  ImguiExample() : OgreBites::ApplicationContext("Cloud_Commander_UI") {}
  #ifdef USE_BOOST_THREAD
    boost::thread *player;
  #else
    std::thread *player;
  #endif
  Ogre::SceneManager *scnMgr;
  Ogre::Entity *cube;
  bool copy_to_clipboard = false;
  Ogre::RenderWindow *mWindow = 0;
  Ogre::Root *root = 0;
  std::atomic<double> refresh_rate = 63;
  double PCFreq = 0.0;
  __int64 CounterStart = 0;
  float gui_transparancy = -2.4f;
  Ogre::MaterialPtr renderMaterial;
  Ogre::Rectangle2D *mMiniScreen ;Ogre::Rectangle2D *cefScreen;

   Ogre::SceneNode *miniScreenNode; Ogre::SceneNode *cefScreenNode;
  
  std::string player_time = "0:0:0";
  std::string player_file_types = ".mkv|.flv|.aac|.vob|.m4a|.mp4|.mp3";
  double old_time;
  double last_max_abuff = 0, last_max_vbuff = 0;

  std::atomic<bool> is_playing;
  boost::posix_time::ptime start_time;
  boost::posix_time::ptime current_time;

  int listbox_items_count = 0;
  int listbox_items_2_count = 0;
  int selected_theme = 1;
  int selected_list = 0, old_selected_list = 0;
  char **listbox_items = 0;
  char **listbox_items_2 = 0;
  char **listbox_episodes = 0;

  char **play_list_items = 0;
  char **current_play_list = 0;

  int last_listbox_items = 0, last_listbox_items_2 = 0,
      last_listbox_episodes = 0, last_playlist_item = 0;
  int old_listbox_item_current = 0, old_listbox_item_current_1 = 0;
  int listbox_item_current = 0;
  int listbox_item_current_1 = 0;
  bool enable_hide = true, hide_is_video = false;
  char *item_current_2 = 0;
  char *item_current = 0;
  struct elapsed_time mouse_moves;
  std::string dedicated_channel="0";

  ////cef stuff
  RenderHandler* renderHandler;
  CefRefPtr<CefBrowser> browser;
  CefRefPtr<BrowserClient> browserClient;

  ////cef stuff


  // std::string time_string(int time_t);
  void update();
  bool frameStarted(const Ogre::FrameEvent &evt);
  void update_timers();
  bool frameEnded(const Ogre::FrameEvent &evt);
  bool frameRenderingQueued(const Ogre::FrameEvent &evt);
  void windowResized(	Ogre::RenderWindow * 	mWindow	);
  void request_login();
  void setup();
  bool mousePressed(const OgreBites::MouseButtonEvent &arg);
  bool keyPressed(const OgreBites::KeyboardEvent &evt);
  bool mouseMoved(const OgreBites::MouseMotionEvent &evt);
  void ClearLog();
  void show_console();
  void refresh_video_frame(void);
  void show_up_down_load_manager(void);
  void start_playback(struct Media_Object media_object);
  void stop_playback();
  void increase_volume();
  void decrease_volume();
  void mute_playback();
  void pause_playback();
  void next_chapter();
  void previous_chapter();
  void forward_playback();
  void next_subtitle();
  void rewind_playback();
  void next_audio_channel();
  void play_next_media(int direction);
  void remove_material(std::string material_name);
  void remove_texture(std::string texture_name);
  Ogre::MaterialPtr create_material(std::string material_name);
  bool Spinner(const char *label, float radius, int thickness,
               const ImU32 &color);
  bool BufferingBar(const char *label, float value, const ImVec2 &size_arg,
                    const ImU32 &bg_col, const ImU32 &fg_col);
  void set_texture_color(std::string material_name, std::string texture_name,
                         int w, int h);
  void bind_material_texture(std::string material_name,
                             std::string texture_name, int w, int h);
  Ogre::TexturePtr ImguiExample::create_texture(std::string texture_name, int w,
                                                int h);

  void closeApp();
};
#endif