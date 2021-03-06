#include "gamescreen.h"
#include "application.h"
#include "actioncamera.h"
#include "gameplayer.h"
#include "geometriccollisionmanager.h"
#include "triangle.h"

#include <gtc/constants.hpp>

GameScreen::GameScreen(Application *parent)
    : ScreenH(parent)
{
    m_parentApp->setUseCubeMap(false);

    m_oh = new ObjectHandler();

    ActionCamera *cam = new ActionCamera();
    glm::vec3 playerPos = glm::vec3(0, 0, 0);
    cam->setCenter(playerPos);

    GamePlayer *player = new GamePlayer(cam, playerPos);
//    player->m_usingLeap = m_parentApp->isUsingLeapMotion();

    GeometricCollisionManager *gcm = new GeometricCollisionManager();

    m_world = new GameWorld();
    m_world->setPlayer(player);
    m_world->addManager(gcm);
//    m_world->setGravity(glm::vec3(0, -10, 0));
    // there is nothing for the player to interact with right now so adding gravity would
    // make the camera position fall into the abyss.

    setCamera(cam);



    /////////////// how lights are created (10 is the maximum in the shader for now)///////////////////
//    // light struct
//    Light *light;

//    for (int i = 0; i < 10; i++)
//    {
//        float angle = i*.2f*glm::pi<float>();

//        light = new Light();
//        light->id = i;                  // index into array in shader
//        light->type = POINT;            // can be POINT or DIRECTIONAL for now
//        light->color = glm::vec3(.2f);  // rgb color
//        light->posDir = glm::vec3(cos(angle) * 10, 10, sin(angle) * 10 - 15);// position or direction depending on light type

//        m_tempLights.append(light);
//    }
}

GameScreen::~GameScreen()
{
    delete m_oh;
    delete m_world;

//    foreach (Light *l, m_tempLights)
//        delete l;
}

// update and render
void GameScreen::onTick(float secs)
{
    m_world->onTick(secs);
}

void GameScreen::onRender(Graphics *g)
{
    /////////////////graphics class basics///////////////////

    // ambient intensity for the world
    g->setWorldColor(.2f, .2f, .2f);


}

void GameScreen::render2D(Graphics *)
{
    // draw UI stuff;
}


void GameScreen::onMouseMoved(QMouseEvent *e, float deltaX, float deltaY, glm::vec3)
{
    m_world->onMouseMoved(e, deltaX * 1000.f, deltaY * 1000.f);
}

void GameScreen::onMouseDragged(QMouseEvent *e, float deltaX, float deltaY, glm::vec3)
{
    m_world->onMouseMoved(e, deltaX * 1000.f, deltaY * 1000.f);
}

void GameScreen::onKeyPressed(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Backspace)
    {
        m_parentApp->popScreens(1);
        return;
    }
    m_world->onKeyPressed(e);
}

void GameScreen::onKeyReleased(QKeyEvent *e )
{
    m_world->onKeyReleased(e);
}


// unused in game
void GameScreen::onMousePressed(QMouseEvent *) {}
void GameScreen::onMouseReleased(QMouseEvent *) {}
void GameScreen::onMouseWheel(QWheelEvent *) {}
