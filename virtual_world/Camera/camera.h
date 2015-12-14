#ifndef CAMERA_H
#define CAMERA_H

#pragma once
#include "icg_common.h"

class Camera{
private:
    float m_cameraDistance;
    float m_horizontalAngle;
    // vertical angle : 0, look at the horizon
    float m_verticalAngle;
    float m_initialFoV;
    vec3 m_cameraPos;
    vec3 m_rCameraPos;
    vec3 m_direction;
    vec3 m_rightPos;
    vec3 m_up;
    float m_mouseSpeed;
    float m_speed;
    float m_deltaTime;
    float m_FoV;
    float m_height;
    int m_xPos, m_yPos;
    int m_oldXPos, m_oldYPos;
    bool m_moveUp, m_moveDown, m_moveRight, m_moveLeft, m_flymode;

public:
    void init(float cameraDistance, float horizontalAngle, float verticleAngle, float initialFoV, vec3 cameraPos, float mouseSpeed, float speed)
    {
        m_cameraDistance = cameraDistance;
        m_horizontalAngle = horizontalAngle;
        m_verticalAngle = verticleAngle;
        m_initialFoV = initialFoV;
        m_cameraPos = cameraPos;
        m_mouseSpeed = mouseSpeed;
        m_speed = speed;
        m_moveUp = false;
        m_moveDown = false;
        m_moveRight = false;
        m_moveLeft = false;
        m_flymode = true;
    }

    void cameraMovement()
    {
        static double lastTime = glfwGetTime();
        double currentTime = glfwGetTime();
        m_deltaTime = currentTime - lastTime;

        glfwGetMousePos(&m_xPos, &m_yPos);

        // Reset mouse position for next frame
        m_horizontalAngle += m_mouseSpeed * float(m_oldXPos - m_xPos );
        m_verticalAngle   += m_mouseSpeed * float( m_oldYPos - m_yPos );
        m_oldXPos = m_xPos;
        m_oldYPos = m_yPos;

        // Compute new orientation

        m_direction = vec3(cos(m_verticalAngle) * sin(m_horizontalAngle), cos(m_verticalAngle) * cos(m_horizontalAngle), sin(m_verticalAngle));
        m_rightPos = vec3(sin(m_horizontalAngle - M_PI/2.0f), cos(m_horizontalAngle - M_PI/2.0f), 0);
        m_up = m_rightPos.cross(m_direction);
        m_FoV = m_initialFoV - 5 * glfwGetMouseWheel();
        lastTime = currentTime;
    }

    void cameraState()
    {
        if(m_moveUp)
            m_cameraPos += m_direction * m_deltaTime * m_speed;
        if(m_moveDown)
            m_cameraPos -= m_direction * m_deltaTime * m_speed;
        if(m_moveRight)
            m_cameraPos += m_rightPos * m_deltaTime * m_speed;
        if(m_moveLeft)
            m_cameraPos -= m_rightPos * m_deltaTime * m_speed;
        if(m_flymode)
            m_cameraPos(2) = m_height + 0.2;
    }

    void moveUp() {
        m_moveUp = true;
    }

    void moveDown() {
        m_moveDown = true;
    }

    void moveRight() {
        m_moveRight = true;
    }

    void moveLeft() {
        m_moveLeft = true;
    }

    void stopUp() {
        m_moveUp = false;
    }

    void stopDown() {
        m_moveDown = false;
    }

    void stopRight() {
        m_moveRight = false;
    }

    void stopLeft() {
        m_moveLeft = false;
    }

    void setHeight(float height) {
        m_height = height;
    }

    void toggleFlymode() {
        m_flymode = !m_flymode;
    }

    vec3 getCameraPosition()
    {
        return m_cameraPos;
    }

    vec3 getReverseCameraPosition()
    {
        m_rCameraPos = m_cameraPos;
        m_rCameraPos(2) = -1.0*m_cameraPos(2);
        return m_rCameraPos;
    }

    vec3 getDirection()
    {
        return m_direction;
    }

    vec3 getUp()
    {
        return m_up;
    }

    vec3 getInitialDirection() {
        return vec3(cos(0.0f) * sin(0.0f), cos(0.0f) * cos(0.0f), sin(0.0f));
    }

};


#endif // CAMERA_H

