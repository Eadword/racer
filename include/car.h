#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>


#include "meshmaker.h"
#include "objmesh.h"

struct Car : public MobileEntity {
    Car(Shader& s) : MobileEntity(new MultiEntity(s, std::make_shared<ObjMesh>("eclipse.obj", s))) {
        glm::vec3 pyr(0.0f, 0.0f, 0.0f);
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        transform = std::make_shared<glm::mat4>(glm::rotate(glm::mat4(), glm::pi<float>(), up));
        updateMobVals(&pyr, nullptr, &up);
    }

    virtual ~Car() {}
};
