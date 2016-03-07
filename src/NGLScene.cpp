#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <iostream>

#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Util.h>
#include <ngl/NGLStream.h>

NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("Blank NGL");

  m_view = ngl::lookAt(ngl::Vec3(2, 2, 2),
                       ngl::Vec3(0, 0, 0),
                       ngl::Vec3(0, 1, 0));
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL(QResizeEvent *_event)
{
  m_width=_event->size().width()*devicePixelRatio();
  m_height=_event->size().height()*devicePixelRatio();

  m_project = ngl::perspective(45.0f,
                               float(m_width/m_height),
                               0.2f,
                               20.0f);
}

void NGLScene::resizeGL(int _w , int _h)
{
  m_width=_w*devicePixelRatio();
  m_height=_h*devicePixelRatio();

  m_project = ngl::perspective(45.0f,
                               float(m_width/m_height),
                               0.2f,
                               20.0f);
}


void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  //name creates a sphere that we can access through a map using "sphere"
  ngl::VAOPrimitives::instance()->createCone("cone", 0.5, 1, 30, 30);

  //initialize shader lib singleton
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  shader->createShaderProgram("diffuse");

  //create shaders
  shader->attachShader("diffuseVertex", ngl::ShaderType::VERTEX);
  shader->attachShader("diffuseFragment", ngl::ShaderType::FRAGMENT);

  shader->loadShaderSource("diffuseVertex" ,"shaders/DiffuseVertex.glsl");
  shader->loadShaderSource("diffuseFragment" ,"shaders/DiffuseFragment.glsl");

  shader->compileShader("diffuseVertex");
  shader->compileShader("diffuseFrafment");

  shader->attachShaderToProgram("diffuse", "diffuseVertex");
  shader->attachShaderToProgram("diffuse", "diffuseFragment");

  shader->linkProgramObject("diffuse");
  shader->use("diffuse");

  shader->setRegisteredUniform("colour", 0.0f, 1.0f, 1.0f, 1.0f);
  shader->setRegisteredUniform("lightPos", 3.0f, 0.0f, 0.0f);
  shader->setRegisteredUniform("lightDiffuse", 1.0f, 1.0f, 1.0f, 1.0f);
}



void NGLScene::paintGL()
{
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();

  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_width,m_height);
  ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
  shader->setRegisteredUniform("colour", 0.0f, 0.0f, 1.0f, 1.0f);
  m_transform.reset();
  m_transform.setScale(1.0, 1.0, 1.0);
  m_transform.setRotation(-90, 0, 0);

  loadMatToShader();

  //draw sphere that was initialised in initializeGL()
  prim->draw("cone");

  shader->setRegisteredUniform("colour", 1.0f, 0.0f, 0.0f, 1.0f);

  m_transform.reset();
  m_transform.setScale(1.0, 1.0, 1.0);
  m_transform.setRotation(90, 0, 0);

  loadMatToShader();

  //draw sphere that was initialised in initializeGL()
  prim->draw("cone");


  std::cout << m_view << "\n" << m_project << std::endl;

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{

}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
  default : break;
  }
  // finally update the GLWindow and re-draw

    update();
}

void NGLScene::loadMatToShader()
{
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  ngl::Mat4 MVP = m_transform.getMatrix() * m_view * m_project;
  shader->setRegisteredUniform("MVP", MVP);
}
