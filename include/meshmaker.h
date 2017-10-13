#pragma once

#include "entity.h"

namespace MeshMaker {
    /**
     * Draw the cap on a Cone.
     * @return A MultiEntity including the Cone and Disk s.
     */
    MultiEntity* cappedCone(const SceneEntity& e);

    /**
     * Draw the caps on a Cylinder.
     * @return A MultiEntity including the Cylinder and Disk s.
     */
    MultiEntity* cappedCylider(const SceneEntity& e);

    /**
     * Returns a MultiEntity representing a tree.
     *
     * @param  h The height of the tree
     */
    MultiEntity* tree(Shader& s, float h, std::shared_ptr<Material> trunk, std::shared_ptr<Material> top);

    /**
     * Returns a MultiEntity representing a lamp.
     *
     * @param  h The height of the lamp
     */
    MultiEntity* lamp(Shader& s, float h, std::shared_ptr<Material> post, std::shared_ptr<Material> top);

    /**
     * Draw a basic car with its orgin in the center.
     * @return A MultiEntity represetnign a basic Car.
     */
    //MultiEntity* basicCar(Shader& s);
};
