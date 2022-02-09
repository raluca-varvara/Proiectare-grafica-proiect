#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = glm::normalize(cameraUp);

        //TODO - Update the rest of camera parameters
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
    }

    //return the view matrix, using the glm::lookAt() function


    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        if (direction == MOVE_FORWARD) {
            this->cameraPosition = this->cameraPosition + speed * this->cameraFrontDirection;
        }
        if (direction == MOVE_BACKWARD) {
            this->cameraPosition = this->cameraPosition - speed * this->cameraFrontDirection;
        }
        if (direction == MOVE_RIGHT) {
            this->cameraPosition = this->cameraPosition + speed * this->cameraRightDirection;
        }
        if (direction == MOVE_LEFT) {
            this->cameraPosition = this->cameraPosition - speed * this->cameraRightDirection;
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        glm::vec3 aux; //pitch -89 _ 79
        float pitch1;
        if (pitch < -89.0f)
            pitch1 = -89.0f;
        else if (pitch > 79.0f)
            pitch1 = 79.0f;
        else
            pitch1 = pitch;

        aux.x = glm::cos(glm::radians(pitch1)) * glm::cos(glm::radians(yaw));
        aux.y = glm::sin(glm::radians(pitch1));
        aux.z = glm::cos(glm::radians(pitch1)) * glm::sin(glm::radians(yaw));
        this->cameraFrontDirection = glm::normalize(aux);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));

    }

}