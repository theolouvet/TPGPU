#include <iostream>
#include <fstream>
#include <chrono>


#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "glhelper.h"
#include "mesh.h"
#include "camera.h"

#define TINYOBJLOADER_IMPLEMENTATION 
#include "external/tiny_obj_loader.h"

// main obj
GLuint program_id;
GLuint program_id2;
GLuint programTF;
GLuint VAO;
GLuint VAO2;
GLuint VAO3;
GLuint n_elements;
GLuint n_elements2;
GLuint texture_id2;
GLuint texture_id;

GLuint framebufferinit = 0;
GLuint renderedtexture = 1;
GLuint depthrenderbuffer = 2;


GLuint VBOtf;
GLuint EBOtf;
GLuint VAOtf;

// camera
Camera cam;
Camera cam2;

Mesh tf;
GLuint tbo;
const GLchar* feedbackVaryings[] = { "outValue" };
Mesh m;

GLuint text;
std::string fragment = "basic.fs";
std::chrono::time_point<std::chrono::system_clock> start, end;


void init()
{
  // create main obj

    std::cout << "Creation of main object" << std::endl; 
    program_id = glhelper::create_program_from_file("shaders/simple.vs", "shaders/frankie.fs");
    program_id2 = glhelper::create_program_from_file("shaders/simple.vs", "shaders/simple.fs");


    glTransformFeedbackVaryings(programTF, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
    programTF = glhelper::create_program_from_file("shaders/tf.vs", "shaders/tf.fs");

    m = Mesh::load_from_file("data/Frankie/Frankie3.obj");
    m.apply_matrix(glm::mat4(
          1., 0., 0., 0.,
          0., 1., 0., 0.,
          0., 0., 1., 0.,
          0., 0., 0., 1.));
    n_elements=m.size_element();
    VAO = m.load_to_gpu();
    VAOtf = VAO;
    texture_id2 = glhelper::load_texture("data/water-normal.png");
    texture_id = glhelper::load_texture("data/Frankie/flyingsquirrel_skin_col.png");

    Mesh quad = Mesh::create_grid(512);
    quad.apply_matrix(glm::mat4(
                          1., 0., 0., 0.,
                          0., 1., 0., 0.,
                          0., 0., 1., 0.,
                          0., 0., 0., 1.));
    n_elements2 = quad.size_element();
    VAO2 = quad.load_to_gpu();
    std::cout << "ahah" << quad.size_element() <<std::endl;



   start = std::chrono::system_clock::now();
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
  glEnable(GL_DEPTH_TEST);

  glGenFramebuffers(1, &framebufferinit);
  glBindFramebuffer(GL_FRAMEBUFFER, framebufferinit);
  glGenTextures(1, &renderedtexture);
  glBindTexture(GL_TEXTURE_2D, renderedtexture);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, cam.width(), cam.height(), 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedtexture, 0);
  glGenRenderbuffers(1, &depthrenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, cam.width(), cam.height());
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout<<"error";


   Mesh o = Mesh::load_from_file("data/Frankie/Frankie3.obj");


   VAOtf = o.load_to_gpu();
   glBindVertexArray(VAOtf);
   //glGenVertexArrays(1, &VAOtf);
   //glBindVertexArray(VAOtf);


   VBOtf = o.create_VBO();
   EBOtf = o.create_VBO();

   /*glGenBuffers(1, &VBOtf);
   glBindBuffer(GL_ARRAY_BUFFER, VBOtf);

   glGenBuffers(1, &EBOtf);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOtf);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size() * sizeof(GLuint), m.indices.data(), GL_STATIC_DRAW);
*/

   GLint inputAttrib = glGetAttribLocation(programTF, "inValue");
   glEnableVertexAttribArray(inputAttrib);
   glVertexAttribPointer(inputAttrib, 1, GL_FLOAT, GL_FALSE, 0, 0);
   //tbo = o.create_VBO();

   glGenBuffers(1, &tbo);
   glBindBuffer(GL_ARRAY_BUFFER, tbo);
   /*glEnable(GL_RASTERIZER_DISCARD);
   glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);
   glBeginTransformFeedback(GL_POINTS);
   glDrawArrays(GL_POINTS, 0, m.size_element());
   glEndTransformFeedback();
   glFlush();
  /* GLfloat feedback[o.size_element()];
  // glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m.size_element(), &VBOtf);
   //std::cout <<" test" << feedback[m.size_element() - 1];

   glDisable(GL_RASTERIZER_DISCARD);*/



  // n_elements=sizeof(&VAO);
   glUseProgram(program_id2);
   text = glGetUniformLocation(program_id2, "textureSamplerM");
   glUniform1i(text,1);
   text = glGetUniformLocation(program_id2, "textureSampler");
   glUniform1i(text, 2);

   cam2.position() = cam.position() * glm::mat3(
               1., 0., 10.,
               0., 1., 0.,
               0., -1., 1.);




}

static void display_callback()
{
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

  //glEnable(GL_RASTERIZER_DISCARD);

  glm::mat4 mvp = cam.projection() * cam.view();
  glm::mat4 mvp2 = cam2.projection() * cam2.view();



  //  glActiveTexture(GL_TEXTURE0);
  // display obj
  {

    end = std::chrono::system_clock::now();
    float time = std::chrono::duration_cast<std::chrono::duration<float>>(start - end).count();
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferinit);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, cam.width(), cam.height());
    glUseProgram(program_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindVertexArray(VAOtf);
    GLint mvp_id = glGetUniformLocation(program_id, "MVP");
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp2[0][0]);
    glDrawElements(GL_TRIANGLES, n_elements, GL_UNSIGNED_INT, 0); 
    glBindVertexArray(0);





  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, cam.width(), cam.height());
    glUseProgram(program_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindVertexArray(VAO);
    GLint mvp_id = glGetUniformLocation(program_id, "MVP");
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp[0][0]);
    glDrawElements(GL_TRIANGLES, n_elements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

   // glEnable(GL_TEXTURE_2D);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(program_id2);
    glBindVertexArray(VAO2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_id2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, renderedtexture);


    GLint mvp_id2 = glGetUniformLocation(program_id2, "MVP");
    glUniformMatrix4fv(mvp_id2, 1, GL_FALSE, &mvp[0][0]);
    GLint campos_id = glGetUniformLocation(program_id2, "POS_CAM");
    glUniform3fv(campos_id, 1, &cam.position()[0]);
    GLint myUniformLocation2 = glGetUniformLocation(program_id2, "time");
    glUniform1f(myUniformLocation2, time);
    if(cos(time) == 1)
        std::cout <<1;

    glDrawElements(GL_TRIANGLES, n_elements2, GL_UNSIGNED_INT, 0);

   // glDisable(GL_TEXTURE_2D);


  }



  glEnable(GL_RASTERIZER_DISCARD);
  glUseProgram(programTF);
  glBindVertexArray(VAOtf);
  glBindBuffer(GL_ARRAY_BUFFER,VBOtf);

  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);

  glBeginTransformFeedback(GL_POINTS);


  glDrawArrays(GL_POINTS, 0, m.size_element());
  glEndTransformFeedback();
  glFlush();
  glDisable(GL_RASTERIZER_DISCARD);
 // glDrawElements(GL_TRIANGLES);
  glBindVertexArray(0);
  glutSwapBuffers ();
}

static void keyboard_callback(unsigned char key, int, int)
{
  switch (key)
  {
    case 'q':
    case 'Q':
    case 27:
      exit(0);
  }
  glutPostRedisplay();
}

static void reshape_callback(int width, int height)
{
  cam.common_reshape(width,height);

  glViewport(0,0, width, height); 
  glutPostRedisplay();
}


static void mouse_callback (int button, int action, int x, int y)
{
  cam.common_mouse(button, action, x, y);

  glutPostRedisplay();
}

static void motion_callback(int x, int y)
{
  cam.common_motion(x, y);
  glutPostRedisplay();
}

static void timer_callback(int)
{
  glutTimerFunc(25, timer_callback, 0);
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
  glutInitContextVersion(3, 3); 
  glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(cam.width(), cam.height());
  glutCreateWindow("opengl");
  glutDisplayFunc(display_callback);
  glutMotionFunc(motion_callback);
  glutMouseFunc(mouse_callback);
  glutKeyboardFunc(keyboard_callback);
  glutReshapeFunc(reshape_callback);
  glutTimerFunc(25, timer_callback, 0);

  glewExperimental=true;
  glewInit();

  init();
  glutMainLoop();

  return 0;
}
