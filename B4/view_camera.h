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
    float MouseSensitivity = 0.01f;
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
            if (KeepUp && (Position - LookAt).y > 0 && glm::dot(Up, WorldUp) < 0.05f) break;
            Position += Up * velocity;
            break;
        case DOWN:
            if (KeepUp && (Position - LookAt).y < 0 && glm::dot(Up, WorldUp) < 0.05f) break;
            Position -= Up * velocity;
            break;
        default:
            break;
        }
        Position = glm::normalize(Position - LookAt) * Distance + LookAt;
        if (KeepUp)
            Right = glm::normalize(glm::cross(LookAt - Position, WorldUp));
        else
            Right = glm::normalize(glm::cross(LookAt - Position, Up));
        Up = glm::normalize(glm::cross(Right, LookAt - Position));
    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;
        
        if (!KeepUp || (Position - LookAt).y * yoffset > 0 || glm::dot(Up, WorldUp) >= 0.05f)
            Position -= Up * yoffset;
        Position -= Right * xoffset;

        Position = glm::normalize(Position - LookAt) * Distance + LookAt;
        if (KeepUp)
            Right = glm::normalize(glm::cross(LookAt - Position, WorldUp));
        else
            Right = glm::normalize(glm::cross(LookAt - Position, Up));
        Up = glm::normalize(glm::cross(Right, LookAt - Position));
    }

    void ProcessMouseScroll(float yoffset)
    {
        Distance -= yoffset;
        if (Distance <= 1.0f)
            Distance = 1.0f;
        if (Distance >= 20.0f)
            Distance = 20.0f;
    }
};

#endif // !CAMERA_H
