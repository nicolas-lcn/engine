#ifndef MESH_H
#define MESH_H

// Include GLEW
#include <GL/glew.h>
// Include glm utils
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Include common utils
#include <common/shader.hpp>
#include <common/texture.hpp>


#include <vector> 
#include <string>

#include "Triangle.h"

struct Texture {
    unsigned int id;
    std::string type;
    char* uniformName;
    char* path;
};

class Mesh
{

private:
	GLuint VertexArrayID;

	//Buffers
	GLuint vertexbuffer;
    GLuint elementbuffer;
    GLuint normalsBuffer;
    GLuint uvsBuffer;





protected:
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_normals;
	std::vector<unsigned short> m_indexes;
	std::vector<glm::vec2> m_uv;

	std::vector<Texture> textures;
	std::vector<std::vector<unsigned short>> m_triangles;


public:
	Mesh(){};
	virtual ~Mesh(){};
	
	Mesh(std::vector<glm::vec3> vertices, std::vector<unsigned short> indexes, std::vector<std::vector<unsigned short>> triangles):
	m_vertices(vertices), m_indexes(indexes), m_triangles(triangles)
	{
		setup();
	}
	
	Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<unsigned short> indexes, std::vector<glm::vec2> uv):
	m_vertices(vertices), m_normals(normals), m_indexes(indexes), m_uv(uv)
	{
		setup();
	}


	Mesh(std::vector<Triangle> _triangles)
	{	
		m_vertices.resize(3 * _triangles.size());
		m_indexes.resize(3 * _triangles.size());
		m_normals.resize(3 * _triangles.size());
		m_triangles.resize(_triangles.size());
		for (int i = 0; i < _triangles.size(); ++i)
		{
			Triangle triangle = _triangles[i];
			m_vertices[3*i + 0] = triangle.c0();
			m_vertices[3*i + 1] = triangle.c1();
			m_vertices[3*i + 2] = triangle.c2();
			m_indexes[3*i + 0] = 3*i + 0;
			m_indexes[3*i + 1] = 3*i + 1;
			m_indexes[3*i + 2] = 3*i + 2;
			m_normals[3*i + 0] = triangle.normal();
			m_normals[3*i + 1] = triangle.normal();
			m_normals[3*i + 2] = triangle.normal();
			m_triangles[i] = {(3*i + 0), (3*i + 1), (3*i + 2)};
		}
	};

	void deleteBuffers()
	{
		glDeleteBuffers(1, &vertexbuffer);
    	glDeleteBuffers(1, &elementbuffer);
    	glDeleteBuffers(1, &normalsBuffer);
    	glDeleteBuffers(1, &uvsBuffer);
    	glDeleteVertexArrays(1, &VertexArrayID);

	}

	

	std::vector<glm::vec3> vertices(){return m_vertices;}
	std::vector<unsigned short> indexes(){return m_indexes;}
	std::vector<std::vector<unsigned short>> triangles(){return m_triangles;}
	std::vector<glm::vec2> texCoords(){return m_uv;}
	std::vector<glm::vec3> normals(){return m_normals;}

	glm::vec3 center()
	{
		glm::vec3 result = glm::vec3();
		for (int i = 0; i < m_vertices.size(); ++i) {
			result += glm::vec3(m_vertices[i][0], m_vertices[i][1], m_vertices[i][2]);
		}
		result *= (1.0/(float) m_vertices.size());
		return result;
	}

	void addTexture(char* path, char* name)
	{
		GLuint id = loadBMP_custom(path);
		Texture tex = {id, "texture", name, path};
		textures.push_back(tex);
	}

	void setup()
	{
    	glGenVertexArrays(1, &VertexArrayID);
    	glBindVertexArray(VertexArrayID);
		// Load it into a VBO

	    glGenBuffers(1, &vertexbuffer);
	    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);

	    // Generate a buffer for the m_indexes as well
	    glGenBuffers(1, &elementbuffer);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexes.size() * sizeof(unsigned short), &m_indexes[0] , GL_STATIC_DRAW);

	    glGenBuffers(1, &normalsBuffer);
	    glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(glm::vec3), &m_normals[0], GL_STATIC_DRAW);

	    glGenBuffers(1, &uvsBuffer);
	    glBindBuffer(GL_ARRAY_BUFFER, uvsBuffer);
	    glBufferData(GL_ARRAY_BUFFER, m_uv.size() * sizeof(glm::vec2), &m_uv[0], GL_STATIC_DRAW);

	}

	void draw(GLuint shaderID)
	{

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );

        glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, uvsBuffer);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

        // Bind and Active textures
        for (size_t i = 0; i < textures.size(); ++i)
        {
        	glActiveTexture(GL_TEXTURE0 + i);
        	glBindTexture(GL_TEXTURE_2D, textures[i].id);
        	glUniform1i(glGetUniformLocation(shaderID, textures[i].uniformName), i);
        }
        glEnableVertexAttribArray(2);

        // Draw the triangles !
        glDrawElements(
                    GL_TRIANGLES,      // mode
                    m_indexes.size(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
                    );


       

        glDisableVertexAttribArray(0);
	}


};

#endif