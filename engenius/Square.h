#ifndef SQUARE_H
#define SQUARE_H

#include "Mesh.h"
#include <random>

class Square : public Mesh
{
private:
	int resolution; // nb of divisions in a side of the square
	glm::vec3 m_c0;
	glm::vec3 m_c1;
	glm::vec3 m_c2;
	glm::vec3 m_c3;
public:
	Square(): Mesh(){};
	/*
	 c0 = upleft, c1 upright, c2 bottom right, c3 bottom left
	*/
	Square(glm::vec3 const &c0, glm::vec3 const &c1, glm::vec3 const &c2, glm::vec3 const &c3, int _resolution=1):
	Mesh(), m_c0(c0), m_c1(c1), m_c2(c2), m_c3(c3), resolution(_resolution)
	{
		buildArrays();
		this->setup();
	}
    glm::vec3 bottomLeft() const { return this->m_c3; }
	glm::vec3 upRight() const { return this->m_c1; }
    glm::vec3 upLeft() const { return this->m_c0; }
    glm::vec3 bottomRight() const {return this->m_c2;}
    glm::vec3 normal() const { return glm::cross((bottomRight() - bottomLeft()) , (upLeft() - bottomLeft()));}

    void setResolution(int _resolution)
    {
    	this->resolution = _resolution;
    	buildArrays();
    	this->setup();
    }

    void buildArrays()
    {
    	glm::vec3 bottom = bottomLeft() - upLeft();
		glm::vec3 right = upRight() - upLeft();
		float h = glm::length(bottom);
		float w = glm::length(right);
		
		glm::vec3 current = upLeft();
		glm::vec3 previous = upLeft();
		glm::vec3 normalizeR = glm::normalize(right);
		glm::vec3 normalizeB = glm::normalize(bottom);
		int size = resolution*2; // nb of triangle on one line
		int nb_v = resolution+1 ;//nb of vertices on one line
		float u,v;
		u = v = 0.f;

		float stepJ = w/(float)resolution;
		float stepI = h/(float)resolution;
		float stepU, stepV;
		stepU = 1/(float)resolution;
		stepV = 1/(float)resolution;
		m_indexes.clear();m_vertices.clear();m_normals.clear();m_uv.clear();
		m_indexes.resize(6*resolution*resolution);
		m_vertices.resize(nb_v*nb_v);
		m_normals.resize(nb_v*nb_v);
		m_uv.resize(nb_v*nb_v);
		for (int i = 0; i < nb_v; ++i)
		{
			v = 0.f;
			for (int j = 0; j < nb_v; ++j)
			{
				m_vertices[i*nb_v+j] = current;
				current += normalizeR*stepJ;
				m_normals[i*nb_v+j] = normal();
				m_uv[i*nb_v+j] = glm::vec2(u,v);
				v += stepV;
			}
			current = previous;
			current += normalizeB*stepI;
			previous = current;
			u+=stepU;
		}
		
		int index = 0;
		int nbTriangles = 0;
		for (int i = 0; i < 6*resolution*resolution; i+=6)
		{
			m_indexes[i] = index;
			m_indexes[i+1] = index+1;
			m_indexes[i+2] = index+nb_v;
			m_indexes[i+3] = index+nb_v;
			m_indexes[i+4] = index+1;
			m_indexes[i+5] = index+nb_v+1;
			nbTriangles+=2;
			if(nbTriangles%(resolution*2) == 0) index+=2;
			else index++;
		}
		

    }


};

#endif 