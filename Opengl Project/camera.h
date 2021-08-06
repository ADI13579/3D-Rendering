#pragma once
#include"coordinate.h"
#include <vector>

/// <summary>
/// ////////////////////DEBUGGING//////////////////
/// </summary>
/// <param name="mat"></param>

static void show_matrix(float mat[4][4]) {
    // show matrix


    std::cout << "Result matrix is \n";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++)
            std::cout << mat[i][j] << " ";
        std::cout << "\n";
    }

}
///////////////////////////////////////////////////

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 20.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
//Pi defined multiple files


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    coordinate3f Position;
    coordinate3f Front;
    coordinate3f Up;
    coordinate3f Right;
    coordinate3f WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(coordinate3f position = coordinate3f(0.0f, 0.0f, 0.0f), coordinate3f up = coordinate3f(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(coordinate3f(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
  /*  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }*/


    // define the  lookAt matrix
    ///////////////////////////////////////////////////////////////////////////////
// equivalent to gluLookAt()
// It returns 4x4 matrix
///////////////////////////////////////////////////////////////////////////////
    void lookAt( coordinate3f& eye,  coordinate3f&& target,  coordinate3f& upDir, float matrix[4][4])
    {

        //std::cout <<"x:" << eye.x <<"y:" << eye.y <<"z:"<< eye.z;
        // compute the forward vector from target to eye
        coordinate3f forward = eye - target;
        forward = !forward;                 // make unit length
        //std::cout << "x:" << forward.x << "y:" << forward.y << "z:" << forward.z;

        // compute the left vector
        coordinate3f left = upDir*forward; // cross product
        left = !left; // make left normalized
        //std::cout << "x:" << left.x << "y:" << left.y << "z:" << left.z;

        // recompute the orthonormal up vector
        coordinate3f up = forward*left;    // cross product
        //std::cout << "x:" << up.x << "y:" << up.y << "z:" << up.z;

        // init 4x4 matrix
        /*float** matrix ;
        matrix = new float* [4];
        matrix[0] = new float[4];
        matrix[1] = new float[4];
        matrix[2] = new float[4];
        matrix[3] = new float[4];*/
         // identitiy matrix
        // set rotation part, inverse rotation matrix: M^-1 = M^T for Euclidean transform
        matrix[0][0] = left.x;
        matrix[0][1] = left.y;
        matrix[0][2] = left.z;
        matrix[1][0] = up.x;
        matrix[1][1] = up.y;
        matrix[1][2] = up.z;
        matrix[2][0] = forward.x;
        matrix[2][1] = forward.y;
        matrix[2][2] = forward.z;

        // set translation part
        matrix[0][3] = -left.x * eye.x - left.y * eye.y - left.z * eye.z;
        matrix[1][3] = -up.x * eye.x - up.y * eye.y - up.z * eye.z;
        matrix[2][3] = -forward.x * eye.x - forward.y * eye.y - forward.z * eye.z;

     
        matrix[3][3] = 1;


        matrix[3][0] = 0;
        matrix[3][1] = 0;
        matrix[3][2] = 0;





        //show_matrix(matrix);

    }




    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    void  GetViewMatrix(float lookAtMatrix[4][4])
    {

        /*float** lookAtMatrix;
        lookAtMatrix = new float* [4];
        lookAtMatrix[0] = new float[4];
        lookAtMatrix[1] = new float[4];
        lookAtMatrix[2] = new float[4];
        lookAtMatrix[3] = new float[4];*/
        

        lookAt(Position, (Position + Front), Up, lookAtMatrix);
    }
    
    void GetPerspectiveMatrix(float angle, float ratio, float near, float far, float persMat[4][4]) {

        // GetPerspectiveMatrix(fov, aspectRatio, nearplane, farplane, perspectiveMatrix
        //persMat[4][4] = { {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0} };
        float tan_half_angle = tan(angle / 2);
        persMat[0][0] = 1 / (ratio * tan_half_angle);
        persMat[1][1] = 1 / (tan_half_angle);
        persMat[2][2] = -(far + near) / (far - near);
        persMat[3][2] = -1;
        persMat[2][3] = -(2 * far * near) / (far - near);

        //persMat = mat;
        //show_matrix(persMat);


    }




    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position = Position + coordinate3f( Front.x * velocity, Front.y * velocity, Front.z * velocity);
        if (direction == BACKWARD)
            Position = Position - coordinate3f(Front.x * velocity, Front.y * velocity, Front.z * velocity);
        if (direction == LEFT)
            Position = Position - coordinate3f(Right.x * velocity, Right.y * velocity, Right.z * velocity);
        if (direction == RIGHT)
            Position = Position + coordinate3f(Right.x * velocity, Right.y * velocity, Right.z * velocity);
        //std::cout << Position.x << "-- " << Position.y << "--" << Position.z;
    
    }



    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:

    float radians(float deg) {
        return PI / 180 * deg;
    }
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        coordinate3f front;
        front.x = cos(radians(Yaw)) * cos(radians(Pitch));
        front.y = sin(radians(Pitch));
        front.z = sin(radians(Yaw)) * cos(radians(Pitch));
        // returns the unit vector:  ! is the unit vector notation
        Front = !front;
        // also re-calculate the Right and Up vector
        Right = !(Front * WorldUp);  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = !(Right * Front);
        //Up = glm::normalize(glm::cross(Right, Front));
    }
};
