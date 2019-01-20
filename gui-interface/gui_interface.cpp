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
#include "gui_interface.h"
#include "common.h"
extern "C" {
#include <ffplay.h>
}
#include "version.h"
extern struct ogre_video_texture ogre_ff_texture;
extern struct user_interface ui_setting;
struct account_info device_login;
std::atomic<connection_info> current_connection_info;
struct download_scheduler active_downloads;
struct download_task temp_task;
boost::container::flat_map<int, struct elapsed_time> active_timers;
bool use_user = false;
int current_rate = 5;
bool update_scroll = false;
std::atomic<bool> reset;
std::atomic<bool> system_ready;
std::atomic<bool> is_quit;
int pressed_key = 0;
bool ScrollToBottom = true;

float max_value = 0;
float min_value = 0;
int last_dlspeed = 0;
static float download_log[150];

float max_ul_value = 0;
float min_ul_value = 0;
int last_ulspeed = 0;
static float upload_log[150];

int log_count = 1;
ImVector<char *> Items;
static bool repeat = false;
static bool infinite_buffer = false;
int current_log = 0;
int current_ul_log;
int dl_clicked = 0;
using nanoseconds = std::chrono::nanoseconds;
using microseconds = std::chrono::microseconds;
using milliseconds = std::chrono::milliseconds;
using seconds = std::chrono::seconds;
using seconds_d64 = std::chrono::duration<double>;
std::string time_string(int time_t) {
  std::stringstream os;
  using hours = std::chrono::hours;
  using minutes = std::chrono::minutes;
  using std::chrono::duration_cast;

  seconds t = (seconds)time_t;
  if (t.count() < 0) {
    os << '-';
    t *= -1;
  }

  // Only handle up to hour formatting
  if (t >= hours(1))
    os << duration_cast<hours>(t).count() << 'h' << std::setfill('0')
       << std::setw(2) << (duration_cast<minutes>(t).count() % 60) << 'm';
  else
    os << duration_cast<minutes>(t).count() << 'm' << std::setfill('0');
  os << std::setw(2) << (duration_cast<seconds>(t).count() % 60) << 's'
     << std::setw(0) << std::setfill(' ');
  std::string final_string = os.str();
  ;
  return final_string;
}

void ImguiExample::update() {

  if (ImGui::Begin("CLoud_Commander [R]", NULL,
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
    // dock layout by hard-coded or .ini file
    ImGui::BeginDockspace();
    if (1) {
      if (ImGui::BeginDock("Gadget Central")) {
        miniScreenNode->setVisible(false);
        cefScreenNode->setVisible(true);
/*
        const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
        const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

        static float f2 = 108.9;
        static float f3 = 10.9;
        static float f4 = 0.0;
        static float f5 = 0;

        {
          ImGui::Columns(3, "mixed", false);

          for (int i = 0; i < f5; i++) {
            ImGui::Text("");
          }

          for (int i = 0; i < f4; i++) {
            ImGui::Spacing();
            ImGui::SameLine();
          }
          Spinner("##spinner", f2, f3, col);
          //////////////////////////////////////////
          ImGui::NextColumn();
          ImGui::Text("DEVICE OPTIONS");
          // ImGui::NextColumn();

          ImGui::Columns(1);
        }
        show_console();*/
      }
      ImGui::EndDock();
    }
    if (0) {
      if (ImGui::BeginDock("Firmware Central")) {
        miniScreenNode->setVisible(false);
        cefScreenNode->setVisible(true);
        ImGui::Text("Nothing here! Ooops");
      }
      ImGui::EndDock();
    }
    if (0) {
      if (ImGui::BeginDock("PCB Layouts/Pinouts")) {
         miniScreenNode->setVisible(false);
        cefScreenNode->setVisible(false);
        ImGui::Text("Nothing here! Ooops");
      }
      ImGui::EndDock();
    }

    if (ImGui::BeginDock("Settings")) {
        miniScreenNode->setVisible(false);
        cefScreenNode->setVisible(false);
      ImGui::Text("CPU REFRESH RATE(Increase this on slower machine)");
      static float f1 = (float)refresh_rate.load();
      if (ImGui::SliderFloat("PF", &f1, 0.0f, 300.0f, "ratio = %.1f"))
        refresh_rate.store((float)f1);

      ImGui::Text("GUI Transparency");
      ImGui::SliderFloat("TPf", &gui_transparancy, -30.0f, 30.0f,
                         "ratio = %0.01f");
      ImGui::Text("Player file types");
      char *str0 = new char[400];

      strcpy(str0, player_file_types.c_str());
      ImGui::InputText("   ", str0, 400);
      player_file_types = str0;
      delete str0;
      const char *items[] = {"Classic", "Dark", "Light", "None"};
      static const char *current_theme =
          items[0]; // Here our selection is a single pointer stored outside the
                    // object.
      ImGui::Text(std::string(std::string("Keyboard key:") +
                              std::to_string(pressed_key))
                      .c_str());
      if (ImGui::BeginCombo("Theme",
                            current_theme)) // The second parameter is the label
      // previewed before opening the combo.
      {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
          bool is_selected = (item_current == items[n]);
          if (ImGui::Selectable(items[n], is_selected)) {
            current_theme = items[n];
            selected_theme = n;
          }
          if (is_selected)
            ImGui::SetItemDefaultFocus(); // Set the initial focus when opening
                                          // the combo (scrolling + for keyboard
                                          // navigation support in the upcoming
                                          // navigation branch)
        }
        ImGui::EndCombo();
      }

      ImGui::Text("[TIMERS]");
      ImGui::Columns(3, "mixed", true);
      ImGui::SetColumnWidth(-1, 10);
      ImGui::Text("NO");
      ImGui::NextColumn();
      ImGui::Text("NAME");
      ImGui::NextColumn();
      ImGui::Text("VALUE");

      for (auto clock_timer : active_timers) {
        ImGui::NextColumn();
        ImGui::Text(std::to_string(clock_timer.first).c_str());
        ImGui::NextColumn();
        ImGui::Text(clock_timer.second.name.c_str());
        ImGui::NextColumn();

        if (clock_timer.second.timer_type == 0) {
          auto milliseconds =
              boost::chrono::duration_cast<boost::chrono::milliseconds>(
                  clock_timer.second.total_elapsed);
          ImGui::Text(std::to_string(milliseconds.count()).c_str());
        } else {
          auto milliseconds = clock_timer.second.total_elapsed_frq;
          ImGui::Text(std::to_string(milliseconds).c_str());
        }
      }
      ImGui::Columns(1);
    }
    ImGui::EndDock();
    if (0) {
      if (ImGui::BeginDock("EEPROM/SPI")) {
                miniScreenNode->setVisible(false);
        cefScreenNode->setVisible(false);
        ImGui::Text("Nothing here! Ooops");
      }
      ImGui::EndDock();
    }
    if (1) {
      if (ImGui::BeginDock("Anime")) {
        miniScreenNode->setVisible(true);
        cefScreenNode->setVisible(false);
        ImGui::Text("Anime DoWnload and Playback Plugin");
        ImGui::Separator();
        ImGui::Text("Titles");
        anime_alpha_titles_lock.lock();
        bool alpha_titles_empty = anime_alpha_titles.empty();
        int alpha_titles_size = anime_alpha_titles.size();
        anime_alpha_titles_lock.unlock();

        anime_title_names_2_lock.lock();
        bool anime_title_names_2_empty = anime_title_names_2.empty();
        int anime_title_names_2_size = anime_title_names_2.size();
        anime_title_names_2_lock.unlock();

        anime_temp_episodes_lock.lock();
        bool anime_temp_episodes_empty = anime_temp_episodes.empty();
        int anime_temp_episodes_size = anime_temp_episodes.size();
        anime_temp_episodes_lock.unlock();
        if (!alpha_requested)
          if (alpha_titles_empty) {
            qued_messages.push_back(
                "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
                                         <root>\
                                             <messages>\
                                                 <message>ANIME</message>\
                                             </messages>\
                                         </root>");
            alpha_requested = true;
          }
        if (!alpha_titles_empty)
          if (!alpha_listing_requested)
            if (1) {
              qued_messages.push_back(
                  "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
                                         <root>\
                                             <messages>\
                                                 <message alpha=\"" +
                  std::string(anime_alpha_titles[0].Alpha) +
                  "\">ANIME_LISTING</message>\
                                             </messages>\
                                         </root>");
              alpha_listing_requested = true;
            }
        if (!episode_listing_requested)
          if (!anime_title_names_2_empty)
            if (!anime_temp_episodes_empty) {
              std::string page_link;
              if (anime_title_names_2[0].dedicated_channel == 1)
                page_link = "https://ww18.watchop.io" +
                            std::string(anime_title_names_2[0].page_link);
              else
                page_link = "https://www6.animesubhd.net" +
                            std::string(anime_title_names_2[0].page_link);

              qued_messages.push_back(
                  "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
                                                  <root>\
                                                      <messages>\
                                                          <message page_link=\"" +
                  page_link + "\" dedicated_channel=\"" +
                  std::to_string(anime_title_names_2[0].dedicated_channel) +
                  "\">ANIME_EPISODES</message>\
                                                      </messages>\
                                                  </root>");
              dedicated_channel =
                  std::to_string(anime_title_names_2[0].dedicated_channel);
              /* qued_messages.push_back(
                   "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
                                                  <root>\
                                                      <messages>\
                                                          <message
                 page_link=\"https://www6.animesubhd.net" +
                   std::string(anime_temp_episodes[0].page_link) +
                   "\">ANIME_EPISODES</message>\
                                                      </messages>\
                                                  </root>");*/
              episode_listing_requested = true;
            }

        if (alpha_titles_size > 0) {
          if (listbox_items != 0) {
            if (last_listbox_items > 0) {
              for (int i = 0; i < last_listbox_items; i++)
                listbox_items[i] = nullptr;
            }
            delete listbox_items;
            last_listbox_items = 0;
          }
          last_listbox_items = alpha_titles_size;
          listbox_items = new char *[last_listbox_items];
          for (unsigned int i = 0; i < last_listbox_items; i++) {
            anime_alpha_titles_lock.lock();
            listbox_items[i] = anime_alpha_titles[i].Alpha;
            anime_alpha_titles_lock.unlock();
          }
          // anime_alpha_titles.clear();
        }
        ///////////////////////////////////////////////////////////////////
        if (anime_title_names_2_size > 0) {
          if (listbox_items_2 != 0) {
            if (last_listbox_items_2 > 0) {
              for (int i = 0; i < last_listbox_items_2; i++)
                listbox_items_2[i] = nullptr;
            }
            delete[] listbox_items_2;
            last_listbox_items_2 = 0;
          }
          last_listbox_items_2 = anime_title_names_2_size;
          listbox_items_2 = new char *[last_listbox_items_2];
          for (unsigned int i = 0; i < last_listbox_items_2; i++) {
            anime_title_names_2_lock.lock();
            listbox_items_2[i] = anime_title_names_2[i].Title;
            anime_title_names_2_lock.unlock();
          }
          // anime_title_names_2.clear();
        }
        ///////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////
        if (anime_temp_episodes_size > 0) {
          if (listbox_episodes != 0) {
            if (last_listbox_episodes > 0) {
              for (int i = 0; i < last_listbox_episodes; i++)
                listbox_episodes[i] = nullptr;
            }
            delete[] listbox_episodes;
            last_listbox_episodes = 0;
          }
          last_listbox_episodes = anime_temp_episodes_size;
          listbox_episodes = new char *[last_listbox_episodes];
          for (unsigned int i = 0; i < anime_temp_episodes_size; i++) {
            anime_temp_episodes_lock.lock();
            listbox_episodes[i] = anime_temp_episodes[i].Episode;
            anime_temp_episodes_lock.unlock();
          }
          // anime_temp_episodes.clear();
        }
        /////////////////////////////////////////////////////////////////////

        float new_width =
            ((float)60 / mWindow->getWidth()) * mWindow->getWidth();

        ImGui::PushItemWidth(new_width);

        ImGui::ListBox("", &listbox_item_current, listbox_items,
                       last_listbox_items, 4);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::ListBox(" ", &listbox_item_current_1, listbox_items_2,
                       last_listbox_items_2, 4);
        if (alpha_listing_requested)
          if (!alpha_titles_empty)
            if (listbox_item_current != old_listbox_item_current) {

              qued_messages.push_back(
                  "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
                                                              <root>\
                                                                  <messages>\
                                                                      <message alpha =\"" +
                  std::string(anime_alpha_titles[listbox_item_current].Alpha) +
                  "\">ANIME_LISTING</message>\
                                                                  </messages>\
                                                              </root>");
              old_listbox_item_current = listbox_item_current;
              alpha_listing_requested = true;
            }
        ImGui::Separator();
        ImGui::Text("Select The Episodes to Download/Play");
        static ImGuiComboFlags flags = 0;

        if (item_current == 0 || items_updated)
          if (listbox_episodes != 0) {
            item_current = listbox_episodes[0];
          }
        ImGui::Separator();
        ImGui::Text("First Episode to Download");

        if (!anime_title_names_2_empty) {
          int current = 0;
          if (listbox_item_current_1 != old_listbox_item_current_1 ||
              !episode_listing_requested) {
            current = listbox_item_current_1;
            std::string page_link;
            if (anime_title_names_2[current].dedicated_channel == 1)
              page_link = "https://ww18.watchop.io" +
                          std::string(anime_title_names_2[current].page_link);
            else
              page_link = "https://www6.animesubhd.net" +
                          std::string(anime_title_names_2[current].page_link);

            qued_messages.push_back(
                "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
                                                  <root>\
                                                      <messages>\
                                                          <message page_link=\"" +
                page_link + "\" dedicated_channel=\"" +
                std::to_string(anime_title_names_2[current].dedicated_channel) +
                "\">ANIME_EPISODES</message>\
                                                      </messages>\
                                                  </root>");
            dedicated_channel =
                std::to_string(anime_title_names_2[current].dedicated_channel);
            episode_listing_requested = true;
          }
          old_listbox_item_current_1 = listbox_item_current_1;
        }
        if (ImGui::BeginCombo("", item_current,
                              flags)) // The second parameter is the label
        // previewed before opening the combo.
        {
          for (int n = 0; n < last_listbox_episodes; n++) {
            bool is_selected = (item_current == listbox_episodes[n]);
            if (ImGui::Selectable(listbox_episodes[n], is_selected))
              item_current = listbox_episodes[n];
            if (is_selected)
              ImGui::SetItemDefaultFocus(); // Set the initial focus when
                                            // opening the combo (scrolling +
                                            // for keyboard navigation support
                                            // in the upcoming navigation
                                            // branch)
          }
          ImGui::EndCombo();
        }
        ImGui::Text("Last Episode to Download");

        if (item_current_2 == 0 || items_updated)
          if (listbox_episodes != 0) {
            item_current_2 = listbox_episodes[last_listbox_episodes - 1];
            items_updated = false;
          }
        if (ImGui::BeginCombo(" ", item_current_2,
                              flags)) // The second parameter is the label
        // previewed before opening the combo.
        {
          for (int n = 0; n < last_listbox_episodes; n++) {
            bool is_selected_2 = (item_current_2 == listbox_episodes[n]);
            if (ImGui::Selectable(listbox_episodes[n], is_selected_2))
              item_current_2 = listbox_episodes[n];
            if (is_selected_2)
              ImGui::SetItemDefaultFocus(); // Set the initial focus when
                                            // opening the combo (scrolling +
                                            // for keyboard navigation support
                                            // in the upcoming navigation
                                            // branch)
          }
          ImGui::EndCombo();
        }

        static int clicked = 0;

        if (ImGui::Button("Download")) {
          qued_messages.push_back(
              "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
                                                  <root>\
                                                      <messages>\
                                                          <message url=\"https://ww18.watchop.io\"   first_episode=\"" +
              std::string(item_current) + "\" last_episode=\"" +
              std::string(item_current_2) + "\" dedicated_channel=\"" +
              dedicated_channel + "\">ANIME_DOWNLOAD</message>\
                                                      </messages>\
                                                  </root>");
          clicked++;
        }

        ImGui::SameLine();
        static int clicked_2 = 0;
        if (ImGui::Button("Play"))
          clicked_2++;
        if (clicked & 1) {
          ImGui::Text("Download task requested, Check the downloads page.");
        }
        if (clicked_2 & 1) {
          ImGui::Text("We will begin playback soon, Check the downloads page.");
        }
        ImGui::Separator();
        const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
        const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);
        static float f1 = 737;

        static float f2 = 9.1;

        static float f3 = 0.7f;

        BufferingBar("##buffer_bar", f3, ImVec2(f1, f2), bg, col);
        show_console();
      }
      ImGui::EndDock();
    }
    if (1) {
      if (ImGui::BeginDock("Streaming")) {
        miniScreenNode->setVisible(true);
        cefScreenNode->setVisible(false);
        bool hide_this_window = false;
        bool time_out = boost::chrono::duration_cast<boost::chrono::seconds>(
                            active_timers[1].total_elapsed)
                            .count() >= 3;
        bool time_out_2 = boost::chrono::duration_cast<boost::chrono::seconds>(
                              active_timers[2].total_elapsed)
                              .count() >= 3;
        if (enable_hide) {
          if (time_out && time_out_2) {
            if (hide_is_video)
              hide_this_window = true;
            else
              hide_this_window = false;
          } else
            hide_this_window = false;
        } else
          hide_this_window = false;
        if (!hide_this_window) {

          static char str0[628] = "";
          ImGui::InputText("    ", str0, IM_ARRAYSIZE(str0));
          ImGui::SameLine();
          int total_playlists = selected.size();
          if (ImGui::Button("Load")) {
            std::string file_folder = str0;
            qued_messages.push_back(
                "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
                                                <root>\
                                                    <messages>\
                                                        <message types=\"" +
                player_file_types + "\" folder=\"" + file_folder +
                "\">PLAYLIST</message>\
                                                    </messages>\
                                                </root>");
            str0[0] = '\0';
          }
          SDL_LockMutex(ui_setting.mutex);
          ui_setting.play_all = 1;
          std::string title = ui_setting.Video_title;
          int first = title.find_first_of("'");
          int second = -1;

          if (first > -1)
            second = title.find("'", first + 1);
          if (second > -1)
            title = title.substr(first + 1, second - 1 - first);

          ImGui::Text(
              std::string("Playing [" + player_time + "] " + title).c_str());
          SDL_UnlockMutex(ui_setting.mutex);
          // ImGui::Text(std::string("Time: " + player_time).c_str());
          static int playlist_item_current_1 = 0;

          play_list_lock.lock();
          if (total_playlists > 0) {
            if (current_play_list != 0) {
              delete[] current_play_list;
            }
            current_play_list = new char *[total_playlists];
          }

          int i = 0;
          for (auto &curr : selected) {
            current_play_list[i] = new char[curr.second.name.length()];
            strcpy(current_play_list[i], (char *)curr.second.name.c_str());
            i++;
          }

          if (play_list_items != 0) {
            delete[] play_list_items;
            play_list_items = 0;
            last_playlist_item = 0;
          }
          last_playlist_item = selected[selected_list].play_list.size();
          if (selected[selected_list].play_list.size() > 0) {

            play_list_items = new char *[last_playlist_item];
            std::string temp;
            for (unsigned int i = 0; i < last_playlist_item; i++) {
              if (selected[selected_list].play_list[i].is_playing) {
                temp =
                    std::string(
                        "[Playing] " + player_time + " " +
                        std::string(selected[selected_list].play_list[i].name))
                        .c_str();
                play_list_items[i] = new char[temp.length()];
                strcpy(play_list_items[i], (char *)temp.c_str());
                /*if (update_scroll) {
                  ImGui::SetScrollHere();
                  update_scroll = false;
                }*/
              } else
                play_list_items[i] = selected[selected_list].play_list[i].name;
            }
            // selected[selected_list].play_list.clear();
          }
          play_list_lock.unlock();
          ///////////////////////////////////////////////////////////////////
          // current_play_list;
          ImGui::ListBox(" ", &playlist_item_current_1, play_list_items,
                         last_playlist_item, 4);

          float new_width = 300;

          ImGui::PushItemWidth(new_width);
          ImGui::SameLine();
          ImGui::ListBox("   ", &selected_list, current_play_list,
                         total_playlists, 4);
          ImGui::PopItemWidth();

          if (ImGui::Button("Start Playing")) {

            if (strlen(str0) > 0) {

              struct Media_Object media_object;
              media_object.file_url_name = new char[strlen(str0)];
              strcpy(media_object.file_url_name, str0);
              media_object.is_web_file = 0;
              if (boost::algorithm::contains(str0, "youtube")) {
                struct download_task temp_task;
                strcpy(temp_task.download_url, str0);

                temp_task.is_playerble = 1;
                media_object.is_web_file = 1;
                strcpy(temp_task.file_name, "");
                strcpy(temp_task.cookies, "");
                strcpy(temp_task.user_agent, "");
                strcpy(temp_task.save_loc, "");
                temp_task.connections = 4;

                active_downloads.jobs.push_back(temp_task);
                str0[0] = '\0';
              } else {
                play_list_lock.lock();

                for (auto &iter : selected)
                  for (auto &stream : iter.second.play_list)
                    stream.is_playing = false;

                media_object.name = media_object.file_url_name;

                selected[0].play_list.push_back(media_object);
                ui_setting.playback_id =
                    selected[selected_list].play_list.size();
                media_object.is_playing = 1;
                play_list_lock.unlock();
                start_playback(media_object);
                str0[0] = '\0';
                old_selected_list = selected_list;
              }
            } else {
              play_list_lock.lock();
              for (auto &iter : selected)
                for (auto &stream : iter.second.play_list)
                  stream.is_playing = false;
              selected[selected_list]
                  .play_list[playlist_item_current_1]
                  .is_playing = true;
              ui_setting.playback_id = playlist_item_current_1;
              start_playback(
                  selected[selected_list].play_list[playlist_item_current_1]);
              old_selected_list = selected_list;
              play_list_lock.unlock();
            }
          }
          ImGui::SameLine();
          if (ImGui::Button("+")) {
            increase_volume();
          }
          ImGui::SameLine();
          if (ImGui::Button("-")) {
            decrease_volume();
          }
          ImGui::SameLine();
          if (ImGui::Button("mute")) {
            mute_playback();
          }
          ImGui::SameLine();
          if (ImGui::Button("Pause")) {
            pause_playback();
          }
          ImGui::SameLine();
          if (ImGui::Button("Stop")) {
            stop_playback();
          }
          ImGui::SameLine();
          if (ImGui::Button("CHP+")) {
            next_chapter();
          }
          ImGui::SameLine();
          if (ImGui::Button("CHP-")) {
            previous_chapter();
          }
          ImGui::SameLine();
          if (ImGui::Button("BWD<<")) {
            forward_playback();
          }
          ImGui::SameLine();
          if (ImGui::Button(">>FWD")) {
            rewind_playback();
          }

          if (ImGui::Button("AUD+N")) {
            next_audio_channel();
          }
          ImGui::SameLine();
          if (ImGui::Button("SUB>>")) {
            next_subtitle();
          }

          ImGui::SameLine();
          if (ImGui::Button("PREV"))
            play_next_media(2);

          ImGui::SameLine();
          if (ImGui::Button("NEXT"))
            play_next_media(1);

          ImGui::SameLine();
          if (ImGui::Button("[X]")) {
            stop_playback();
            play_list_lock.lock();
            selected[selected_list].play_list.clear();
            ui_setting.playback_id = 0;
            play_list_lock.unlock();
            if (play_list_items != 0) {
              delete[] play_list_items;
            }
            play_list_items = 0;
            last_playlist_item = 0;
          }

          ImGui::Checkbox("Repeat", &repeat);

          ImGui::SameLine();
          ImGui::Checkbox("InfBuff", &infinite_buffer);
          ImGui::SameLine();
          ImGui::Checkbox("hide", &enable_hide);

          SDL_LockMutex(ui_setting.mutex);

          if (infinite_buffer) {
            ui_setting.infinite_buffer = 1;
          } else
            ui_setting.infinite_buffer = 0;
          double abuff = ui_setting.audio_buffer_size;
          double vbuff = ui_setting.video_buffer_size;

          double total_seek_value = ui_setting.total_seek_value;
          SDL_UnlockMutex(ui_setting.mutex);

          if (abuff >= last_max_abuff) {
            last_max_abuff = abuff;
          }
          if (vbuff >= last_max_vbuff) {
            last_max_vbuff = vbuff;
          }
          ImGui::Text("VideoBuffer");
          ImGui::SameLine();
          ImGui::ProgressBar(vbuff / last_max_vbuff,
                             ImVec2(mWindow->getWidth() - 400, 12.2f));
          ImGui::SameLine();
          ImGui::Text(std::string(std::to_string((int)vbuff) + "/" +
                                  std::to_string((int)last_max_vbuff))
                          .c_str());

          ImGui::Text("AudioBuffer");
          ImGui::SameLine();
          ImGui::ProgressBar(abuff / last_max_abuff,
                             ImVec2(mWindow->getWidth() - 400, 12.2f));
          ImGui::SameLine();
          ImGui::Text(std::string(std::to_string((int)abuff) + "/" +
                                  std::to_string((int)last_max_abuff))
                          .c_str());
          SDL_LockMutex(ui_setting.mutex);
          float seek_value = ui_setting.current_video_time;
          player_time = time_string((int)seek_value);
          std::string total_time =
              player_time + "/" + time_string((int)ui_setting.total_seek_value);
          if (ImGui::SliderFloat(total_time.c_str(), &seek_value, 0.0f,
                                 total_seek_value, player_time.c_str())) {
            ui_setting.skip_to_video_time = seek_value;
            ui_setting.seek_requested = 1;
          }
          SDL_UnlockMutex(ui_setting.mutex);
        }
      }
      ImGui::EndDock();
    }
    if (0) {
      if (ImGui::BeginDock("BETTING")) {
               miniScreenNode->setVisible(false);
        cefScreenNode->setVisible(true);
        ImGui::Text("Nothing here! Ooops");
        request_login();
      }
      ImGui::EndDock();
    }
    if (0) {

      if (ImGui::BeginDock("Platonic A/V")) {
                miniScreenNode->setVisible(false);
        cefScreenNode->setVisible(false);
        ImGui::Text("Nothing here! Ooops");
      }
      ImGui::EndDock();
    }
    if (1) {
      
      if (ImGui::BeginDock("Transfers")) {
          miniScreenNode->setVisible(true);
        cefScreenNode->setVisible(false);
        show_up_down_load_manager();
      }
      ImGui::EndDock();
    }
      if (1) {
      if (ImGui::BeginDock("About")) {
       miniScreenNode->setVisible(true);
        cefScreenNode->setVisible(false);
       ImGui::Text("Neon");
        ImGui::Text("      Version:");ImGui::SameLine();ImGui::Text(PROJECT_VERSION);
        ImGui::Text("      BuildID:");ImGui::SameLine();ImGui::Text(GIT_HEAD_SHA1);
        ImGui::Text("         Date:");ImGui::SameLine();ImGui::Text(COMMIT_DATE);
        ImGui::Text("Commit Author:");ImGui::SameLine();ImGui::Text(COMMIT_AUTHOR);
        ImGui::Text("CommitMessage:");ImGui::SameLine();ImGui::Text(COMMIT_MESSAGE);
        ImGui::Text("    Copyright:");ImGui::SameLine();ImGui::Text("Copyright © 2019 by NeonIntelligence Inc.\n\
All rights reserved. Since 2017.");
        ImGui::Text("         Site: www.neonintelligence.com");
        
      }
      ImGui::EndDock();
    }
    ImGui::EndDockspace();
  }
  ImGui::End();
}

bool ImguiExample::frameStarted(const Ogre::FrameEvent &evt) {

  active_timers[0].start_time_frq = StartCounter();
  active_timers[0].name = "Main program clock";
  active_timers[0].timer_type = 1;
  active_timers[0].global_update = true;
  OgreBites::ApplicationContext::frameStarted(evt);

  Ogre::ImguiManager::getSingleton().newFrame(
      evt.timeSinceLastFrame,
      Ogre::Rect(0, 0, mWindow->getWidth(), mWindow->getHeight()));
  ImGui::SetNextWindowBgAlpha(gui_transparancy);
  bool show_dock_window = true;
  ImGui::SetNextWindowSize(ImVec2(mWindow->getWidth(), mWindow->getHeight()),
                           ImGuiSetCond_Always);
  ImGui::SetNextWindowPos(ImVec2(.0f, .0f), ImGuiSetCond_Always);
  if (selected_theme == 0)
    ImGui::StyleColorsClassic();
  else if (selected_theme == 1)
    ImGui::StyleColorsDark();
  else if (selected_theme == 2)
    ImGui::StyleColorsLight();
  else
    ;
  update();
  update_timers();
  return true;
}
void ImguiExample::update_timers() {
  for (auto &clock_timer : active_timers) {
    if (clock_timer.second.global_update) {
      if (clock_timer.second.timer_type == 0)
        clock_timer.second.total_elapsed = boost::chrono::nanoseconds(
            clock_timer.second.start_time.elapsed().user +
            clock_timer.second.start_time.elapsed().system);
      else
        clock_timer.second.total_elapsed_frq =
            (GetCounter(clock_timer.second.start_time_frq));
    }
  }
}
bool ImguiExample::frameEnded(const Ogre::FrameEvent &evt) {
  SDL_LockMutex(ui_setting.mutex);
  if (repeat) {
    ui_setting.repeat = 1;
  } else
    ui_setting.repeat = 0;

  if (player)
    if (ui_setting.stop_play == 2) {

      if (ui_setting.repeat == 1) {
        set_texture_color("video_material", "FFmpegVideoTexture", 1920, 800);
        Ogre::MaterialPtr material =
            Ogre::MaterialManager::getSingleton().getByName("video_material");
        mMiniScreen->setMaterial(material);
        play_next_media(3);
      } else if (ui_setting.repeat == 2) {
        set_texture_color("video_material", "FFmpegVideoTexture", 1920, 800);
        Ogre::MaterialPtr material =
            Ogre::MaterialManager::getSingleton().getByName("video_material");
        mMiniScreen->setMaterial(material);
        play_next_media(1);
      } else {
        set_texture_color("video_material", "FFmpegVideoTexture", 1920, 800);
        Ogre::MaterialPtr material =
            Ogre::MaterialManager::getSingleton().getByName("video_material");
        mMiniScreen->setMaterial(material);
        play_next_media(1);
      }
    }
  SDL_UnlockMutex(ui_setting.mutex);

  active_timers[0].total_elapsed_frq =
      GetCounter(active_timers[0].start_time_frq);

  // std::cout<<"ELAPSED:"<<elapsed<<std::endl;
  double refresh_r = refresh_rate.load();
  if (refresh_r - active_timers[0].total_elapsed_frq > 1)
     #ifdef USE_BOOST_THREAD
       boost::this_thread::sleep_for(boost::chrono::milliseconds((int)refresh_r));
    #else
              Sleep(refresh_r);
    #endif
  return OgreBites::ApplicationContext::frameEnded(evt);
}

void ImguiExample::windowResized	(	Ogre::RenderWindow * 	window	)	
{
      /*bind_material_texture("browser_material", "browser_texture",
                           window->getWidth(),window->getHeight());*/
   return  OgreBites::ApplicationContext::windowResized(window);
}


bool ImguiExample::frameRenderingQueued(const Ogre::FrameEvent &evt) {

  if (!mWindow->isActive()) {
    if (mWindow->isClosed()) {
      reset.store(true);
      is_quit.store(true);
      return false;
    } else {

#if defined(WIN32) || defined(_WIN32) ||                                       \
    defined(__WIN32) && !defined(__CYGWIN__)
      while (!mWindow->isActive()) {
        OgreBites::WindowEventUtilities::messagePump();
        Sleep(300);
        mWindow->update(true);
      }
#else
      while (!mWindow->isActive()) {
        OgreBites::WindowEventUtilities::messagePump();
        usleep(1000 * 1000);
        mWindow->update(true);
      }
#endif
    }
  }

  refresh_video_frame();
  return OgreBites::ApplicationContext::frameRenderingQueued(evt);
}
void ImguiExample::request_login() {
  if (device_login.login_state == 1)
    if (ImGui::BeginPopupModal("Login Required!")) {
      ImGui::Text(
          "Hello dear user!\nThis is a subscription based feature", 
          std::string("\nPlease open " + device_login.login_page).c_str(),
          std::string("Please enter the following code the page\n\n").c_str(),
          device_login.login_code.c_str(), 
          "This  window will automatically close.");
    }
  ImGui::EndPopup();
}

void ImguiExample::setup() {

  // video_play=new Player();
  player = nullptr;
  ogre_ff_texture.is_ready = 1;
  ogre_ff_texture.sub_is_ready = 1;
  ogre_ff_texture.mutex = SDL_CreateMutex();
  ui_setting.mutex = SDL_CreateMutex();

  ui_setting.stop_play = 0;
  ui_setting.force_refresh = 0;
  ui_setting.pause = 0;
  ui_setting.mute = 0;
  ui_setting.skip_n_frame = 0;
  ui_setting.next_audio_stream = 0;
  ui_setting.next_video_stream = 0;
  ui_setting.next_subtitle_stream = 0;
  ui_setting.next_all_streams = 0;
  ui_setting.display_av_graph = 0;
  ui_setting.next_chapter = 0;
  ui_setting.previous_chapter = 0;
  ui_setting.fast_forward = 0;
  ui_setting.fast_backward = 0;
  ui_setting.increase_volume = 0;
  ui_setting.decrease_volume = 0;
  ui_setting.startup_volume = 30;
  ui_setting.seek_requested = 0;
  ui_setting.skip_to_video_time = 0;
  ui_setting.current_video_time = 0;
  ui_setting.skip_to_video_time = 0;
  ui_setting.total_seek_value = 0;

  ui_setting.infinite_buffer = 0;
  ui_setting.playback_id = 0;
  ui_setting.play_all = 1;
  ui_setting.repeat = 0;
  ui_setting.video_buffer_size = 0;
  ui_setting.audio_buffer_size = 0;
  ui_setting.network_initialized = 0;
  ui_setting.Video_title = "";

  OgreBites::ApplicationContext::setup();
  addInputListener(this);

  Ogre::ImguiManager::createSingleton();
  addInputListener(Ogre::ImguiManager::getSingletonPtr());
  ImGui::InitDock();
  // get a pointer to the already created root
  root = getRoot();
  scnMgr = root->createSceneManager();
  Ogre::ImguiManager::getSingleton().init(scnMgr);

  // register our scene with the RTSS
  Ogre::RTShader::ShaderGenerator *shadergen =
      Ogre::RTShader::ShaderGenerator::getSingletonPtr();
  shadergen->addSceneManager(scnMgr);

  Ogre::Light *light = scnMgr->createLight("MainLight");
  Ogre::SceneNode *lightNode =
      scnMgr->getRootSceneNode()->createChildSceneNode();
  lightNode->setPosition(0, 10, 15);
  lightNode->attachObject(light);

  Ogre::SceneNode *camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
  camNode->setPosition(0, 0, 15);
  camNode->lookAt(Ogre::Vector3(0, 0, -1), Ogre::Node::TS_PARENT);
  


  set_texture_color("video_material", "FFmpegVideoTexture", 1920, 800);
  set_texture_color("browser_material", "browser_texture",800, 600);
  
  mMiniScreen = new Ogre::Rectangle2D(true);
  mMiniScreen->setCorners(-1.0, 1.0, 1.0, -1.0);
  mMiniScreen->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
   mMiniScreen->setVisible(false);

  cefScreen= new Ogre::Rectangle2D(true);
  cefScreen->setCorners(-1.0, 1.0, 1.0, -1.0);
  cefScreen->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);

  mMiniScreen->setMaterial(Ogre::MaterialManager::getSingleton().getByName("video_material"));
  cefScreen->setMaterial(Ogre::MaterialManager::getSingleton().getByName("browser_material"));


  miniScreenNode = scnMgr->getRootSceneNode()->createChildSceneNode();
  cefScreenNode = scnMgr->getRootSceneNode()->createChildSceneNode();

  miniScreenNode->attachObject(mMiniScreen);
  cefScreenNode->attachObject(cefScreen);

  Ogre::Camera *cam = scnMgr->createCamera("myCam");
  cam->setNearClipDistance(5); // specific to this sample
  cam->setAutoAspectRatio(true);
  camNode->attachObject(cam);
  mWindow = getRenderWindow();
  mWindow->addViewport(cam);
  ogre_ff_texture.coded_h = mWindow->getHeight();
  ogre_ff_texture.coded_w = mWindow->getWidth();
  system_ready.store(true);
     Ogre::TexturePtr renderTexture =
        Ogre::TextureManager::getSingleton().getByName("browser_texture");
    {
        renderHandler = new RenderHandler(renderTexture,mWindow);

        root->addFrameListener(renderHandler);
    }
    {
        CefWindowInfo window_info;
        CefBrowserSettings browserSettings;

        browserSettings.windowless_frame_rate = 60; // 30 is default

        // in linux set a gtk widget, in windows a hwnd. If not available set nullptr - may cause some render errors, in context-menu and plugins.
       CefWindowHandle windowHandle;
       #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
          windowHandle=(HWND)mWindow->getCustomAttribute("WINDOW");
       #else
          windowHandle=(int)mWindow->getCustomAttribute("WINDOW");
        #endif
        window_info.SetAsWindowless(windowHandle); // false means no transparency (site background colour)

        browserClient = new BrowserClient(renderHandler);

        browser = CefBrowserHost::CreateBrowserSync(window_info, browserClient.get(), "http://127.0.0.1:5500/awesome_progress.html", browserSettings, nullptr);
         CefRefPtr<CefFrame> frame = browser->GetMainFrame();
         frame->ExecuteJavaScript("window.resizeTo(600,800);", frame->GetURL(), 0);
        // frame->LoadURL("https://www.google.com/");
        // inject user-input by calling - non-trivial for non-windows - checkout the cefclient source and the platform specific cpp, like cefclient_osr_widget_gtk.cpp for linux
        // browser->GetHost()->SendKeyEvent(...);
        // browser->GetHost()->SendMouseMoveEvent(...);
        // browser->GetHost()->SendMouseClickEvent(...);
        // browser->GetHost()->SendMouseWheelEvent(...);
    }  
  

  struct Media_Object media_file;
  media_file.file_url_name = "http://149.56.74.125:9508/stream";
  media_file.name = "Japanese rock";
  media_file.is_playing = 0;

  play_list_lock.lock();
  selected[1].name = "Internet Streams";
  selected[0].name = "Files";
  selected[1].play_list.push_back(media_file);

  media_file.file_url_name = "http://23.29.71.154:8140/stream";
  media_file.name = "Modern metal Rock";
  selected[1].play_list.push_back(media_file);

  media_file.file_url_name = "http://199.180.75.2:9315/";
  media_file.name = "Learn Japanese";
  selected[1].play_list.push_back(media_file);

  media_file.file_url_name = "http://142.4.217.133:9848/stream";
  media_file.name = "Vocaloids";
  selected[1].play_list.push_back(media_file);

  media_file.file_url_name = "http://curiosity.shoutca.st:8019/stream";
  media_file.name = "Vocaloids 2";
  selected[1].play_list.push_back(media_file);


  media_file.file_url_name = "http://uk6.internet-radio.com:8424/";
  media_file.name = "UK-AT Radio";
  selected[1].play_list.push_back(media_file);
  play_list_lock.unlock();
  start_time = boost::posix_time::microsec_clock::local_time();
  /// Ogre::Entity* ent = scnMgr->createEntity("Sinbad.mesh");
  // Ogre::SceneNode* node =
  // scnMgr->getRootSceneNode()->createChildSceneNode();
  //  node->attachObject(ent);
  boost::timer::cpu_timer timer;
  active_timers[1].start_time = timer;
  active_timers[1].name = "Mouse events";
  active_timers[1].global_update = true;
  active_timers[1].timer_type = 0;

  active_timers[2].start_time = timer;
  active_timers[2].name = "Keyboard events";
  active_timers[2].global_update = true;
  active_timers[2].timer_type = 0;
}
bool ImguiExample::keyPressed(const OgreBites::KeyboardEvent &evt) {
  if (evt.keysym.sym == 27) {
    root->queueEndRendering();
  }
  if (evt.keysym.sym == 32) { // space key
    pause_playback();
  }
  if (evt.keysym.sym == 1073741903) { // right button
    rewind_playback();
  }
  if (evt.keysym.sym == 1073741904) { // left button
    forward_playback();
  }
  if (evt.keysym.sym == 1073741906) { // up button
    play_next_media(2);
  }
  if (evt.keysym.sym == 1073741905) { // down button
    play_next_media(1);
  }
  if (evt.keysym.sym == 109) { // m button
    mute_playback();
  }
  if (evt.keysym.sym == 110) { // n button
    next_audio_channel();
  }
  if (evt.keysym.sym == 61) { //+ button
    increase_volume();
  }
  if (evt.keysym.sym == 45) { //+ button
    decrease_volume();
  }
  if (evt.keysym.sym == 104) { // h button
    if (enable_hide)
      enable_hide = false;
    else
      enable_hide = true;
  }
  if (evt.keysym.sym == 114) { // r button
    if (repeat)
      repeat = false;
    else
      repeat = true;
  }
  if (evt.keysym.sym == 105) { // i button
    if (infinite_buffer)
      infinite_buffer = false;
    else
      infinite_buffer = true;
  }
  boost::timer::cpu_timer timer;
  active_timers[2].start_time = timer;
  pressed_key = evt.keysym.sym;
  return true;
}
bool ImguiExample::mouseMoved(const OgreBites::MouseMotionEvent &evt) {
  boost::timer::cpu_timer timer;
  active_timers[1].start_time = timer;
  return true;
}
bool ImguiExample::mousePressed(const OgreBites::MouseButtonEvent &arg) {
  boost::timer::cpu_timer timer;
  active_timers[1].start_time = timer;
  return true;
}
void ImguiExample::ClearLog() {
  for (int i = 0; i < Items.Size; i++)
    free(Items[i]);
  Items.clear();
  ScrollToBottom = true;
}
void ImguiExample::show_console() {
  ImGui::Separator();
  ImGui::Text("Console Log");
  ImGui::Separator();
  static ImGuiTextFilter filter;
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                      ImVec2(4, 1)); // Tighten spacing

  if (copy_to_clipboard)
    ImGui::LogToClipboard();

  ImVec4 col_default_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);
  for (int i = 0; i < Items.Size; i++) {
    const char *item = Items[i];
    if (!filter.PassFilter(item))
      continue;
    ImVec4 col = col_default_text;
    if (strstr(item, "[error]"))
      col = ImColor(1.0f, 0.4f, 0.4f, 1.0f);
    else if (strncmp(item, "# ", 2) == 0)
      col = ImColor(1.0f, 0.78f, 0.58f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, col);
    ImGui::TextUnformatted(item);
    ImGui::PopStyleColor();
  }
  if (copy_to_clipboard)
    ImGui::LogFinish();
  if (ScrollToBottom)
    ImGui::SetScrollHere();
  ScrollToBottom = false;
  ImGui::PopStyleVar();
  ImGui::Separator();
}
void ImguiExample::refresh_video_frame(void) {
  SDL_LockMutex(ogre_ff_texture.mutex);
  if (ogre_ff_texture.is_ready == 2) {
    if (ogre_ff_texture.update_texture == 1) {
      hide_is_video = false;
      bind_material_texture("video_material", "FFmpegVideoTexture",
                            ogre_ff_texture.coded_w, ogre_ff_texture.coded_h);
      /*
      void MyWidget::resizeEvent(QResizeEvent *event)
{
   static const float ratioY2X = (float) HEIGHT / (float) WIDTH;

   // QResizeEvent type has oldSize() and size()
   // and size() has an actual new size for the widget
   // so we can just take it and apply the ratio.

   // first try to detect the direction of the widget resize
   if (event->oldSize().width() != event->size().width())
      this->resize(event->size().width(),             // apply the correct
ratio event->size().width() * ratioY2X); // to either of width/height else
this->resize(event->size().height() / ratioY2X, // apply the correct ratio
event->size().height());           // to either of width/height
   event->accept(); // we've finished processing resize event here
}
      */
     #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
      HWND handle = ::FindWindow(NULL, L"Cloud_Commander_UI");
      if (ogre_ff_texture.coded_h < 360) {

        ogre_ff_texture.coded_h = 360;
        ogre_ff_texture.coded_w = 640;
      }
      ::SetWindowPos(handle, HWND_TOPMOST, 0, 0, ogre_ff_texture.coded_w,
                     ogre_ff_texture.coded_h,
                     SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOMOVE |
                         SWP_NOZORDER);
      #endif
      Ogre::MaterialPtr material =
          Ogre::MaterialManager::getSingleton().getByName("video_material");
      mMiniScreen->setMaterial(material);
      ogre_ff_texture.update_texture = 0;
    }
    hide_is_video = true;
    Ogre::TexturePtr texture =
        Ogre::TextureManager::getSingleton().getByName("FFmpegVideoTexture");

     unsigned char *texData = (unsigned char *)texture->getBuffer()->lock(
        Ogre::HardwareBuffer::HBL_DISCARD);

   memcpy(texData, ogre_ff_texture.dst_data[0], ogre_ff_texture.buffer_size);
    av_freep(&ogre_ff_texture.dst_data);
    texture->getBuffer()->unlock();/*
        Ogre::HardwarePixelBufferSharedPtr texBuf = texture->getBuffer();
        texBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        memcpy(texBuf->getCurrentLock().data, ogre_ff_texture.dst_data, ogre_ff_texture.buffer_size);
        texBuf->unlock();
*/


    ogre_ff_texture.is_ready = 1;
  }
  SDL_UnlockMutex(ogre_ff_texture.mutex);
}
void ImguiExample::show_up_down_load_manager(void) {

  struct rate {
    char *symbol;
    int Bps; /* bytes per second */
  };
  struct rate rate[] = {{"Mbps", 1000000},      {"kbps", 1000},
                        {"MiB/s", 1024 * 1024}, {"KiB/s", 1024},
                        {"kB/s", 1000},         {"B/s", 1}};
  current_time = boost::posix_time::microsec_clock::local_time();
  boost::posix_time::time_duration diff = current_time - start_time;

  float reduce_by = 300;
  bool is_reset = reset.load();

  if (is_reset) {

    active_connections.clear();

    active_ul_connections.clear();

    reset.store(false);
  }
  ImGui::Text("ENTER DOWNLOAD LINK");
  // static char buf[32] = u8"NIHONGO"; // <- this is how you would write it
  // with C++11, using real kanjis
  ImGui::Text("URL");
  ImGui::SameLine();

  ImGui::InputText("|", temp_task.download_url,
                   IM_ARRAYSIZE(temp_task.download_url));
  ImGui::SameLine();
  ImGui::Text("FILE");
  ImGui::SameLine();
  temp_task.connections = 4;

  ImGui::InputText("||", temp_task.file_name,
                   IM_ARRAYSIZE(temp_task.file_name));
  if (dl_clicked == 0)
    if (ImGui::Button("Download")) {
      active_downloads.url_downloading = false;
      active_downloads.cancel = false;
      temp_task.is_playerble = 0;
      active_downloads.jobs.push_back(temp_task);
      dl_clicked = 1;
    }

  if (dl_clicked == 1) {
    if (ImGui::Button("Cancel")) {

      active_downloads.url_downloading = false;
      active_downloads.cancel = true;
      dl_clicked = 0;
    }
  }
  ImGui::Separator();
  ImGui::Text("Active Connections");

  {
    double downloaded = 0;
    double uploaded = 0;
    double total = 0;
    double main_progress = 0;
    double main_ul_progress = 0;
    int download_speed = 0;
    int upload_speed = 0;
    static int display_count = 150;
    double graph_rt = 0;

    for (auto &it : active_connections) {

      // Typically we would use ImVec2(-1.0f,0.0f) to use all available width,
      // or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses
      // ItemWidth.
      std::string conexion;
      if (it.first < 10)
        conexion = std::string("0") + std::to_string(it.first);
      else
        conexion = std::to_string(it.first);
      ImGui::Text(std::string("Connection #" + conexion).c_str());
      ImGui::SameLine();
      ImGui::ProgressBar((double)it.second.load().percentage / (double)100,
                         ImVec2(mWindow->getWidth() - reduce_by, 12.2f));
      ImGui::SameLine();
      if (it.second.load().speed < 1000)
        current_rate = 5;
      else if (it.second.load().speed < 1000 * 1000)
        current_rate = 1;
      else
        current_rate = 0;

      ImGui::Text(std::string(std::to_string(it.second.load().speed /
                                             rate[current_rate].Bps) +
                              std::string(rate[current_rate].symbol))
                      .c_str());
      ImGui::Separator();
      downloaded = it.second.load().percentage + downloaded;
      total = total + 100;
      download_speed =
          download_speed + it.second.load().speed / rate[current_rate].Bps;
      graph_rt += it.second.load().speed / rate[2].Bps;
    }

    if (graph_rt > max_value)
      max_value = graph_rt;

    if (last_dlspeed == 0)
      last_dlspeed = graph_rt;

    if (current_log > IM_ARRAYSIZE(download_log))
      current_log = 0;

    if (last_ulspeed == 0)
      last_ulspeed = graph_rt;

    if (current_ul_log > IM_ARRAYSIZE(upload_log))
      current_ul_log = 0;

    if (diff.total_milliseconds() >= 1000) {
      download_log[current_log] = graph_rt;
      last_dlspeed = graph_rt;
      start_time = boost::posix_time::microsec_clock::local_time();
      current_log++;

      if (current_connection_info.load().transfer_not_started == 1)
        current_ul_log = 0;
      upload_log[current_ul_log] = graph_rt;
      last_ulspeed = graph_rt;
      start_time = boost::posix_time::microsec_clock::local_time();
      current_ul_log++;
      float temp = upload_log[0];
      upload_log[0] = upload_log[IM_ARRAYSIZE(upload_log)];
      upload_log[IM_ARRAYSIZE(upload_log)] = temp;
    }

    main_progress = downloaded / total;

    double current_percent = 0;
    if (active_connections.size() <= 0) {
      ImGui::Text("No active downloads");
    }
    ////////////////////////////////////////////////////////////////////////////////////////

    ImGui::Separator();
    ImGui::Text("Active Operations");

    for (auto &it : active_ul_connections) {

      // Typically we would use ImVec2(-1.0f,0.0f) to use all available width,
      // or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses
      // ItemWidth.
      std::string conexion;
      if (it.first < 10)
        conexion = std::string("0") + std::to_string(it.first);
      else
        conexion = std::to_string(it.first);
      if (it.second.load().con_type == 1)
        ImGui::Text(std::string("Uploading #" + conexion).c_str());
      else if (it.second.load().con_type == 2)
        ImGui::Text(std::string("Encryption #" + conexion).c_str());
      else if (it.second.load().con_type == 3)
        ImGui::Text(std::string("Decryption #" + conexion).c_str());
      else if (it.second.load().con_type == 4)
        ImGui::Text(std::string("Hashing #" + conexion).c_str());
      else if (it.second.load().con_type == 5)
        ImGui::Text(std::string("Merging #" + conexion).c_str());
      ImGui::SameLine();
      current_percent = it.second.load().percentage;
      ImGui::ProgressBar((double)it.second.load().percentage / (double)100,
                         ImVec2(mWindow->getWidth() - reduce_by - 100, 18.2f));
      ImGui::SameLine();
      if (it.second.load().speed < 1000)
        current_rate = 5;
      else if (it.second.load().speed < 1000 * 1000)
        current_rate = 1;
      else
        current_rate = 0;

      ImGui::Text(std::string(std::to_string(it.second.load().speed /
                                             rate[current_rate].Bps) +
                              std::string(rate[current_rate].symbol))
                      .c_str());
      ImGui::Separator();
      upload_speed = it.second.load().speed / rate[current_rate].Bps;
      graph_rt = it.second.load().speed / rate[2].Bps;
    }
    if (active_ul_connections.size() <= 0) {
      ImGui::Text("No ongoing operations");
    }

    ImGui::Separator();

    ImGui::Text("DL|UL");
    ImGui::PushItemWidth(mWindow->getWidth() / 2 - 10);
    ImGui::PlotHistogram("", download_log, IM_ARRAYSIZE(download_log),
                         current_log,
                         std::string("speed " + std::to_string(download_speed) +
                                     std::string(rate[current_rate].symbol))
                             .c_str(),
                         0.0f, max_value, ImVec2(0, 80));
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(mWindow->getWidth() / 2 - 10);
    ImGui::PlotHistogram(" ", upload_log, IM_ARRAYSIZE(upload_log),
                         current_ul_log,
                         std::string("speed " + std::to_string(upload_speed) +
                                     std::string(rate[current_rate].symbol))
                             .c_str(),
                         0.0f, max_ul_value, ImVec2(0, 80));
    ImGui::PopItemWidth();

    main_ul_progress = uploaded / total;

    ImGui::Separator();
    ImGui::Text("Total Upload");
    ImGui::ProgressBar(
        (double)current_connection_info.load().upload_percentage / (double)100,
        ImVec2(mWindow->getWidth() - reduce_by + 200, 0.0f));
    ImGui::Separator();
    ImGui::Text("Total Download");
    ImGui::ProgressBar(main_progress,
                       ImVec2(mWindow->getWidth() - reduce_by + 200, 0.0f));
    ImGui::SameLine();
    ImGui::Text(std::string(std::to_string(download_speed) +
                            std::string(rate[current_rate].symbol))
                    .c_str());

    show_console();
  }
  /*
for(auto i=1;i<10;i++){
static float progress_dir = 1.0f;

progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f;
} if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
// Typically we would use ImVec2(-1.0f,0.0f) to use all
available width, or ImVec2(width,0.0f) for a specified width.
ImVec2(0.0f,0.0f) uses ItemWidth.
ImGui::Text(std::string(std::string("Connection
#")+std::to_string(i)).c_str()); ImGui::SameLine();
ImGui::ProgressBar(progress, ImVec2(0.0f,12.2f));
ImGui::SameLine();
ImGui::Text(std::string(std::to_string(i*10)+std::string("
KBS/S")).c_str());
}

ImGui::Separator();
ImGui::Text("Total Download");
ImGui::ProgressBar(progress, ImVec2(-1.0f,0.0f));
show_console();
ImGui::End();
*/
}
bool ImguiExample::BufferingBar(const char *label, float value,
                                const ImVec2 &size_arg, const ImU32 &bg_col,
                                const ImU32 &fg_col) {
  ImGuiWindow *window = ImGui::GetCurrentWindow();
  if (window->SkipItems)
    return false;

  ImGuiContext &g = *GImGui;
  const ImGuiStyle &style = g.Style;
  const ImGuiID id = window->GetID(label);

  ImVec2 pos = window->DC.CursorPos;
  ImVec2 size = size_arg;
  size.x -= style.FramePadding.x * 2;

  const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
  ImGui::ItemSize(bb, style.FramePadding.y);
  if (!ImGui::ItemAdd(bb, id))
    return false;

  // Render
  const float circleStart = size.x * 0.7f;
  const float circleEnd = size.x;
  const float circleWidth = circleEnd - circleStart;

  window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y),
                                  bg_col);
  window->DrawList->AddRectFilled(
      bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

  const float t = g.Time;
  const float r = size.y / 2;
  const float speed = 1.5f;

  const float a = speed * 0;
  const float b = speed * 0.333f;
  const float c = speed * 0.666f;

  const float o1 =
      (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
  const float o2 =
      (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
  const float o3 =
      (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

  window->DrawList->AddCircleFilled(
      ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
  window->DrawList->AddCircleFilled(
      ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
  window->DrawList->AddCircleFilled(
      ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
}
bool ImguiExample::Spinner(const char *label, float radius, int thickness,
                           const ImU32 &color) {
  ImGuiWindow *window = ImGui::GetCurrentWindow();
  if (window->SkipItems)
    return false;

  ImGuiContext &g = *GImGui;
  const ImGuiStyle &style = g.Style;
  const ImGuiID id = window->GetID(label);

  ImVec2 pos = window->DC.CursorPos;
  ImVec2 size((radius)*2, (radius + style.FramePadding.y) * 2);

  const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
  ImGui::ItemSize(bb, style.FramePadding.y);
  if (!ImGui::ItemAdd(bb, id))
    return false;

  // Render
  window->DrawList->PathClear();

  int num_segments = 30;
  int start = 0;

  const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
  const float a_max =
      IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

  const ImVec2 centre =
      ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

  for (int i = 0; i < num_segments; i++) {
    const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
    window->DrawList->PathLineTo(
        ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
               centre.y + ImSin(a + g.Time * 8) * radius));
  }

  window->DrawList->PathStroke(color, false, thickness);
}
void ImguiExample::start_playback(struct Media_Object media_object) {
  update_scroll = true;
  hide_is_video = false;
  if (!player) {
    last_max_abuff = 0;
    last_max_vbuff = 0;
    ui_setting.audio_buffer_size = 0;
    ui_setting.video_buffer_size = 0;
    ui_setting.stop_play = 0;
    #ifdef USE_BOOST_THREAD
      player = new boost::thread(start_player, media_object);
    #else
      player = new std::thread(start_player, media_object);
    #endif
  } else {

    SDL_LockMutex(ui_setting.mutex);
    ui_setting.stop_play = 1;
    SDL_UnlockMutex(ui_setting.mutex);
    player->join();
    delete player;
    player = nullptr;
    ui_setting.stop_play = 0;
    last_max_abuff = 0;
    last_max_vbuff = 0;
    ui_setting.audio_buffer_size = 0;
    ui_setting.video_buffer_size = 0;
    #ifdef USE_BOOST_THREAD
    player = new boost::thread(start_player, media_object);
    #else
      player = new std::thread(start_player, media_object);
    #endif
  }
}
void ImguiExample::stop_playback() {
  SDL_LockMutex(ui_setting.mutex);
  ui_setting.stop_play = 1;
  SDL_UnlockMutex(ui_setting.mutex);
  if (player) {
    player->join();
    delete player;
    player = nullptr;
  }
  ui_setting.stop_play = 0;
  last_max_abuff = 0;
  last_max_vbuff = 0;
  ui_setting.audio_buffer_size = 0;
  ui_setting.video_buffer_size = 0;
}
void ImguiExample::increase_volume() {
  SDL_LockMutex(ui_setting.mutex);
  ui_setting.increase_volume += 1;
  SDL_UnlockMutex(ui_setting.mutex);
}
void ImguiExample::decrease_volume() {
  SDL_LockMutex(ui_setting.mutex);
  ui_setting.decrease_volume += 1;
  SDL_UnlockMutex(ui_setting.mutex);
}
void ImguiExample::mute_playback() {
  SDL_LockMutex(ui_setting.mutex);
  ui_setting.mute = 1;
  SDL_UnlockMutex(ui_setting.mutex);
}
void ImguiExample::pause_playback() {
  SDL_LockMutex(ui_setting.mutex);
  ui_setting.pause = 1;
  SDL_UnlockMutex(ui_setting.mutex);
}
void ImguiExample::next_chapter() {
  SDL_LockMutex(ui_setting.mutex);
  ui_setting.next_chapter += 1;
  SDL_UnlockMutex(ui_setting.mutex);
}
void ImguiExample::previous_chapter() {
  SDL_LockMutex(ui_setting.mutex);
  ui_setting.previous_chapter += 1;
  SDL_UnlockMutex(ui_setting.mutex);
}
void ImguiExample::forward_playback() {
  SDL_LockMutex(ui_setting.mutex);
  ui_setting.fast_backward += 1;
  SDL_UnlockMutex(ui_setting.mutex);
}
void ImguiExample::next_subtitle() {
  SDL_LockMutex(ui_setting.mutex);
  ui_setting.next_subtitle_stream = 1;
  SDL_UnlockMutex(ui_setting.mutex);
}
void ImguiExample::rewind_playback() {
  SDL_LockMutex(ui_setting.mutex);
  ui_setting.fast_forward += 1;
  SDL_UnlockMutex(ui_setting.mutex);
}
void ImguiExample::next_audio_channel() {
  SDL_LockMutex(ui_setting.mutex);
  ui_setting.next_audio_stream = 1;
  SDL_UnlockMutex(ui_setting.mutex);
}
void ImguiExample::play_next_media(int direction) {
  if (player) {
    SDL_LockMutex(ui_setting.mutex);
    ui_setting.stop_play = 1;
    SDL_UnlockMutex(ui_setting.mutex);
    player->join();
    delete player;
    hide_is_video = false;
    player = nullptr;
  }
  ui_setting.stop_play = 0;
  if (direction == 3) {
    play_list_lock.lock();
    for (auto &iter : selected)
      for (auto &stream : iter.second.play_list)
        stream.is_playing = false;

    int size = selected[old_selected_list].play_list.size();
    if (size == 1)
      ui_setting.playback_id = 0;
    if (ui_setting.playback_id >= size)
      ui_setting.playback_id = 0;
    selected[old_selected_list].play_list[ui_setting.playback_id].is_playing =
        true;
    start_playback(
        selected[old_selected_list].play_list[ui_setting.playback_id]);
    play_list_lock.unlock();

  } else {
    SDL_LockMutex(ui_setting.mutex);
    if (ui_setting.play_all == 1) {
      play_list_lock.lock();
      int vidbox_sz = selected[old_selected_list].play_list.size();
      play_list_lock.unlock();
      if (ui_setting.playback_id + 1 < vidbox_sz) {
        if (direction == 1) {
          ui_setting.playback_id += 1;
        } else {
          if (ui_setting.playback_id - 1 < 0)
            ui_setting.playback_id = 0;
          else
            ui_setting.playback_id -= 1;
        }
        play_list_lock.lock();
        for (auto &iter : selected)
          for (auto &stream : iter.second.play_list)
            stream.is_playing = false;
        selected[old_selected_list]
            .play_list[ui_setting.playback_id]
            .is_playing = true;
        start_playback(
            selected[old_selected_list].play_list[ui_setting.playback_id]);
        play_list_lock.unlock();
      } else {

        if (vidbox_sz >= 1) {
          ui_setting.playback_id = 0;
          play_list_lock.lock();
          for (auto &iter : selected)
            for (auto &stream : iter.second.play_list)
              stream.is_playing = false;
          selected[old_selected_list]
              .play_list[ui_setting.playback_id]
              .is_playing = true;
          start_playback(
              selected[old_selected_list].play_list[ui_setting.playback_id]);
          play_list_lock.unlock();
        }
      }
    }
    SDL_UnlockMutex(ui_setting.mutex);
  }
}
void ImguiExample::remove_material(std::string material_name) {
  Ogre::MaterialPtr material =
      Ogre::MaterialManager::getSingleton().getByName(material_name);
  if (!material.isNull()) {
    material.setNull();
    Ogre::MaterialManager::getSingleton().remove(material_name);
  }
}
void ImguiExample::remove_texture(std::string texture_name) {
  Ogre::TexturePtr texture =
      Ogre::TextureManager::getSingleton().getByName(texture_name);
  if (!texture.isNull()) {
    Ogre::TextureManager::getSingleton().remove(texture_name);
    texture.setNull();
  }
}
Ogre::MaterialPtr ImguiExample::create_material(std::string material_name) {
  Ogre::MaterialPtr material =
      Ogre::MaterialManager::getSingleton().getByName(material_name);
  if (material.isNull())
    material = Ogre::MaterialManager::getSingleton().create(
        material_name, // name
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

  return material;
}
Ogre::TexturePtr ImguiExample::create_texture(std::string texture_name, int w,
                                              int h) {
  Ogre::TexturePtr texture =
      Ogre::TextureManager::getSingleton().getByName(texture_name);
  if (texture.isNull())
    texture = Ogre::TextureManager::getSingleton().createManual(
        texture_name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        Ogre::TEX_TYPE_2D, w, h, 0, Ogre::PF_BYTE_RGBA,
        Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
  return texture;
}
void ImguiExample::bind_material_texture(std::string material_name,
                                         std::string texture_name, int w,
                                         int h) {

  remove_material(material_name);
  remove_texture(texture_name);

  Ogre::MaterialPtr material = create_material(material_name);
  Ogre::TexturePtr texture = create_texture(texture_name, w, h);

  material->getTechnique(0)->getPass(0)->createTextureUnitState(texture_name);
  material->getTechnique(0)->getPass(0)->setSceneBlending(
      Ogre::SBT_TRANSPARENT_ALPHA);
  material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
}
void ImguiExample::set_texture_color(std::string material_name,
                                     std::string texture_name, int w, int h) {
  remove_texture(material_name);
  remove_texture(texture_name);
  Ogre::MaterialPtr material = create_material(material_name);
  Ogre::TexturePtr texture = create_texture(texture_name, w, h);
  material->getTechnique(0)->getPass(0)->createTextureUnitState(texture_name);
  material->getTechnique(0)->getPass(0)->setSceneBlending(
      Ogre::SBT_TRANSPARENT_ALPHA);
  material->getTechnique(0)->getPass(0)->setLightingEnabled(false);

  // Get the pixel buffer
  Ogre::HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();

  // Lock the pixel buffer and get a pixel box
  pixelBuffer->lock(Ogre::Box(0, 0, w, h), Ogre::HardwareBuffer::HBL_DISCARD);
  const Ogre::PixelBox &pixelBox = pixelBuffer->getCurrentLock();

  Ogre::uint8 *pDest = static_cast<Ogre::uint8 *>(pixelBox.data);

  // Fill in some pixel data. This will give a semi-transparent blue,
  // but this is of course dependent on the chosen pixel format.
  for (size_t j = 0; j < w; j++) {
    for (size_t i = 0; i < h; i++) {
      *pDest++ = 0;   // R
      *pDest++ = 0;   // G
      *pDest++ = 105; // B
      *pDest++ = 127; // A
    }

    pDest += pixelBox.getRowSkip() *
             Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
  }

  // Unlock the pixel buffer
  pixelBuffer->unlock();
}
void ImguiExample::closeApp() {
  browser = nullptr;
  browserClient = nullptr;
  CefShutdown();
  remove_material("browser_material");
  remove_texture("browser_texture");
  remove_material("video_material");
  remove_texture("FFmpegVideoTexture");
  OgreBites::ApplicationContext::closeApp();

}