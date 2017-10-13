#include <glm/glm.hpp>

class Camera { // TODO: have this extend a mobile class which is also used for car
private:
    glm::vec3 position;
    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 n;
    float fovy;
    float near_plane;
    float far_plane;
    /// Width / Height
    float aspect;

    /// @return cos(a) when a is the angle between vector and the up direction
    inline float getTopAngle(const glm::vec3& vec) { return glm::dot(vec, glm::vec3(0, 1.0f, 0)); }

public:
    Camera();
    virtual ~Camera() {}

    void orient(const glm::vec3& position, const glm::vec3& at, const glm::vec3& up);
    inline void setViewVolume(float fovy, float aspect, float near_plane, float far_plane) {
        this->fovy = fovy; this->aspect = aspect;
        this->near_plane = near_plane; this->far_plane    = far_plane;
    }

    void slide(const glm::vec3& d);
    void slideXZ(const glm::vec3& d);
    void slideY(const glm::vec3& d);

    void rotate(float angle, const glm::vec3& axis);
    void pitch(float angle);

    void setAspect(float a) { aspect = a; }

    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewMatrix() const;

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getU() const { return u; }
    glm::vec3 getV() const { return v; }
    glm::vec3 getN() const { return n; }
};
