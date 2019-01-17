#include "utilities.h"

#ifdef BOOST_MAPS
boost::container::flat_map<int, std::atomic<struct download_info>>
    active_connections;
boost::container::flat_map<int, std::atomic<struct download_info>>
    active_ul_connections;
#else
std::map<int, std::atomic<struct download_info>> active_connections;
std::map<int, std::atomic<struct download_info>> active_ul_connections;
#endif
double PCFreq = 0.0;
bool set_cpu_freq = false;
__int64 CounterStart = 0;
std::map<int, struct Alpha_map> anime_alpha_titles;
std::map<int, struct Titles_map> anime_title_names_2;
std::map<int, struct Episode_map> anime_temp_episodes;

std::map<int, struct list_item> selected;

std::mutex play_list_lock, anime_temp_episodes_lock, anime_alpha_titles_lock,
    anime_title_names_2_lock;
bool items_updated = false;
std::vector<std::string> qued_messages;

bool alpha_requested = false, alpha_listing_requested = false,
     episode_listing_requested = false;
void parse_xml(std::string to_parse) {

  using namespace rapidxml;
  using namespace std;
  xml_document<> doc;
  xml_node<> *root_node;
  xml_node<> *log_node;
  xml_node<> *connection_main_node;
  xml_node<> *packet_type;

  try {

    doc.parse<0>(
        (char *)to_parse.c_str()); // Pass the non-const char* to parse()
    root_node = doc.first_node("root");
    if (root_node)
      packet_type = root_node->first_node("packet_type");

    if (root_node && packet_type) {
      std::string packet = packet_type->value();
      if (packet == "DL_UL") {
        connection_main_node = root_node->first_node("Connections");
        connection_info temp;
        if (connection_main_node) {
          temp.download_complete =
              stoi(connection_main_node->first_attribute("download_complete")
                       ->value());
          temp.upload_complete =
              stoi(connection_main_node->first_attribute("upload_complete")
                       ->value());
          temp.upload_percentage =
              stoi(connection_main_node->first_attribute("upload_percentage")
                       ->value());
          temp.transfer_not_started =
              stoi(connection_main_node->first_attribute("transfer_not_started")
                       ->value());
          temp.each_file_percent =
              stoi(connection_main_node->first_attribute("each_file_percent")
                       ->value());
          current_connection_info.store(temp);
          std::string files =
              connection_main_node->first_attribute("operation_file_name")
                  ->value();
          if (!(strlen(temp_task.file_name) <= 0) &&
              (strlen(
                   connection_main_node->first_attribute("operation_file_name")
                       ->value()) > 0))
            strcpy(temp_task.file_name,
                   connection_main_node->first_attribute("operation_file_name")
                       ->value());
          for (xml_node<> *connection_node =
                   connection_main_node->first_node("Connection");
               connection_node;
               connection_node = connection_node->next_sibling()) {

            int con_type = stoi(
                connection_node->first_attribute("connection_type")->value());
            int connection_id = stoi(
                connection_node->first_attribute("connection_id")->value());
            if (con_type == 0) {

              struct download_info temp;
              temp.con_type = con_type;
              // temp.part_file_name =
              // connection_node->first_attribute("part_file_name")->value();
              temp.connection_id = connection_id;
              temp.percentage =
                  stoi(connection_node->first_attribute("percentage")->value());
              temp.speed =
                  stoi(connection_node->first_attribute("speed")->value());
              temp.total_bytes = stoi(
                  connection_node->first_attribute("total_bytes")->value());
              temp.downloaded_bytes =
                  stoi(connection_node->first_attribute("downloaded_bytes")
                           ->value());
              active_connections[connection_id].store(temp);

            } else {

              struct download_info temp;
              temp.con_type = con_type;
              // temp.file_name =
              // connection_node->first_attribute("file_name")->value();
              temp.connection_id = connection_id;
              temp.percentage =
                  stoi(connection_node->first_attribute("percentage")->value());
              temp.speed =
                  stoi(connection_node->first_attribute("speed")->value());
              temp.total_bytes = stoi(
                  connection_node->first_attribute("total_bytes")->value());
              temp.uploaded_bytes = stoi(
                  connection_node->first_attribute("uploaded_bytes")->value());
              active_ul_connections[connection_id].store(temp);
            }
          }

          log_node = root_node->first_node("logs");
          for (xml_node<> *message_node = log_node->first_node("message");
               message_node; message_node = message_node->next_sibling()) {
            AddLog(std::string(std::to_string(log_count) + std::string(":") +
                               std::string(message_node->value()))
                       .c_str());
            log_count++;
          }
        }
      } else if (packet == "ANIME_LIST") {
        xml_node<> *anime_titles = root_node->first_node("anime_titles");
        anime_alpha_titles_lock.lock();
        anime_alpha_titles.clear();
        anime_alpha_titles_lock.unlock();
        int current_id = 0;
        for (xml_node<> *alpha_node = anime_titles->first_node("title");
             alpha_node; alpha_node = alpha_node->next_sibling()) {
          struct Alpha_map title;
          anime_alpha_titles_lock.lock();
          title.Alpha = new char[strlen(alpha_node->value())];
          strcpy(title.Alpha, alpha_node->value());
          anime_alpha_titles[current_id] = title;
          anime_alpha_titles_lock.unlock();
          current_id++;
        }
        alpha_requested = false;
      } else if (packet == "ANIME_ENUMURATOR") {
        xml_node<> *anime_titles = root_node->first_node("anime_list");
        anime_title_names_2_lock.lock();
        anime_title_names_2.clear();
        anime_title_names_2_lock.unlock();
        int current_id = 0;

        for (xml_node<> *alpha_node = anime_titles->first_node("anime");
             alpha_node; alpha_node = alpha_node->next_sibling()) {
          struct Titles_map title;

          anime_title_names_2_lock.lock();
          title.Title = new char[strlen(alpha_node->value())];
          title.dedicated_channel = atoi(alpha_node->first_attribute("dedicated_channel")->value());
          title.page_link = new char[strlen(
              alpha_node->first_attribute("page_link")->value())];
          strcpy(title.Title, alpha_node->value());
          strcpy(title.page_link,
                 alpha_node->first_attribute("page_link")->value());
          anime_title_names_2[current_id] = title;
          anime_title_names_2_lock.unlock();
          current_id++;
        }

      } else if (packet == "ANIME_EPISODES") {
        xml_node<> *anime_titles = root_node->first_node("anime_episodes");
        anime_temp_episodes_lock.lock();
        anime_temp_episodes.clear();
        anime_temp_episodes_lock.unlock();
        int current_id = 0;
        for (xml_node<> *alpha_node = anime_titles->first_node("episode");
             alpha_node; alpha_node = alpha_node->next_sibling()) {
          struct Episode_map title;
          title.Episode = new char[strlen(alpha_node->value())];
          title.page_link = new char[strlen(
              alpha_node->first_attribute("page_link")->value())];

          anime_temp_episodes_lock.lock();
          strcpy(title.page_link,
                 alpha_node->first_attribute("page_link")->value());
          strcpy(title.Episode, alpha_node->value());
          anime_temp_episodes[current_id] = title;
          anime_temp_episodes_lock.unlock();
          current_id++;
        }

        items_updated = true;
      } else if (packet == "YOUTUBE") {
        struct Media_Object youtube;
        youtube.file_url_name = root_node->first_node("youtube_url")->value();
        youtube.cookies = root_node->first_node("youtube_cookies")->value();
        youtube.headers = root_node->first_node("youtube_useragent")->value();
        youtube.name = root_node->first_node("youtube_file")->value();
        youtube.is_web_file = 1;
        youtube.is_playing = 0;
        play_list_lock.lock();
        selected[2].name = "Youtube";
        selected[2].play_list.push_back(youtube);
        play_list_lock.unlock();

      } else if (packet == "PLAYLIST") {

        struct Media_Object media;
        for (xml_node<> *playlists = root_node->first_node("playitem");
             playlists; playlists = playlists->next_sibling()) {
          media.file_url_name =
              new char[strlen(playlists->first_attribute("file")->value())];
          strcpy(media.file_url_name,
                 playlists->first_attribute("file")->value());
          media.name =
              new char[strlen(playlists->first_attribute("name")->value())];
          strcpy(media.name, playlists->first_attribute("name")->value());
          media.is_web_file = 0;
          media.is_playing = 0;
          play_list_lock.lock();
          selected[0].name = "Files";
          selected[0].play_list.push_back(media);
          play_list_lock.unlock();
        }

      } else if (packet == "PLAY") {
        struct Media_Object youtube;
        youtube.is_playing = 0;
        youtube.file_url_name =
            new char[strlen(root_node->first_node("youtube_url")->value())];
        strcpy(youtube.file_url_name,
               root_node->first_node("youtube_url")->value());

        youtube.cookies =
            new char[strlen(root_node->first_node("youtube_cookies")->value())];
        strcpy(youtube.cookies,
               root_node->first_node("youtube_cookies")->value());

        youtube.headers = new char[strlen(
            root_node->first_node("youtube_useragent")->value())];
        strcpy(youtube.headers,
               root_node->first_node("youtube_useragent")->value());

        youtube.name =
            new char[strlen(root_node->first_node("youtube_file")->value())];
        strcpy(youtube.name, root_node->first_node("youtube_file")->value());
        youtube.is_web_file =
            atoi(root_node->first_node("is_web_file")->value());

        play_list_lock.lock();
        int current_size = selected[1].play_list.size();
        selected[2].name = "Youtube";
        selected[2].play_list.push_back(youtube);
        play_list_lock.unlock();
      }
    }

  } catch (const std::runtime_error &e) {
    std::cerr << "Runtime error was: " << e.what() << std::endl;
  } catch (const rapidxml::parse_error &e) {
    std::cerr << "Parse error was: " << e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error was: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "An unknown error occurred." << std::endl;
  }
}
float normalize(float input, float max, float min) {
  float average = (min + max) / 2;
  float range = (max - min) / 2;
  float normalized_x = (input - average) / range;
  return normalized_x;
}
void AddLog(const char *fmt, ...) IM_FMTARGS(2) {
  // FIXME-OPT
  char buf[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
  buf[IM_ARRAYSIZE(buf) - 1] = 0;
  va_end(args);
  Items.push_back(Strdup(buf));
  if (Items.size() > 6)
    Items.erase(Items.begin());
  ScrollToBottom = true;
}
static char *Strdup(const char *str) {
  size_t len = strlen(str) + 1;
  void *buff = malloc(len);
  return (char *)memcpy(buff, (const void *)str, len);
}

double StartCounter() {
  LARGE_INTEGER li;
  if (!QueryPerformanceFrequency(&li))
    std::cout << "QueryPerformanceFrequency failed!\n";
  // PCFreq = double(li.QuadPart); //for seconds
  // PCFreq = double(li.QuadPart)/1000000.0; //for microsends
  PCFreq = double(li.QuadPart) / 1000.0; // milliseconds

  QueryPerformanceCounter(&li);
  return double(li.QuadPart);
}
double GetCounter(double &CounterStart) {
  LARGE_INTEGER li;
  QueryPerformanceCounter(&li);
  return double(li.QuadPart - CounterStart) / PCFreq;
}