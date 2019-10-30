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
GLuint program_background;

GLuint programTF;
GLuint VAO;
GLuint VAO2;
GLuint VAO3;
GLuint VAO_background;
GLuint n_elements;
GLuint n_elements2;
GLuint background_elements;
GLuint texture_id2;
GLuint texture_id;
GLuint texture_id3;
GLuint texture_id4;
GLuint skytexture;

GLuint framebufferinit = 0;
GLuint renderedtexture = 1;
GLuint depthrenderbuffer = 2;
GLuint query;
GLuint n_points;
// camera
Camera cam;
Camera cam2;
Mesh background;



GLuint text;
std::string fragment = "basic.fs";
std::chrono::time_point<std::chrono::system_clock> start, end;

GLuint program_tf_id;
GLuint VAOtf;
GLuint VBO[4]; // 0 and 1 are for the position 2 for the normal and 3 for the texcoor
glm::mat4 mvp3;

void init()
{
  

    std::cout << "Creation of main object" << std::endl; 

    //creation des shader program
    program_id = glhelper::create_program_from_file("shaders/simple.vs", "shaders/frankie.fs");
    program_id2 = glhelper::create_program_from_file("shaders/simple.vs", "shaders/test.fs");
    program_background = glhelper::create_program_from_file("shaders/simple.vs", "shaders/test2.fs");
    
    //Chargement texture
    texture_id2 = glhelper::load_texture("data/water-normal.png");
    texture_id3 = glhelper::load_texture("data/grass-normal.jpg");
    texture_id3 = glhelper::load_texture("data/grass.jpg");
    texture_id = glhelper::load_texture("data/Frankie/flyingsquirrel_skin_col.png");
    skytexture = glhelper::load_texture("data/sky.png");
   
  
  {
    std::cout << "Creation of main object" << std::endl; 
    //Maillage Frankie
    Mesh m = Mesh::load_from_file("data/Frankie/Frankie3.obj");
    n_elements=m.size_element();
    m.apply_matrix(glm::mat4(
                          0.2, 0., 0., 0.,
                          0., 0.2, 0., 0.,
                          0., 0., 0.2, 0.,
                          0., 0., 0., 1.));
    auto pos = m.position();
    auto normal = m.normal();
    auto texcoord = m.texcoord();
    n_points = pos.size()/3;
    texture_id = glhelper::load_texture("data/Frankie/flyingsquirrel_skin_col.png");

    // create transform feedback
    auto content =  glhelper::read_file("shaders/tf.vs");
    auto shader_id = glhelper::compile_shader(content.c_str(), GL_VERTEX_SHADER);
    program_tf_id = glCreateProgram();
    glAttachShader(program_tf_id, shader_id);

    const GLchar* attributes[] = {"pos"};
    glTransformFeedbackVaryings(program_tf_id, 1, attributes, GL_SEPARATE_ATTRIBS);
    glLinkProgram(program_tf_id);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.idx().size() * sizeof(GLuint), m.idx().data(), GL_STATIC_DRAW);

    glGenBuffers(4, VBO);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(GLfloat), pos.data(), GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(GLfloat), pos.data(), GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(GLfloat), normal.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, texcoord.size() * sizeof(GLfloat), texcoord.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glGenQueries(1, &query);
  }
   
    //Creation quad (eau + pelouse)
    Mesh quad = Mesh::create_grid(512);
    quad.apply_matrix(glm::mat4(
                          1., 0., 0., 0.,
                          0., 1., 0., 0.,
                          0., 0., 1., 0.,
                          0., 0., 0., 1.));
    n_elements2 = quad.size_element();
    VAO2 = quad.load_to_gpu();
    
    //Creation quad (fond d ecran)
    background = Mesh::create_grid(512);
    //Rotation de la grille
    background.apply_matrix(glm::mat4(
                          1., 0., 0., 0.,
                          0., cos(M_PI/2.), -sin(M_PI/2.), 0.,
                          0., sin(M_PI/2.), cos(M_PI/2.), 0.,
                          0., 0., 0., 1.) * glm::mat4(
                            cos(M_PI_2),-sin(M_PI_2),0,0,
                            sin(M_PI_2),cos(M_PI_2),0,0,
                            0,0,1,0,
                            0,0,0,1
                          ) );
    background_elements = quad.size_element();
    VAO_background = background.load_to_gpu();


   start = std::chrono::system_clock::now();
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
  glEnable(GL_DEPTH_TEST);

  //initialisation framebuffer pour capturer une texure qui nous permettra de creer le reflet de frankie et du ciel dans l eau
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


  


  

    //Plusieurs texture utlise pour le quad avec l eau 
   glUseProgram(program_id2);
   text = glGetUniformLocation(program_id2, "textureSamplerM");
   glUniform1i(text,1);
   text = glGetUniformLocation(program_id2, "textureSampler");
   glUniform1i(text, 2);
   text = glGetUniformLocation(program_id2, "textureGrass");
   glUniform1i(text, 3);
   text = glGetUniformLocation(program_id2, "textureGrass2");
   glUniform1i(text, 4);
   glm::vec3 testccc = cam.position() * glm::mat3(
               cos(M_PI), 0., sin(M_PI),
               0., 1., 0.,
               -sin(M_PI), 0., cos(M_PI));
  cam.common_motion(0.5,1);
   
   //La deuxième camera ne bouge pas elle permet de fixer des objets vis a vis de la cam1
   cam2.position() = cam.position() ;
}

static void display_callback()
{
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

  //glEnable(GL_RASTERIZER_DISCARD);

  //Dessine les objets dont le position dépend de notre cam 
  glm::mat4 mvp = cam.projection() * cam.view();
  //Dessine nos objets fixe avec la cam2
  glm::mat4 mvp2 = cam2.projection() * cam2.view();
  //mvp3 utilise pour le fond avec un reshape (on aurai pu le realiser directement sur le modele (init))
  mvp3 = cam2.projection() * glm::mat4( 
               5.0, 0., 0., 0.,
               0., 5., 0.,0.,
              0., 0., 1.0, 0.1,
               0., 0., 0., 1.)*cam2.view()  ;


  {

    end = std::chrono::system_clock::now();
    float time = std::chrono::duration_cast<std::chrono::duration<float>>(start - end).count();
    //Recuperation d une texture avec frankie et le ciel pour le mettre sur un quad
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferinit);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, cam2.width(), cam2.height());
    glUseProgram(program_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindVertexArray(VAO);
    GLint mvp_id = glGetUniformLocation(program_id, "MVP"); 
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp2[0][0]);
    glDrawElements(GL_TRIANGLES, n_elements, GL_UNSIGNED_INT, 0); 
    glBindVertexArray(0);
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferinit);
    glViewport(1, 1, cam.width(), cam.height());
    glUseProgram(program_background);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skytexture);
    glBindVertexArray(VAO_background);
    GLint mvp_idb = glGetUniformLocation(program_background, "MVP");
    glUniformMatrix4fv(mvp_idb, 1, GL_FALSE, &mvp3[0][0]);
    GLint campos_id2 = glGetUniformLocation(program_background, "POS_CAM");
    glUniform3fv(campos_id2, 1, &cam2.position()[0]);
    GLint timevalue = glGetUniformLocation(program_background, "time");
    glUniform1f(timevalue, time);
    glDrawElements(GL_TRIANGLES, background_elements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}

    //Draw ciel
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(1, 1, cam.width(), cam.height());
    glUseProgram(program_background);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skytexture);
    glBindVertexArray(VAO_background);
    GLint mvp_idb = glGetUniformLocation(program_background, "MVP");
    glUniformMatrix4fv(mvp_idb, 1, GL_FALSE, &mvp3[0][0]);
    GLint campos_id2 = glGetUniformLocation(program_background, "POS_CAM");
    glUniform3fv(campos_id2, 1, &cam2.position()[0]);
    GLint timevalue = glGetUniformLocation(program_background, "time");
    glUniform1f(timevalue, time);
    glDrawElements(GL_TRIANGLES, background_elements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

 
  // displacement with tf
  {	
    glEnable(GL_RASTERIZER_DISCARD);
    glUseProgram(program_tf_id);
    GLint myUniformLocationtf = glGetUniformLocation(program_tf_id, "time");
    glUniform1f(myUniformLocationtf, time);
    glBindVertexArray(VAO);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, VBO[1]);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,  VBO[0]);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,0,0);
    glBeginTransformFeedback(GL_POINTS);
    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);                
    glDrawArrays(GL_POINTS, 0, n_points);
    glEndTransformFeedback();
    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);                         
    GLuint n;
    glGetQueryObjectuiv(query, GL_QUERY_RESULT, &n);              
    glFlush();
    glDisableVertexAttribArray(1);                                                
    glDisable(GL_RASTERIZER_DISCARD);
    std::swap(VBO[0], VBO[1]);
  }

  // Draw Frankie
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, cam.width(), cam.height());
    glUseProgram(program_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLint mvp_id = glGetUniformLocation(program_id, "MVP");
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp[0][0]); 
    glDrawElements(GL_TRIANGLES, n_elements, GL_UNSIGNED_INT, 0); 
  }
  

    //Draw quad

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(program_id2);
    glBindVertexArray(VAO2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_id2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, renderedtexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture_id3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texture_id4);


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
   
    glBindVertexArray(0);

    //glActiveTexture(0);



  }



 
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
