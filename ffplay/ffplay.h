#ifndef FFPLAY_H
#define FFPLAY_H 1
#pragma once
#include "libavutil/imgutils.h"
#include "media_object.h"
#include <SDL.h>
#include <SDL_thread.h>

struct ogre_video_texture {
  uint8_t *dst_data[4];
  int dst_linesize[4];

  uint8_t *sub_data[4];
  int sub_linesize[4];
  int sub_buffer_size;
  int sub_w;
  int sub_h;

  SDL_mutex *mutex;
  SDL_mutex *sub_mutex;
  int is_ready;
  int sub_is_ready;
  int coded_h, coded_w;
  int update_texture;
  int buffer_size;
  struct SwsContext *ptr;
};
struct user_interface {
  int stop_play;
  int force_refresh;
  int pause;
  int mute;
  int current_playback_id;
  int current_list;
  int playback_finished;
  int is_playing;
  int skip_n_frame;
  int next_audio_stream;
  int next_video_stream;
  int next_subtitle_stream;
  int next_all_streams;
  int display_av_graph;
  int next_chapter;
  int previous_chapter;
  int playback_id;
  int play_all;
  int repeat;
  int fast_forward;
  int fast_backward;
  int increase_volume;
  int decrease_volume;
  int startup_volume;
  int infinite_buffer;
  char *Video_title;

  SDL_mutex *mutex;
  int seek_requested;
  double skip_to_video_time;
  double current_video_time;
  double total_seek_value;

  int video_buffer_size;
  int audio_buffer_size;

  int network_initialized;
};
struct ogre_video_texture ogre_ff_texture;
struct user_interface ui_setting;

int start_player(struct Media_Object media_object);
void init_opts(struct Media_Object media_object);
void uninit_opts(void);
#endif