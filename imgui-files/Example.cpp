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
#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <windows.h>

#include "ImguiManager.h"
#include "imgui_dock.h"
class ImguiExample : public OgreBites::ApplicationContext,
                     public OgreBites::InputListener {
public:
  ImguiExample() : OgreBites::ApplicationContext("OgreImguiExample") {}
  void update() {
    if (ImGui::Begin("Dock Demo", NULL,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
      // dock layout by hard-coded or .ini file
      ImGui::BeginDockspace();

      if (ImGui::BeginDock("Dock 1")) {

        ImGui::Text("I'm BentleyBlanks!");
      }
      ImGui::EndDock();

      if (ImGui::BeginDock("Dock 2")) {
        ImGui::Text("I'm BentleyBlanks!");
      }
      ImGui::EndDock();

      if (ImGui::BeginDock("Dock 3")) {
        ImGui::Text("I'm LonelyWaiting!");
      }
      ImGui::EndDock();

      ImGui::EndDockspace();
    }
    ImGui::End();
  }
  bool frameStarted(const Ogre::FrameEvent &evt) {

    OgreBites::ApplicationContext::frameStarted(evt);

    Ogre::ImguiManager::getSingleton().newFrame(
        evt.timeSinceLastFrame, Ogre::Rect(0, 0, getRenderWindow()->getWidth(),
                                           getRenderWindow()->getHeight()));
    ImGui::SetNextWindowBgAlpha(0.95f);
    bool show_dock_window = true;
    ImGui::SetNextWindowSize(ImVec2(getRenderWindow()->getWidth() - 50.0f,
                                    getRenderWindow()->getHeight() - 50.0f),
                             ImGuiSetCond_Always);

    ImGui::StyleColorsClassic();
    update();

    return true;
  }

  void setup() {

    OgreBites::ApplicationContext::setup();
    addInputListener(this);

    Ogre::ImguiManager::createSingleton();
    addInputListener(Ogre::ImguiManager::getSingletonPtr());
    ImGui::InitDock();
    // get a pointer to the already created root
    Ogre::Root *root = getRoot();
    Ogre::SceneManager *scnMgr = root->createSceneManager();
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

    Ogre::SceneNode *camNode =
        scnMgr->getRootSceneNode()->createChildSceneNode();
    camNode->setPosition(0, 0, 15);
    camNode->lookAt(Ogre::Vector3(0, 0, -1), Ogre::Node::TS_PARENT);

    Ogre::Camera *cam = scnMgr->createCamera("myCam");
    cam->setNearClipDistance(5); // specific to this sample
    cam->setAutoAspectRatio(true);
    camNode->attachObject(cam);
    getRenderWindow()->addViewport(cam);

    Ogre::Entity *ent = scnMgr->createEntity("Sinbad.mesh");
    Ogre::SceneNode *node = scnMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(ent);
  }

  bool keyPressed(const OgreBites::KeyboardEvent &evt) {
    if (evt.keysym.sym == 27) {
      getRoot()->queueEndRendering();
    }
    return true;
  }
};

int main(int argc, char *argv[]) {
  ImguiExample app;
  app.initApp();
  app.getRoot()->startRendering();
  app.closeApp();

  return 0;
}
