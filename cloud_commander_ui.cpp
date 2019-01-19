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
#ifdef USE_BOOST_THREAD
  #include <boost/thread.hpp>
#else
  #include <thread>
#endif
#include "common.h"
#include "web_socket.h"
#include "gui_interface.h"
#include "ImguiManager.h"
#include <include/cef_app.h>
#include <include/cef_client.h>

 #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#undef main
int CALLBACK WinMain(HINSTANCE hInstance, 
                        HINSTANCE hPrevInstance, 
                        LPSTR lpCmdLine, 
                        int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{

    CefMainArgs args(argc, argv);


    {
        int result = CefExecuteProcess(args, nullptr, nullptr);
        // checkout CefApp, derive it and set it as second parameter, for more control on
        // command args and resources.
        if (result >= 0) // child proccess has endend, so exit.
        {
            return result;
        }
        if (result == -1)
        {
            // we are here in the father proccess.
        }
    }

    {
        CefSettings settings;
        #if !defined(CEF_USE_SANDBOX)
        settings.no_sandbox = true;
        #endif

        // checkout detailed settings options http://magpcss.org/ceforum/apidocs/projects/%28default%29/_cef_settings_t.html
        // nearly all the settings can be set via args too.
        // settings.multi_threaded_message_loop = true; // not supported, except windows
        // CefString(&settings.browser_subprocess_path).FromASCII("sub_proccess path, by default uses and starts this executeable as child");
        // CefString(&settings.cache_path).FromASCII("");
        // CefString(&settings.log_file).FromASCII("");
        // settings.log_severity = LOGSEVERITY_DEFAULT;
        // CefString(&settings.resources_dir_path).FromASCII("");
        // CefString(&settings.locales_dir_path).FromASCII("");

        bool result = CefInitialize(args, settings, nullptr, nullptr);
        // CefInitialize creates a sub-proccess and executes the same executeable, as calling CefInitialize, if not set different in settings.browser_subprocess_path
        // if you create an extra program just for the childproccess you only have to call CefExecuteProcess(...) in it.
        if (!result)
        {
            // handle error
            return -1;
        }
    }

    #ifdef USE_BOOST_THREAD
    boost::thread player,client_link;
    client_link = boost::thread(client_socket);
    #else
        std::thread player;
        std::thread client_link(client_socket);
    #endif
    //player = thread(start_player, argv[1]);
    ImguiExample app;

    reset.store(false);
    system_ready.store(false);
    is_quit.store(false);

    app.initApp();
    app.getRoot()->startRendering();
    app.closeApp();
    is_quit = true;
    client_link.join();
      CefShutdown();
    //player.join();
    return 0;
}
