#ifndef TRIANGLE_H
#define TRIANGLE_H
// Include GLM
#include <glm/glm.hpp>
class Triangle
{
private:
	glm::vec3 m_c[3];
	glm::vec3 m_normal;
public:
	Triangle(){};
	Triangle(glm::vec3 const &c0, glm::vec3 const &c1, glm::vec3 const &c2)
	{
		m_c[0] = c0;
        m_c[1] = c1;
        m_c[2] = c2;
        updateAreaAndNormal();
	}

	void updateAreaAndNormal() {
        glm::vec3 nNotNormalized = glm::cross( m_c[1] - m_c[0] , m_c[2] - m_c[0] );
        float norm = nNotNormalized.length();
        m_normal = nNotNormalized / norm;
    }

	glm::vec3 const & normal() const { return m_normal; }
    glm::vec3 const & c0() const { return m_c[0]; }
    glm::vec3 const & c1() const { return m_c[1]; }
    glm::vec3 const & c2() const { return m_c[2]; }
};

#endif 