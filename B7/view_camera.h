#ifndef CAMERA_H
#define CAMERA_H

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

enum Camara_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 LookAt;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float Distance;
    float MovementSpeed = 2.5f;
    bool KeepUp = true;

    Camera(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
        : Position(position), LookAt(lookAt), Up(up)
    {
        Distance = glm::length(LookAt - Position);
    }


    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, LookAt, Up);
    }

    void ProcessInput(Camara_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        switch (direction)
        {
        case FORWARD:
            Distance -= velocity;
            break;
        case BACKWARD:
            Distance += velocity;
            break;
        case LEFT:
            Position -= Right * velocity;
            break;
        case RIGHT:
            Position += Right * velocity;
            break;
        case UP:
            if (KeepUp && glm::dot(Up, WorldUp) < 0.02f) break;
            Position += Up * velocity;
            break;
        case DOWN:
            if (KeepUp && glm::dot(Up, WorldUp) < 0.02f) break;
            Position -= Up * velocity;
            break;
        default:
            break;
        }
        Position = glm::normalize(Position) * Distance;
        if (KeepUp)
            Right = glm::normalize(glm::cross(LookAt - Position, WorldUp));
        else
            Right = glm::normalize(glm::cross(LookAt - Position, Up));
        Up = glm::normalize(glm::cross(Right, LookAt - Position));
    }
};

#endif // !CAMERA_H
