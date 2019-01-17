#include <include/cef_client.h>
#include <include/cef_render_handler.h>// for manual render handler
#include <cef_osr/render_handler.h>
class BrowserClient : public CefClient
{
public:
    BrowserClient(RenderHandler *renderHandler)
        : m_renderHandler(renderHandler)
    {;}

    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() {
        return m_renderHandler;
    }

    CefRefPtr<CefRenderHandler> m_renderHandler;

    IMPLEMENT_REFCOUNTING(BrowserClient);
};
