#pragma once

#include <memory>

#include "shader.h"
#include "shapes.h"
#include "material.h"

/**
 * Defines an object in the scene. It links to potentially shared data like
 * meshes to reduce the system memory load.
 *
 * Eventually this will expand to include textures and materials, possibly more
 *
 * @note For simplicity of handling shared memory, this makes use of smart pointers.
 */
struct SceneEntity {
    /// Reference to shader in shapesview, this will be modified
    Shader& shader;

    /// The mesh
    std::shared_ptr<Mesh> mesh;
    /// The transformation object
    std::shared_ptr<glm::mat4> transform;
    /// @note if this is a nullptr, it is assumed the mesh will handle it
    std::shared_ptr<Material> material;
    /// not all things use a normal map
    bool normal_map;

    /**
     * @param s The shader for what is being rendered
     * @param m The mesh which will be rendered
     * @param t The mesh's tranformation from its default form
     * @param c The the material that will be used when rendering the mesh
     */
    SceneEntity(Shader& s, std::shared_ptr<Mesh> m, std::shared_ptr<glm::mat4> t = nullptr,
                std::shared_ptr<Material> c = nullptr, bool normal_map = false) :
            shader(s), mesh(m), transform(t), material(c), normal_map(normal_map) {

        if(m == nullptr) mesh = std::shared_ptr<Mesh>(new Cube(1.0f));
        if(t == nullptr) transform = std::shared_ptr<glm::mat4>(new glm::mat4(1.0f));
    }

    virtual ~SceneEntity() {}

    virtual void render(const glm::mat4& objtowld = glm::mat4()) {
        mesh->setUniform(shader);
        shader.setUniform("obj", objtowld * (*transform));
        if(material != nullptr) {
            //printf("%s\n", "Setting uniforms");
            material->setUniforms(shader);
        }
        shader.setUniform("enable_normal_map", normal_map);
        mesh->render();
    }

    virtual void initMesh() { mesh->init(); }
};

/**
 * A group of SceneEntities. They each have their own unique pointers to the data,
 * so they may be unique or not. Also, any other MultiEntities in this group,
 * should not stand alone, as then there is the potential for double-deletion.
 * Because the data is referenced by shared_ptrs, it is best just to make a
 * deep copy of the MultiEntity if that is necessary. (I will add support for a
 * deep copy if an when it becomes needed).
 *
 * @note this takes ownership of the other Entities
 */
class MultiEntity : public SceneEntity {
    MultiEntity* next; //For safty reasons, prevent outsiders from reaching this

public:
    /// This moves the entire group. Only the one stored in the master obj is considered.
    glm::mat4 objtowld;

    /// Construct from scratch
    MultiEntity(Shader& s, std::shared_ptr<Mesh> m, std::shared_ptr<glm::mat4> t = nullptr,
                std::shared_ptr<Material> c = nullptr, MultiEntity* n = nullptr,
                glm::mat4 objtowld = glm::mat4(), bool nm = false) :
            SceneEntity(s, m, t, c, nm), next(n), objtowld(objtowld) {}


    /// Construct from an existing SceneEntity
    MultiEntity(const SceneEntity& s, MultiEntity* n = nullptr) :
            SceneEntity(s), next(n), objtowld(1.0f) {}

    virtual ~MultiEntity() {
        delete next;
    }

    virtual void render(const glm::mat4& additional_transform = glm::mat4()) {
        for(MultiEntity* i = this; i != nullptr; i = i->next)
            i->SceneEntity::render(additional_transform * objtowld);
    }

    /**
     * Add another MultiEntitiy, is added right after the this object.
     * I figure since there is no signifigance to render order, why bother
     * iterating though the list.
     *
     * @param e An entity to add to the scene
     */
    virtual void push(MultiEntity* e) {
        e->next = next;
        next = e;
    }

    /**
     * Pushes a copy of the SceneEntity
     *
     * @param e An entity to add to the scene.
     */
    virtual void push(const SceneEntity& e) {
        next = new MultiEntity(e, next);
    }

    /**
     * Get the nth MultiEntitiy
     * @return The MultiEntity or nullptr if out of range
     */
    virtual MultiEntity* at(size_t index) {
        MultiEntity* i = this;
        for(size_t x = 0; (i->next != nullptr) && (x < index); i = i->next, ++x);
        return i;
    }

    virtual void initMesh() {
        for(MultiEntity* i = this; i != nullptr; i = i->next)
            i->SceneEntity::initMesh();
    }
};

/**
 * This is a Mob which can be moved around. In addition to the basic tranformations,
 * this object is also transformed by its current position and the direction it
 * is facing/traveling.
 */
class MobileEntity : public MultiEntity {
protected:
    /// The current position of this entity (assumes that it started at orgin)
    glm::vec3 position;
    /// The pitch/yaw/roll (only pitch currently supported)
    glm::vec3 pyr;
    /// The direction this object is traveling
    glm::vec3 direction;
    /// This object upward direction
    glm::vec3 up;
    /// The transformation based on location and direction
    glm::mat4 mob_transform;

public:
    /**
     * Construct from a MultiEntitiy
     * @warning This is not a deep copy
     */
    MobileEntity(MultiEntity* e) :
            MultiEntity(*e), position(0), pyr(0,0,0), direction(1.0f,0,0), up(0, 1.0f, 0) {}

    virtual ~MobileEntity() {}

    virtual void render(const glm::mat4& additional_transform = glm::mat4()) {
        MultiEntity::render(additional_transform * mob_transform);
    }

    /**
     * Set the mobile specific values.
     * @note nullptrs will have the value remain the same
     */
    virtual void updateMobVals(glm::vec3* pyr = nullptr, glm::vec3* position = nullptr, glm::vec3* up = nullptr);

    /**
     * Move t distance along direction vector
     * @param distance How far to go in the direction being faced.
     */
    virtual void move(float distance);

    /**
     * Sums yaw and angle. So somewhere between 0 and 2pi.
     * @param angle Radian incriment of turning
     * @param axis  The axis to tun along
     */
    virtual void turn(float angle);


    const glm::vec3& getPosition()  { return position; }
    const glm::vec3& getDirection() { return direction; }
    const glm::vec3& getUp()        { return up; }
};
