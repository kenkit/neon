#ifndef COMMON_H
#define COMMON_H 1

#include <atomic>
#include <boost/chrono.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/timer/timer.hpp>
#include <map>
#include <mutex>
#include <tuple>
#include <vector>
struct account_info {
  int login_state;
  std::string login_code;
  std::string login_page;
};

struct download_info {
  int con_type;
  int connection_id;
  int downloaded_bytes;
  int uploaded_bytes;
  int total_bytes;
  int percentage;
  double speed;
};

struct connection_info {
  int connection_type;
  int download_complete;
  int upload_complete;
  double upload_percentage;
  int transfer_not_started;
  double each_file_percent;
};

struct download_task {
  char download_url[500];
  char file_name[500];
  char cookies[500];
  char user_agent[500];
  char save_loc[500];
  int connections;
  int is_playerble;
};

struct download_scheduler {
  bool url_downloading;
  std::vector<struct download_task> jobs;
  bool cancel;
};

struct YOUTUBE_DL {
  std::string url_ret;
  std::string cookies;
  std::string file_name;
  std::string user_agent;
};
struct elapsed_time {
  boost::timer::cpu_timer start_time;
  boost::chrono::nanoseconds total_elapsed;
  std::string name;
  double start_time_frq;
  double total_elapsed_frq;
  int timer_type;
  bool global_update;
  // total_elapsed.total_milliseconds() << std::endl;
};
void AddLog(const char *fmt, ...);

#ifdef BOOST_MAPS
extern boost::container::flat_map<int, struct elapsed_time> active_timers;
extern boost::container::flat_map<int, std::atomic<struct download_info>>
    active_connections;
extern boost::container::flat_map<int, std::atomic<struct download_info>>
    active_ul_connections;
#else
extern std::map<int, std::atomic<struct download_info>> active_connections;
extern std::map<int, std::atomic<struct download_info>> active_ul_connections;
#endif
extern std::atomic<connection_info> current_connection_info;

extern std::atomic<bool> reset;
extern std::atomic<bool> system_ready;
extern std::atomic<bool> is_quit;
extern int log_count;
extern bool ScrollToBottom;

extern download_scheduler active_downloads;
extern download_task temp_task;
extern int dl_clicked;
extern struct download_task temp_task;
#endif