#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "meshmaker.h"

MultiEntity* MeshMaker::cappedCone(const SceneEntity& e) {
    Cone* cone = std::dynamic_pointer_cast<Cone>(e.mesh).get();
    if(cone == nullptr) throw std::invalid_argument("drawCappedCone expected SceneEntity containing a mesh of type Cone");

    auto mesh = std::shared_ptr<Mesh>(
        new Disk( cone->getRadius(), cone->getSlices() ) // Has same radius and num slices
    );
    auto transform = std::make_shared<glm::mat4>(
        glm::rotate(*e.transform, glm::pi<float>(), glm::vec3(1, 0, 0))
    );

    return new MultiEntity(
        e.shader,           // Reference to shader
        mesh,               // The disk
        transform,          // The postion and orientation of the disk
        e.material,         // The material is the same
        new MultiEntity(e)  // Group the disk and cylinder
    );
}

MultiEntity* MeshMaker::cappedCylider(const SceneEntity& e) {
    Cylinder* cylinder = std::dynamic_pointer_cast<Cylinder>(e.mesh).get();
    if(cylinder == nullptr) throw std::invalid_argument("drawCappedCylider expected SceneEntity containing a mesh of type Cylinder");

    // Both will use the same mesh
    auto mesh = std::shared_ptr<Mesh>(
        // Has same radius and num slices
        new Disk( cylinder->getRadius(), cylinder->getSlices() )
    );

    auto transform1 = std::make_shared<glm::mat4>(
        // The bottom cap just needs to be flipped
        glm::rotate(*e.transform, glm::pi<float>(), glm::vec3(1, 0, 0))
    );

    auto transform2 = std::make_shared<glm::mat4>(
        // The top cap just needs to be translated
        glm::translate(*e.transform, glm::vec3(0, 0, cylinder->getHeight()))
    );

    MultiEntity* t = new MultiEntity( // Bottom cap and Cylinder only
        e.shader,           // Reference to shader
        mesh,               // The disk
        transform1,         // The postion and orientation of the disk
        e.material,         // The material is the same
        new MultiEntity(e)  // Group the disk and cylinder
    );
    return new MultiEntity( // Everything
        e.shader,
        mesh,
        transform2,
        e.material,
        t                   // Group Cylinder with the Bottom and Top cap
    );
}

MultiEntity* MeshMaker::tree(Shader& s, float h, std::shared_ptr<Material> trunk, std::shared_ptr<Material> top) {
    MultiEntity* tree = MeshMaker::cappedCone( SceneEntity(s, std::make_shared<Cone>(1.0f, h / 1.5f, 16), nullptr, top) );
    auto transform = std::make_shared<glm::mat4>(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, h / -3.0f)));
    tree = new MultiEntity(s, std::make_shared<Cylinder>(0.4f, h / 3.0f, 8), transform, trunk, tree);
    tree->objtowld = // Move group so it is correctly oriented
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, h / 3.0f, 0.0f)) *                  // Move it out of the ground
        glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::vec3(-1.0f, 0.0f, 0.0f));  // Make it face upwards
    return tree;
}

MultiEntity* MeshMaker::lamp(Shader& s, float h, std::shared_ptr<Material> post, std::shared_ptr<Material> top) {

    MultiEntity* lamp = new MultiEntity(s, std::make_shared<Cube>(0.5f), nullptr, top);
    auto transform = std::make_shared<glm::mat4>(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f - h))); //TODO: this, left off here
    lamp = new MultiEntity(s, std::make_shared<Cylinder>(0.1f, h - 0.5f, 8), transform, post, lamp);
    lamp->objtowld = // Move group so it is correctly oriented
        glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::vec3(-1.0f, 0.0f, 0.0f));  // Make it face upwards
    return lamp;
}

// MultiEntity* MeshMaker::basicCar(Shader& s) {
//     // The center of the car is at the origin and the length of the car is oriented
//     // along the z axis.  The bottom of the car is in the x-z plane.
//     float width  = 2.0f;  // Width of the car
//     float length = 4.0f; // Length
//     float height = 1.0f; // Height
//
//     float w2 = width  / 2.0f;
//     float l2 = length / 2.0f;
//
//     glm::vec3 base[] = {
//         glm::vec3(-w2, 0.0f, -l2),
//         glm::vec3( w2, 0.0f, -l2),
//         glm::vec3( w2, 0.0f,  l2),
//         glm::vec3(-w2, 0.0f,  l2)
//     };
//     float height_data[] = {height / 2.0f, height / 2.0f, height, height};
//
//     auto car = std::make_shared<Building>(base, height_data);
//     return new MultiEntity(SceneEntity(s, car));;
// }
