#ifndef ENTITY_H
#define ENTITY_H

#include <list>
#include <memory>

#include "Model.h"
#include "RigidBody.h"


class Transform
{
protected:
    //Local 
    glm::vec3 m_pos = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_eulerRot = { 0.0f, 0.0f, 0.0f }; //In degrees
    glm::vec3 m_scale = { 1.0f, 1.0f, 1.0f };

    //Global space information concatenate in matrix
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);

    // This flag is used to know if the transform has been modified
    bool m_hasMoved = true;

protected:
    glm::mat4 getLocalModelMatrix()
    {
        const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
                    glm::radians(m_eulerRot.x),
                    glm::vec3(1.0f, 0.0f, 0.0f));
        const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
                    glm::radians(m_eulerRot.y),
                    glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
                    glm::radians(m_eulerRot.z),
                    glm::vec3(0.0f, 0.0f, 1.0f));

        const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

        return glm::translate(glm::mat4(1.0f), m_pos) *
                    rotationMatrix *
                    glm::scale(glm::mat4(1.0f), m_scale);
    }
public:

    void computeModelMatrix()
    {
        m_modelMatrix = getLocalModelMatrix();
        m_hasMoved = false;
    }

    void computeModelMatrix(const glm::mat4& parentGlobalModelMatrix)
    {
        m_modelMatrix = parentGlobalModelMatrix * getLocalModelMatrix();
        m_hasMoved = false;
    }

    void setLocalPosition(const glm::vec3& newPosition)
    {
        m_pos = newPosition;
        m_hasMoved = true;
    }

    const glm::vec3& getLocalPosition()
    {
        return m_pos;
    }

    const glm::mat4& getModelMatrix()
    {
        return m_modelMatrix;
    }

    bool hasMoved()
    {
        return m_hasMoved;
    }

    void setScale(glm::vec3 xyz)
    {
        m_scale = xyz;
        m_hasMoved = true;
    }

    void setEulerRot(glm::vec3 xyz)
    {
        m_eulerRot = xyz;
        m_hasMoved = true;
    }

    glm::vec3 getEulerRot(){return m_eulerRot;}
};


class Entity : public Model
{
public:
    std::list<std::unique_ptr<Entity>> children;
    Entity* parent = nullptr;

    Transform transform;

    Entity(const char* path, unsigned int type) : Model(path, type)
    {}

    
    void addChild(const char* path, unsigned int type)
    {
        children.emplace_back(std::make_unique<Entity>(path, type));
        children.back()->parent = this;
    }

    void setRigidBody(RigidBody &_rb){rb = _rb;}
    RigidBody* getRigidBody(){return &rb;}

    //Update transform if it was changed
    void updateSelfAndChild()
    {
        if (transform.hasMoved())
        {
            forceUpdateSelfAndChild();
            return;
        }

        for (auto&& child : children)
        {
            child->updateSelfAndChild();
        }
    }

    //Force update of transform even if local space don't change
    void forceUpdateSelfAndChild()
    {
        if (parent)
            transform.computeModelMatrix(parent->transform.getModelMatrix());
        else
            transform.computeModelMatrix();

        for (auto&& child : children)
        {
            child->forceUpdateSelfAndChild();
        }
    }

    void update(float deltaTime)
    {
        rb.computeForces(deltaTime);
        transform.setLocalPosition(transform.getLocalPosition() + rb.getSpeed());
        updateSelfAndChild();
    }

protected:
    RigidBody rb;
};

class LODEntity : public Entity
{
private:
    int current;
public:
    LODEntity(const char* path, unsigned int type) : Entity(path, type), current(0){}
    void nextLevel(){(current+1 >= this->meshes.size())? current = 0 : current ++;}
    void setCurrentLevel(int level){current = level;}
    bool isMaxLevel(){return current == this->meshes.size()-1;}
    void Draw(GLuint shaderID){this->meshes[current]->draw(shaderID);}
};

#endif
    