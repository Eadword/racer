#pragma once

#include <glm/glm.hpp>
#include "shader.h"

struct Material {
    /// Emission
    glm::vec3 Le;
    /// Ambient reflectivity
    glm::vec3 Ka;
    /// Diffuse reflectivity
    glm::vec3 Kd;
    /// Specular reflectivity
    glm::vec3 Ks;
    /// Specular exponent
    float shine;

    Material() : Le(0.0f), Ka(0.0f), Kd(0.5f), Ks(0.0f), shine(1.0f) {}
    Material(glm::vec3 Le, glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float shine) :
            Le(Le), Ka(Ka), Kd(Kd), Ks(Ks), shine(shine) {}

    void setUniforms(Shader& s) {
        s.setUniform("Le", Le);
        s.setUniform("Ka", Ka);
        s.setUniform("Kd", Kd);
        s.setUniform("Ks", Ks);
        s.setUniform("shine", shine);
    }
};
