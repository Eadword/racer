#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include <glm/gtc/matrix_transform.hpp>

#include "world.h"

#define REF(t, x) ((float)t[x].toDouble())

World::World(const std::string& file_name, Shader& s) : initlized(false), shader(s) {
    mtl_ground = std::make_shared<Material>(
        glm::vec3(0.0f),                    // Emmission
        glm::vec3(0.0f),                    // Ambient reflectivity
        glm::vec3(0.671f, 0.486f, 0.246f),  // Diffuse reflectivity
        glm::vec3(0.0f),                    // Specular reflectivity
        1.0f                                // Shine
    );
    mtl_track = std::make_shared<Material>(
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.150f, 0.150f, 0.150f),
        glm::vec3(0.1f),
        40.0f
    );
    mtl_tree = std::make_shared<Material>(
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.031f, 0.565f, 0.067f),
        glm::vec3(0.0f),
        1.0f
    );
    mtl_trunk = std::make_shared<Material>(
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.565f, 0.341f, 0.051f),
        glm::vec3(0.0f),
        1.0f
    );
    mtl_building = std::make_shared<Material>(
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.898f, 0.898f, 0.898f),
        glm::vec3(0.0f),
        1.0f
    );
    mtl_lamp = std::make_shared<Material>(
        glm::vec3(1.0f),
        glm::vec3(0.0f),
        glm::vec3(1.0f),
        glm::vec3(0.0f),
        1.0f
    );
    mtl_post = std::make_shared<Material>(
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.671f, 0.671f, 0.671f),
        glm::vec3(1.0f),
        80.0f
    );

    race_track = ground = car = nullptr; //init to null

    // Load race data
    QFile json_data(file_name.c_str());
    if( !json_data.open(QIODevice::ReadOnly) ) {
        qFatal("Unable to open %s", file_name.c_str());
    }

    QJsonParseError err;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json_data.readAll(), &err);
    QJsonObject json = jsonDoc.object();

    QJsonArray adata = json["bbox"].toArray();
    bbox[0] = glm::vec3(
        REF(adata, 0),
        REF(adata, 1),
        REF(adata, 2)
    );
    bbox[1] = glm::vec3(
        REF(adata, 3),
        REF(adata, 4),
        REF(adata, 5)
    );
    {
        glm::vec3 points[4] = {
            glm::vec3(bbox[1].x, bbox[0].y - 1e-3, bbox[0].z),
            glm::vec3(bbox[1].x, bbox[0].y - 1e-3, bbox[1].z),
            glm::vec3(bbox[0].x, bbox[0].y - 1e-3, bbox[1].z),
            glm::vec3(bbox[0].x, bbox[0].y - 1e-3, bbox[0].z)
        };
        ground = new SceneEntity(
            shader,
            std::make_shared<Quad>(points),
            nullptr,
            mtl_ground
        );
    }

    race_track = new SceneEntity(
        shader,std::make_shared<Track>(json["track"].toObject()),
        nullptr, mtl_track, true
    );

    adata = json["trees"].toArray();
    for(auto&& i : adata) {
        QJsonObject t1 = i.toObject();
        MultiEntity* tree = MeshMaker::tree(shader, REF(t1, "height"), mtl_trunk, mtl_tree);

        QJsonArray t2 = t1["position"].toArray();
        glm::vec3 pos(
            REF(t2, 0),
            REF(t2, 1),
            REF(t2, 2)
        );
        tree->objtowld = glm::translate(glm::mat4(), pos) * tree->objtowld;
        trees.push_back(tree);
    }

    adata = json["buildings"].toArray();
    for(auto&& i : adata) {
        QJsonArray t = i.toObject()["outline"].toArray();
        glm::vec3 points[4] = {
            glm::vec3(REF(t, 0), REF(t, 1), REF(t, 2)),
            glm::vec3(REF(t, 4), REF(t, 5), REF(t, 6)),
            glm::vec3(REF(t, 8), REF(t, 9), REF(t, 10)),
            glm::vec3(REF(t, 12), REF(t, 13), REF(t, 14))
        };
        float heights[4] = { REF(t, 3), REF(t, 7), REF(t, 11), REF(t, 15) };
        buildings.push_back(
            new SceneEntity(shader, std::make_shared<Building>(points, heights), nullptr, mtl_building)
        );
    }

    car = new Car(shader);
    {
        adata = json["startPYR"].toArray();
        glm::vec3 pyr(
            REF(adata, 0),
            REF(adata, 1),
            REF(adata, 2)
        );
        adata = json["startPosition"].toArray();
        glm::vec3 pos(
            REF(adata, 0),
            REF(adata, 1),
            REF(adata, 2)
        );
        car->updateMobVals(&pyr, &pos);
    }

    adata = json["photoPosition"].toArray();
    photo_pos = glm::vec3(
        REF(adata, 0),
        REF(adata, 1),
        REF(adata, 2)
    );

    adata = json["observerPosition"].toArray();
    observer_pos = glm::vec3(
        REF(adata, 0),
        REF(adata, 1),
        REF(adata, 2)
    );

    adata = json["weather"].toObject()["sunLightDirection"].toArray();
    sun_direction = glm::vec3(
        REF(adata, 0),
        REF(adata, 1),
        REF(adata, 2)
    );

    adata = json["lamps"].toArray();
    unsigned int x = 0;
    for(auto&& i : adata) {
        QJsonObject t = i.toObject();
        QJsonArray pos = t["position"].toArray();
        lamp_positions[x] = glm::vec3(
            REF(pos, 0),
            REF(t, "height") - 0.5f,
            REF(pos, 2)
        );

        MultiEntity* lamp = MeshMaker::lamp(shader, 4, mtl_post, mtl_lamp);
        lamp->objtowld = glm::translate(glm::mat4(), lamp_positions[x++]) * lamp->objtowld;
        lamps.push_back(lamp);
    }

    adata = json["lampIntensity"].toArray();
    lamp_intensity = glm::vec3(
        REF(adata, 0),
        REF(adata, 1),
        REF(adata, 2)
    );

    adata = json["weather"].toObject()["sunIntensity"].toArray();
    sun_intensity = glm::vec3(
        REF(adata, 0),
        REF(adata, 1),
        REF(adata, 2)
    );
}

World::~World() {
    delete race_track;
    for(auto&& i : trees) delete i;
    for(auto&& i : lamps) delete i;
    for(auto&& i : buildings) delete i;
    delete ground;
    delete car;
}

void World::init() {
    if(initlized) return;
    race_track->initMesh();
    for(auto&& i : trees) i->initMesh();
    for(auto&& i : lamps) i->initMesh();
    for(auto&& i : buildings) i->initMesh();
    ground->initMesh();
    car->initMesh();

    shader.setUniform("normal_map", 0);

    initlized = true;
}
