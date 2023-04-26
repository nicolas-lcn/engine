#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#define EARTH_G 9.81

// Include glm utils
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class RigidBody
{
public:
	RigidBody(float _mass=1.0):mass(_mass){};
	~RigidBody(){};
	
	void computeForces(float deltaTime)
	{
		glm::vec3 F = glm::vec3(0.0, 0.0, 0.0);
		for (int i = 0; i < others.size(); ++i)
		{
			F += others[i];
		}
		glm::vec3 sumForces = glm::vec3(0.0, -0.5, 0.0) + F;
		float invMass = mass > 0.0f ? (1.0/mass) : 0.0f;
		glm::vec3 a = invMass * sumForces;
		setAcceleration(a);
		glm::vec3 a_t = deltaTime * a;
		glm::vec3 v_t = speed + a_t;
		setSpeed(v_t);
		others = std::vector<glm::vec3>();
	}

	void applyForce(glm::vec3 force)
	{
		others.push_back(force);
	}

	glm::vec3 getForcesDirection()
	{
		glm::vec3 F = glm::vec3(0.0, 0.0, 0.0);
		for (int i = 0; i < others.size(); ++i)
		{
			F += others[i];
		}
		F += glm::vec3(0.0, -0.1, 0.0);
		return glm::normalize(F);
	}

	void setSpeed(glm::vec3 &_speed){speed = _speed;}
	void setAcceleration(glm::vec3 &_accel){accel=_accel;}

	glm::vec3 getSpeed(){return speed;}
	glm::vec3 getAccel(){return accel;}

	glm::vec3 computeRebound(glm::vec3 normal)
	{
		return glm::reflect(speed, normal);
		
	}



private:
	glm::vec3 speed;
	glm::vec3 accel;
	std::vector<glm::vec3> others;

	float mass;
};
#endif