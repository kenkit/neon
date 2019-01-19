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
