#include "leveltwo.h"
#include "application.h"
#include "gameworld.h"
#include "objecthandler.h"
#include "obj.h"
#include "actioncamera.h"
#include "gameplayer.h"
#include "geometriccollisionmanager.h"
#include "verletmanager.h"
#include "audio.h"
#include "soundtester.h"
#include "grass.h"
#include "trianglemesh.h"
#include "half.h"

#define PI 3.14159

#include "debugprinting.h"

LevelTwo::LevelTwo(Application *parent)
    : ScreenH(parent),
      m_world(NULL),
      m_oh(NULL),
      m_resetIndex(0)
{
    m_parentApp->setMouseDecoupled(true);
    m_parentApp->setLeapRightClick(GRAB);
    m_parentApp->setLeapLeftClick(PINCH);

    m_oh = new ObjectHandler();
    QList<Triangle *> tris;

    resetWorld(glm::vec3(0, 10, 0));
}


LevelTwo::~LevelTwo()
{
    delete m_world;
    delete m_oh; // m_level is deleted here
}


OBJ* LevelTwo::addIsland(const QString& path, GLuint shader, const glm::vec3& offset){
    QList<Triangle *> tris;
    OBJ *island = m_oh->getObject(path, shader, &tris, offset);
    m_world->addObject(island);
    m_world->addToMesh(tris);
    m_resetHalves.append(island->top);
    return island;
}

void LevelTwo::addMarker(const QString& objPath, GLuint shader, const glm::vec3& offset, const QString& signPath){

    QList<Triangle*> tris;
    OBJ* objMarker = m_oh->getObject(objPath, shader, &tris, offset);
    Marker* marker = new Marker(objMarker, glm::vec2(0.f, 0.f), glm::vec2(1.2,1.2), signPath);
    m_markers.append(marker);

    m_world->addObject(objMarker);
    m_world->addToMesh(tris);
}


void LevelTwo::resetWorld(glm::vec3 playerPos)
{
    playerPos = glm::vec3(0,50,-50);

    if (m_world)
    {
        delete m_world;
        m_world = NULL;
    }
    m_resetHalves.clear();

    GLuint shader = m_parentApp->getShader(GEOMETRY);
//    GLuint shader = m_parentApp->getShader(DEFAULT);

    QList<Light*> lights;
    // set lights

    ActionCamera *cam;
    cam = new ActionCamera();
    cam->setCenter(playerPos);

    GamePlayer *player = new GamePlayer(cam, playerPos);
    player->useSound(m_parentApp->getAudioObject());

    GeometricCollisionManager *gcm = new GeometricCollisionManager();
    VerletManager *vm = new VerletManager(cam);

    m_world = new GameWorld();
    m_world->setLights(lights);
    m_world->addManager(gcm);
    m_world->addManager(vm);
    m_world->setPlayer(player);

    m_world->setGravity(glm::vec3(0,-10,0));
    setCamera(cam);
    player->setMaxOffset(50); //zoom

    //Add all islands
//    OBJ* island1 = addIsland(":/objects/Plane.obj",shader,glm::vec3(0));
    addIsland(":/objects/testsmall.obj", shader, glm::vec3(0,0,0));
    addIsland(":/objects/testsmall.obj", shader, glm::vec3(0,42,0));
    addIsland(":/objects/MediumIsland.obj", shader, glm::vec3(0,42,-50));

    //Add all verlet entities
    vm->addVerlet(new TriangleMesh(glm::vec2(5,35), .6, glm::vec3(-2,43,-3), vm, shader,X,true,TOP_EDGE));

    vm->addVerlet(new TriangleMesh(glm::vec2(10,40), .6, glm::vec3(0,44,-55), vm, shader,Y,true, NONE));

    // stairs
    int numStairs = 10;
    int y = 5;
    for(int i=0; i<720; i+= 360/numStairs, y+=2){

        float rad = i * (PI/180.0);
        TriangleMesh* t = new TriangleMesh(glm::vec2(6,12), .6, glm::vec3(-10*sin(rad),y,10*cos(rad)), vm, shader);
        vm->addVerlet(t);
    }



//    vm->addVerlet(new TriangleMesh(glm::vec2(6,20), .6, glm::vec3(-90,2,-36), vm, shader));

//    vm->addVerlet(new TriangleMesh(glm::vec2(25,55), .6, glm::vec3(-136,35,-48), vm, shader,Z,true, ALL_CORNERS));



//    Grass* grass = new Grass(vm, shader);
//    grass->createPatch(glm::vec2(0,0),10,island1);
//    vm->addVerlet(grass);

    m_cursor = glm::scale(glm::mat4(), glm::vec3(.02f / cam->getAspectRatio(), .02f, .02f));
    m_cursor[3][2] = -.999f;

    m_drawCursor = true;
    m_deltas = glm::vec2(0);

    onResize(m_parentApp->getWidth(), m_parentApp->getHeight());

}

// update and render
void LevelTwo::onTick(float secs)
{

    m_world->onTick(secs, m_cursor[3][0], m_cursor[3][1]);

    if (m_world->getPlayer()->getPosition().y < -40)
    {
        Half *h = m_resetHalves.value(m_resetIndex);
        glm::vec2 c = h->getCenter();
        resetWorld(glm::vec3(c.x, h->getYLimits().y + 5.f, c.y));
    }

    glm::vec3 pos = m_world->getPlayer()->getPosition();
    for (int i = 0; i < m_resetHalves.size(); i++){
        Half* h = m_resetHalves[i];
        if(h->inHitBox(pos)&&i>m_resetIndex){
            m_resetIndex = i;
            break;
        }
    }

//    cout<<"player pos: "<<pos.x<<" "<<pos.y<<" "<<pos.z<<endl;
}


void LevelTwo::onRender(Graphics *g)
{
    // draw markers first
    foreach(Marker* marker, m_markers){
        if(marker->isInRange(m_world->getPlayer()->getPosition(), 1.8))
            marker->displayTexture(g);
    }

    g->setWorldColor(.2f,.2f,.2f);
    g->setColor(1,1,1,1,0);

    g->setTexture("grass.png", 5.f, 5.f);

    m_world->onDraw(g);

    g->setAllWhite(true);
    g->drawLine(glm::vec3(0, 0, -5), glm::vec3(0, 15, -5));
    g->setAllWhite(false);

    render2D(g);
}

void LevelTwo::render2D(Graphics *g)
{
    g->setGraphicsMode(DRAW2D);

    if (m_drawCursor)
    {
        g->setTexture("");
        g->setColor(1, 0, 0, 1, 0);
//        g->setAllWhite(true);
        g->drawSphere(m_cursor);
        g->setAllWhite(false);
    }
}

void LevelTwo::onMouseMoved(QMouseEvent *e, float deltaX, float deltaY, glm::vec3 pos)
{
    if (m_parentApp->isUsingLeapMotion())
    {
        deltaX *= 1.5f;
        deltaY *= 1.5f;
    }

    m_world->onMouseMoved(e, deltaX * 1000.f, deltaY * 1000.f);

    m_cursor[3][0] = pos.x;
    m_cursor[3][1] = pos.y;
}

void LevelTwo::onMouseDragged(QMouseEvent *e, float deltaX, float deltaY, glm::vec3 pos)
{
    if (m_parentApp->isUsingLeapMotion())
    {
        deltaX *= 1.5f;
        deltaY *= 1.5f;
    }

    m_world->onMouseMoved(e, deltaX * 1000.f, deltaY * 1000.f);

    m_cursor[3][0] = pos.x;
    m_cursor[3][1] = pos.y;
}

void LevelTwo::onKeyPressed(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Backspace)
    {
        m_parentApp->popScreens(1);
        return;
    }
    m_world->onKeyPressed(e);
}

void LevelTwo::onKeyReleased(QKeyEvent *e )
{
    if (e->key() == Qt::Key_P)
        m_parentApp->useLeapMotion(!m_parentApp->isUsingLeapMotion());

    m_world->onKeyReleased(e);

}

void LevelTwo::onResize(int w, int h)
{
    ScreenH::onResize(w, h);

    float inverseAspect = h * 1.f / w;

    glm::vec4 pos = m_cursor[3];
    m_cursor = glm::scale(glm::mat4(), glm::vec3(.02f * inverseAspect, .02f, 1));
    m_cursor[3] = pos;
}

void LevelTwo::onMousePressed(QMouseEvent *e)
{
    if (e->button() == Qt::MidButton)   //Qt::RightButton
        m_parentApp->setMouseDecoupled(false);

    m_world->onMousePressed(e);
}

void LevelTwo::onMouseReleased(QMouseEvent *e)
{
    if (e->button() == Qt::MidButton)  //Qt::RightButton
        m_parentApp->setMouseDecoupled(true);

    m_world->onMouseReleased(e);
}

// unused in game
void LevelTwo::onMouseWheel(QWheelEvent *e) {
    m_world->onMouseWheel(e);
}
