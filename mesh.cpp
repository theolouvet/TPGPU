#include "mesh.h"

#include <iostream> 

Mesh::Mesh(const std::vector<GLfloat>& v, const std::vector<GLuint>& i)
  : vertices(v), indices(i){}

Mesh Mesh::load_from_file(const std::string filename)
{
  std::string warn, err;

  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  vertices.clear();
  indices.clear();

  std::vector<glm::ivec3> index;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) 
  {
    std::cerr << (warn + err) << std::endl;
  }
  for (const auto& shape : shapes)
    for(const auto& indice : shape.mesh.indices)
    {
      glm::ivec3 ind = glm::ivec3(indice.vertex_index, indice.normal_index, indice.texcoord_index);
      auto it = index.begin(); 
      while ( it != index.cend() && (*it != ind) ){++it;}
      if(it == index.cend())
      {

        index.emplace_back(indice.vertex_index, indice.normal_index, indice.texcoord_index);
        vertices.push_back(attrib.vertices[3*indice.vertex_index]);
        vertices.push_back(attrib.vertices[3*indice.vertex_index+1]);
        vertices.push_back(attrib.vertices[3*indice.vertex_index+2]);

        if(indice.normal_index != -1)
        {
          vertices.push_back(attrib.normals[3*indice.normal_index]);
          vertices.push_back(attrib.normals[3*indice.normal_index+1]);
          vertices.push_back(attrib.normals[3*indice.normal_index+2]);
        }
        else
        {
          vertices.push_back(1.);
          vertices.push_back(0.);
          vertices.push_back(0.);
        }
        vertices.push_back(attrib.texcoords[2*indice.texcoord_index]);
        vertices.push_back(attrib.texcoords[2*indice.texcoord_index+1]);
      }
    }

  for (const auto& shape : shapes)
    for(const auto& indice : shape.mesh.indices)
    {
      glm::ivec3 ind = glm::ivec3(indice.vertex_index, indice.normal_index, indice.texcoord_index);

      auto it = index.cbegin(); 
      while ( it != index.cend() && (*it != ind) ){++it;}
      if(it != index.cend())
      {
        indices.push_back(std::distance(index.cbegin(),it));
      }
    }

  return {vertices, indices};
}
unsigned int  Mesh::size_element() const {return indices.size();}

GLuint Mesh::create_VBO() const
{
  GLuint vbo;
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

  return vbo;
}
GLuint Mesh::create_EBO() const
{
  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
  return ebo;
}
GLuint Mesh::load_to_gpu() const
{
  GLuint vao, vbo, ebo;

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  vbo = create_VBO();
  ebo = create_EBO();

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return vao;
}

Mesh Mesh::create_grid(int N)
{
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  GLfloat delta = 1./(float(N)-1.);

  for(int i = 0; i < N; ++i)
  {
    for(int j = 0; j < N; ++j)
    {
      vertices.push_back(-1.+2.*j*delta); // x
      vertices.push_back(0.);// y
      vertices.push_back(-1.+2.*i*delta); // y
      vertices.push_back(0.); // nx
      vertices.push_back(-1.); // ny
      vertices.push_back(0.); // nz
      vertices.push_back(j*delta); // u
      vertices.push_back(i*delta); // v
    }
  }
  for(int i = 0; i < N*(N-1); ++i)
  {
    if( (i+1)%N != 0 ) 
    {
      indices.push_back(i);
      indices.push_back(i+1);
      indices.push_back(i+N);

      indices.push_back(i+1);
      indices.push_back(i+N);
      indices.push_back(i+N+1);
    }
  }

  return {vertices, indices};
}


void Mesh::apply_matrix(const glm::mat4& m)
{
  for (auto i = 0u; i < vertices.size(); i+=8)
  {
    glm::vec4 p(vertices[i], vertices[i+1], vertices[i+2], 1.);
    glm::vec4 n(vertices[i+3], vertices[i+4], vertices[i+5], 0.);
    p = m * p;
    n = m * n;
    vertices[i] = p.x;
    vertices[i+1] = p.y;
    vertices[i+2] = p.z;
    vertices[i+3] = n.x;
    vertices[i+4] = n.y;
    vertices[i+5] = n.z;
  }
}
