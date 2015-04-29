#include "gameplayer.h"
#include "ellipsoid.h"

#include <gtx/constants.hpp>
#include <gtx/vector_angle.hpp>

GamePlayer::GamePlayer(ActionCamera *camera, glm::vec3 pos)
    : Player(camera, pos)
{
    m_offset = 25.f;
    setMaxOffset(m_offset);
    m_camera->setOffset(m_offset);
    m_camera->setLook(glm::vec4(-5, -1, 0, 0));

    Ellipsoid *e = new Ellipsoid(glm::vec3(0, 0, 0), glm::vec3(.98f), "player");
    e->updatePos(pos);
    this->addCollisionShape(e);

    RenderShape *rs;
    rs = new RenderShape();
    rs->type = SPHERE;
    rs->color = glm::vec3(1.f, .5f, 0);
    rs->shininess = 32.f;
    rs->transparency = 1.f;
    rs->trans = glm::scale(glm::mat4(), glm::vec3(1.9f));
    rs->inv = glm::inverse(rs->trans);
    rs->texture = "";
    rs->repeatU = 1.f;
    rs->repeatV = 1.f;
    this->addRenderShape(rs);

//    m_usingLeap = false;
}


GamePlayer::~GamePlayer()
{
}

void GamePlayer::onTick(float secs)
{
    Player::onTick(secs);
}

void GamePlayer::onMouseMoved(QMouseEvent *, float deltaX, float deltaY)
{
//    std::cout << deltaX << std::endl;
//    if (m_usingLeap)
//    {
//        glm::vec3 oldLook = glm::vec3(m_camera->getLook());
//        glm::vec3 oldLook = glm::rotate(glm::vec3(0,0,-1), glm::pi<float>() * 2.f * deltaX, glm::vec3(0,1,0));
//        glm::vec3 look = glm::rotate(glm::vec3(oldLook.x, 0, oldLook.z), -glm::half_pi<float>() * deltaY,
//                                     glm::vec3(-oldLook.z, 0, oldLook.x));
//        m_camera->setLook(glm::vec4(look,0));
//    }
//    else
        m_camera->yaw(deltaX / 10.f);
        m_camera->pitch(deltaY / 10.f);

}

void GamePlayer::handleCollision(Collision *col)
{
    if (glm::length(col->mtv) > 0.0001f)
        col->impulse = glm::normalize(col->mtv);

    Player::handleCollision(col);
    if (!m_tempSolid)
    {
        if (col->t > 0.0001f)
        {
            setPosition(getPosition() + col->mtv);
//            setVelocity(getVelocity() + (col->mtv / col->t) * .5f);
            applyForce((col->mtv / (col->t * col->t)) * getMass());
        }
    }
}
