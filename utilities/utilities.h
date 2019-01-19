#ifndef UTILITIES_H
#define UTILITIES_H 1
#include "common.h"
#include "rapidxml.hpp"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  #include "windows.h"
#endif
#include <iostream>
#include <vector>

#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreRectangle2D.h>
//#include "client_ws.hpp"
#include "ImguiManager.h"
#include "imgui_dock.h"
#include "imgui_internal.h"
#include "media_object.h"
extern ImVector<char *> Items;
static char *Strdup(const char *str);
void AddLog(const char *fmt, ...) IM_FMTARGS(2);
float normalize(float input, float max, float min);
void parse_xml(std::string to_parse);
double GetCounter(double &CounterStart);
double StartCounter();
struct Alpha_map {
  char *Alpha;
  int id;
};
struct Titles_map {
  char *Title;
  char *page_link;
  int dedicated_channel;
  int id;
};
struct Episode_map {
  char *Episode;
  char *page_link;
  int id;
};
extern bool items_updated;
extern std::map<int, struct Alpha_map> anime_alpha_titles;
extern std::map<int, struct Titles_map> anime_title_names_2;
extern std::map<int, struct Episode_map> anime_temp_episodes;
extern struct account_info device_login;
struct list_item {
  std::string name;
  std::vector<struct Media_Object> play_list;
};

extern std::map<int, struct list_item> selected;

extern std::mutex play_list_lock, anime_temp_episodes_lock,
    anime_alpha_titles_lock, anime_title_names_2_lock;
extern std::vector<std::string> qued_messages;

extern bool alpha_requested;
extern bool alpha_listing_requested;
extern bool episode_listing_requested;
#endif