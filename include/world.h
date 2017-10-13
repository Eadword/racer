#pragma once

#include "car.h"

/**
 * The world, this contains all models which will be rendered in it, and is able
 * to render them as well. Note that it takes ownership of all objects. This is
 * a relativly simple container for the world data.
 */
struct World {
    std::shared_ptr<Material> mtl_ground;
    std::shared_ptr<Material> mtl_track;
    std::shared_ptr<Material> mtl_tree;
    std::shared_ptr<Material> mtl_trunk;
    std::shared_ptr<Material> mtl_building;
    std::shared_ptr<Material> mtl_lamp;
    std::shared_ptr<Material> mtl_post;

    glm::vec3 sun_direction;
    glm::vec3 sun_intensity;
    glm::vec3 lamp_positions[12];
    glm::vec3 lamp_intensity;

    bool initlized;
    Shader& shader;
    /// bbox[0] represents min values, bbox[1] represents max vals
    glm::vec3 bbox[2];

    SceneEntity* race_track;
    std::vector<SceneEntity*> trees;
    std::vector<SceneEntity*> lamps;
    std::vector<SceneEntity*> buildings;
    SceneEntity* ground;
    Car* car;

    glm::vec3 photo_pos;
    glm::vec3 observer_pos;

    World(const std::string& file_name, Shader& s);
    ~World();

    /// Initlize the meshes, this prevents calls to GL before it is ready.
    void init();

    inline void render() {
        if(!initlized) init();
        glm::mat4 objtowld = glm::mat4(); // TODO: consider making this the projection and view transform

        if(race_track) {
            QOpenGLFunctions_4_1_Core* gl =
        		  QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
            gl->glActiveTexture(GL_TEXTURE0);
            race_track->render(objtowld);
        }

        for(auto&& i : trees) i->render(objtowld);
        for(auto&& i : lamps) i->render(objtowld);
        for(auto&& i : buildings)  i->render(objtowld);
        if(ground)     ground->render(objtowld);
        if(car)        car->render(objtowld);
    }
};
