#ifndef MESH_H
#define MESH_H
#include <vector>
#include "Triangle.h"

class Mesh
{
protected:
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_normals;
	std::vector<unsigned int> m_indexes;
	std::vector<std::vector<unsigned int>> m_triangles;
	std::vector<glm::vec2> m_uv;
public:
	Mesh(){};
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

	std::vector<glm::vec3> vertices(){return m_vertices;}
	std::vector<unsigned int> indexes(){return m_indexes;}
	std::vector<std::vector<unsigned int>> triangles(){return m_triangles;}
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


};

#endif