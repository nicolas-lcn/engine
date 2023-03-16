#ifndef SPHERE_H
#define SPHERE_H

#include <cmath>

#include "Mesh.h"

static
glm::vec3 SphericalCoordinatesToEuclidean( glm::vec3 ThetaPhiR ) {
    return ThetaPhiR[2] * glm::vec3( cos(ThetaPhiR[0]) * cos(ThetaPhiR[1]) , sin(ThetaPhiR[0]) * cos(ThetaPhiR[1]) , sin(ThetaPhiR[1]) );
}
static
glm::vec3 SphericalCoordinatesToEuclidean( float theta , float phi ) {
    return glm::vec3( cos(theta) * cos(phi) , sin(theta) * cos(phi) , sin(phi) );
}

static
glm::vec3 EuclideanCoordinatesToSpherical( glm::vec3 xyz ) {
    float R = glm::length(xyz);
    float phi = asin( xyz[2] / R );
    float theta = atan2( xyz[1] , xyz[0] );
    return glm::vec3( theta , phi , R );
}

class Sphere : public Mesh
{
private:
	float m_radius;
	glm::vec3 m_center;

public:
	Sphere() : Mesh(){};
	Sphere(float radius, glm::vec3 center) : Mesh(), m_radius(radius), m_center(center)
	{
		buildArrays();
		this->setup();
	}

	void buildArrays()
	{
        unsigned int nTheta = 20 , nPhi = 20;
        m_vertices.resize(nTheta * nPhi );
        m_normals.resize(nTheta * nPhi );
        m_uv.resize(nTheta * nPhi );
        for( unsigned int thetaIt = 0 ; thetaIt < nTheta ; ++thetaIt ) {
            float u = (float)(thetaIt) / (float)(nTheta-1);
            float theta = u * 2 * M_PI;
            for( unsigned int phiIt = 0 ; phiIt < nPhi ; ++phiIt ) {
                unsigned int vertexIndex = thetaIt + phiIt * nTheta;
                float v = (float)(phiIt) / (float)(nPhi-1);
                float phi = - M_PI/2.0 + v * M_PI;
                glm::vec3 xyz = SphericalCoordinatesToEuclidean( theta , phi );
                m_vertices[vertexIndex] = m_center + m_radius * xyz;
                m_normals[vertexIndex] = xyz;
                m_uv[vertexIndex] = {u,v};
            }
        }
        m_indexes.clear();
        for( unsigned int thetaIt = 0 ; thetaIt < nTheta - 1 ; ++thetaIt ) {
            for( unsigned int phiIt = 0 ; phiIt < nPhi - 1 ; ++phiIt ) {
                unsigned short vertexuv = thetaIt + phiIt * nTheta;
                unsigned short vertexUv = thetaIt + 1 + phiIt * nTheta;
                unsigned short vertexuV = thetaIt + (phiIt+1) * nTheta;
                unsigned short vertexUV = thetaIt + 1 + (phiIt+1) * nTheta;
                m_indexes.push_back( vertexuv );
                m_indexes.push_back( vertexUv );
                m_indexes.push_back( vertexUV );
                m_indexes.push_back( vertexuv );
                m_indexes.push_back( vertexUV );
                m_indexes.push_back( vertexuV );
            }
        }
	}
};

#endif 