#include "web_socket.h"
void client_socket(void) {
  using namespace std;

  using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;
  WsClient client("127.0.0.1:2254/echo");
  client.on_message = [](shared_ptr<WsClient::Connection> connection,
                         shared_ptr<WsClient::Message> message) {
    auto send_stream = make_shared<WsClient::SendStream>();

    std::string to_parse = message->string();
    parse_xml(to_parse);

    string message_t;
    if (active_downloads.jobs.size() > 0) {
      active_downloads.url_downloading = true;

      if (active_downloads.jobs[0].is_playerble == 1) {
        message_t = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
        <root>\
            <messages>\
                <message url=\"" +
                    std::string(active_downloads.jobs[0].download_url) +
                    "\"  file_name=\"" +
                    std::string(active_downloads.jobs.at(0).file_name) +
                    "\" user_agent=\"" +
                    std::string(active_downloads.jobs.at(0).user_agent) +
                    "\" cookies=\"" +
                    std::string(active_downloads.jobs.at(0).cookies) +
                    "\" save_loc=\"" +
                    std::string(active_downloads.jobs.at(0).save_loc) +
                    "\" connections=\"" +
                    std::to_string(active_downloads.jobs.at(0).connections) +
                    "\">PLAY</message>\
            </messages>\
        </root>";
      } else {
        message_t = "    <?xml version=\"1.0\" encoding=\"utf-8\"?>\
        <root>\
            <messages>\
                <message url=\"" +
                    std::string(active_downloads.jobs[0].download_url) +
                    "\"  file_name=\"" +
                    std::string(active_downloads.jobs.at(0).file_name) +
                    "\" user_agent=\"" +
                    std::string(active_downloads.jobs.at(0).user_agent) +
                    "\" cookies=\"" +
                    std::string(active_downloads.jobs.at(0).cookies) +
                    "\" save_loc=\"" +
                    std::string(active_downloads.jobs.at(0).save_loc) +
                    "\" connections=\"" +
                    std::to_string(active_downloads.jobs.at(0).connections) +
                    "\">DL</message>\
            </messages>\
        </root>";
      }

      active_downloads.jobs.erase(active_downloads.jobs.begin());

    } else if (!qued_messages.empty()) {
      message_t = qued_messages[0];
      qued_messages.erase(qued_messages.begin());
    } else
      message_t = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
        <root>\
            <messages>\
                <message>TAB1</message>\
            </messages>\
        </root>";

    if (is_quit.load()) {
      message_t = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
        <root>\
            <messages>\
                <message>EXIT</message>\
            </messages>\
        </root>";
      *send_stream << message_t;
      /**
       * Implement run service in background setting here.
       */
      connection->send(send_stream);
      connection->send_close(1000);

    } else {
      *send_stream << message_t;
      connection->send(send_stream);
    }

    /// connection->send_close(1000);
  };

  client.on_open = [](shared_ptr<WsClient::Connection> connection) {
    cout << "Client: Opened connection" << endl;
    string message = "";
    // cout << "Client: Sending message: \"" << message << "\"" << endl;

    auto send_stream = make_shared<WsClient::SendStream>();
    *send_stream << message;
    connection->send(send_stream);
  };

  client.on_close = [](shared_ptr<WsClient::Connection> /*connection*/,
                       int status, const string & /*reason*/) {
    cout << "Client: Closed connection with status code " << status << endl;

    reset.store(true);
  };

  // See
  // http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html,
  // Error Codes for error code meanings
  client.on_error = [](shared_ptr<WsClient::Connection> /*connection*/,
                       const SimpleWeb::error_code &ec) {
    AddLog(std::string(std::to_string(log_count) + std::string(":") +
                       std::string("Client: Error:") +
                       std::string(ec.message()))
               .c_str());
    log_count++;
    cout << "Client: Error: " << ec << ", error message: " << ec.message()
         << endl;

    reset.store(true);
  };

  while (!is_quit.load()) {
    while (!system_ready.load())
      Sleep(100);
    client.start();
    Sleep(500);
  }
}