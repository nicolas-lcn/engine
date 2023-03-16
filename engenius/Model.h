#ifndef MODEL_H
#define MODEL_H

#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

#define OBJ 0
#define OFF 1
#define SQUARE 2
#define SPHERE 3

#include "Sphere.h"
#include "Square.h"

class Model
{
public:
	std::vector<Mesh> meshes;

	Model(const char* path, unsigned int type){loadModel(path, type);}

	void Draw(GLuint shaderID)
	{
		for (size_t i = 0; i < meshes.size(); ++i)
		{
			meshes[i].draw(shaderID);
		}
	}
private:
	void loadModel(const char* path, unsigned int type)
	{
		std::vector<unsigned short> indices; //Triangles concaténés dans une liste
	    std::vector<glm::vec3> indexed_vertices;
	    std::vector<glm::vec2> uvs;
	    std::vector<glm::vec3> normals;
     	std::vector<std::vector<unsigned short> > triangles;

     	Mesh mesh;
	    //Chargement du fichier de maillage
	    switch(type)
	    {
	    	case OBJ:  
	    		loadOBJ(path, indexed_vertices, uvs, normals);
	    		indexVBO(indexed_vertices, uvs, normals, indices, indexed_vertices, uvs, normals);
	    		mesh = Mesh(indexed_vertices, normals, indices, uvs);
	    		break;
	    	case OFF:  
	    		loadOFF(path, indexed_vertices, indices, triangles);
	    		mesh = Mesh(indexed_vertices, indices, triangles);
	    		break;
	    	case SQUARE:
	    		mesh = static_cast<Mesh>(Square());
	    		break;
	    	default:
	    		mesh = static_cast<Mesh>(Sphere(1, {0.f, 0.f, 0.f}));
	    		break;
	    }
	   
	    
	    meshes.push_back(mesh);
	}
};
#endif 





// Mesh(std::string filepath)
// 	{

// 	    //Chargement du fichier de maillage
// 	    loadOBJ("../data/models/icosphere.obj", m_vertices, m_uv, m_normals);
// 	    indexVBO(m_vertices, m_uv, m_normals, m_indexes, m_vertices, m_uv, m_normals);
// 	    setup();

// 	}