#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
void setParameters(glm::vec3 _position, float _verticalAngle, float _horizontalAngle, bool mode);
glm::vec3 getCamPosition();

#endif