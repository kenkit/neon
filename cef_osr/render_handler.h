#include <include/cef_app.h>
#include <include/cef_render_handler.h>

class RenderHandler : public Ogre::FrameListener, public CefRenderHandler
{
public:
    Ogre::TexturePtr m_renderTexture;
    Ogre::RenderWindow *mWindow;
    RenderHandler(Ogre::TexturePtr texture,Ogre::RenderWindow *window)
        : m_renderTexture(texture),mWindow(window)
    {;}

    // FrameListener interface
public:
    bool frameStarted(const Ogre::FrameEvent &evt)
    {
        if (mWindow->isClosed())
        {
            return false;
        }
        
        CefDoMessageLoopWork();

        return true;
    }

    // CefRenderHandler interface
public:
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
    {
        rect = CefRect(0, 0, m_renderTexture->getWidth(), m_renderTexture->getHeight());
        //return true;
    }
    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
    {
        Ogre::HardwarePixelBufferSharedPtr texBuf = m_renderTexture->getBuffer();
        texBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        memcpy(texBuf->getCurrentLock().data, buffer, width*height*4);
        texBuf->unlock();
    }

    // CefBase interface
public:
    IMPLEMENT_REFCOUNTING(RenderHandler);

};
