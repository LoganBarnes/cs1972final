#include "net.h"
#include "mesh.h"
#include "graphics.h"

#define GLM_FORCE_RADIANS
#include <gtc/type_ptr.hpp>

Net::Net(glm::vec2 dimension, const glm::vec3 &start,
         const glm::vec3 &interval1, const glm::vec3 &interval2,
         VerletManager* vm, GLuint shader)
    : Verlet(vm),
      m_shader(shader)
{
    int width = dimension.x;
    int height = dimension.y;
    int count = 0;

    for(int i = 0; i<width; i++){
        for(int j = 0; j<height; j++){
            createPoint(start+(float)j*interval1+(float)i*interval2);
            if(j!=0)  //attach to the left
                createLink(count-1,count);
            if(i!=0)  //attach to top
                createLink(count-width, count);
            count++;
        }
    }

    m_mesh = new Mesh();
    m_mesh->initStrip(m_shader, width, height, getPosArray(), getNormArray());
}

Net::~Net()
{
    delete m_mesh;
}


void Net::updateBuffer()
{
    m_mesh->setVerts(getPosArray(), getNormArray());
}


void Net::onDraw(Graphics *g)
{
//    g->setColor(.5,.5,1.f,1,64);

//    if(pass==1){
//        glUniform1f(glGetUniformLocation(m_shader, "shininess"), 0);
//        glUniform4f(glGetUniformLocation(m_shader, "materialColor"), 1, 1, 1, .7);
//    }
//    else
//        g->setColor(1, 1, 1, 1, 0);
    g->setColor(1, 1, 1, .7f, 0);

    glUniformMatrix4fv(glGetUniformLocation(m_shader, "model"),
                       1, GL_FALSE, glm::value_ptr(glm::mat4()));
    m_mesh->onDraw(GL_TRIANGLE_STRIP);
}

