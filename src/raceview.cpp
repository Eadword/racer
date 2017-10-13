#include "raceview.h"

#include <string>

#include <QDebug>
#include <QKeyEvent>

#include <glm/gtc/matrix_transform.hpp>

void RaceView::orientChase() {
    glm::vec3 pos = world.car->getPosition();
    glm::vec3 dir = world.car->getDirection();
    glm::vec3 at = pos + dir * 10.0f; // have it look at something ahead of the car
    pos += dir * -6.0f; // have it positioned behind car
    pos.y = 3.0f; // have it positioned above car
    chase.orient(pos, at, glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setUniform("view", chase.getViewMatrix());
}

void RaceView::orientPhoto() {
    photo.orient(world.photo_pos, world.car->getPosition(), glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setUniform("view", chase.getViewMatrix());
}


void RaceView::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(0.420f, 0.824f, 1.0f, 1.0f);

    try {
        shader.compileStageFile("shaders/flat.vert");
        shader.compileStageFile("shaders/flat.frag");
        shader.link();
        shader.use();
    } catch(ShaderException &e) {
        qFatal("Shader exception: %s \n%s", e.what(), e.getOpenGLLog().c_str());
    }

    // Enable the depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    world.init();

    camera_mode = CHASE;

    observer.orient( //initial observer position
        world.observer_pos,
        world.car->getPosition(), // at
        glm::vec3(0.0f, 1.0f, 0.0f) // up
    );
    orientChase();
    orientPhoto();

    depressed_keys = KEYS_NONE;
    startTimer(1000.0f / 60.0f);
}

void RaceView::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    float a = (float)w / (float)h;
    chase.setAspect(a);
    photo.setAspect(a);
    observer.setAspect(a);
}

void RaceView::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 proj;
    glm::mat4 view;

    switch(camera_mode) {
    case CHASE:
        proj = chase.getProjectionMatrix();
        view = chase.getViewMatrix();
        break;
    case PHOTO:
        proj = photo.getProjectionMatrix();
        view = photo.getViewMatrix();
        break;
    case OBSERVER:
        proj = observer.getProjectionMatrix();
        view = observer.getViewMatrix();
        break;
    }
    shader.setUniform("proj", proj);
    shader.setUniform("view", view);

    setLightUniforms(view);

    shader.setUniform("show_back_facing", false);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    shader.setUniform("black_overide", false);
    world.render();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // shader.setUniform("black_overide", true);
    // world.render();
}

int RaceView::getKey(Qt::Key key) {
    switch (key) {
        case Qt::Key_W: return KEYS_W;
        case Qt::Key_A: return KEYS_A;
        case Qt::Key_S: return KEYS_S;
        case Qt::Key_D: return KEYS_D;
        case Qt::Key_Q: return KEYS_Q;
        case Qt::Key_E: return KEYS_E;
        case Qt::Key_Space: return KEYS_SPACE;
        case Qt::Key_Shift: return KEYS_SHIFT;
        default: return KEYS_NONE;
    };
}

void RaceView::keyPressEvent(QKeyEvent* key) {
    Qt::Key k = (Qt::Key)key->key();
    switch(k) {
    case Qt::Key_1:
        camera_mode = CHASE;
        break;
    case Qt::Key_2:
        camera_mode = PHOTO;
        break;
    case Qt::Key_3:
        camera_mode = OBSERVER;
        break;

    default: // It is either a tracked key, or one which will result in 0
        depressed_keys |= getKey(k); // Sets the bit of the released key to 1
        break;
    }
}

void RaceView::keyReleaseEvent(QKeyEvent* key) {
    // Sets the bit of the released key to 0
    depressed_keys &= (~KEYS_NONE ^ getKey((Qt::Key)key->key()));
}

void RaceView::mousePressEvent(QMouseEvent* e) {
    mouse_x = e->x();
    mouse_y = e->y();
}

void RaceView::mouseMoveEvent(QMouseEvent* e) {
    if(camera_mode == OBSERVER) {
        float dif_x = e->x() - mouse_x;
        float dif_y = e->y() - mouse_y;

        observer.rotate(dif_x / 400.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        observer.pitch(dif_y / 400.0f);
    }

    mouse_x = e->x();
    mouse_y = e->y();
}

void RaceView::timerEvent(QTimerEvent*) {
    if(camera_mode == CHASE || camera_mode == PHOTO) {
        if(depressed_keys & KEYS_W) world.car->move(0.5f);
        if(depressed_keys & KEYS_S) world.car->move(-0.5f);
        if(depressed_keys & KEYS_A) world.car->turn(-0.05f);
        if(depressed_keys & KEYS_D) world.car->turn(0.05f);
    }
    else if(camera_mode == OBSERVER) {
        if(depressed_keys & KEYS_W) observer.slideXZ(0.5f * glm::vec3(0.0f, 0.0f, -1.0f));
        if(depressed_keys & KEYS_S) observer.slideXZ(0.5f * glm::vec3(0.0f, 0.0f, 1.0f));
        if(depressed_keys & KEYS_A) observer.slideXZ(0.5f * glm::vec3(-1.0f, 0.0f, 0.0f));
        if(depressed_keys & KEYS_D) observer.slideXZ(0.5f * glm::vec3(1.0f, 0.0f, 0.0f));
        if(depressed_keys & KEYS_SHIFT) observer.slideY(0.5f * glm::vec3(0.0f, -1.0f, 0.0f));
        if(depressed_keys & KEYS_SPACE) observer.slideY(0.5f * glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 view = observer.getViewMatrix();
        shader.setUniform("view", view);
    }

    if(depressed_keys) { //no change if no movement
        if(camera_mode == CHASE)
            orientChase();
        if(camera_mode == PHOTO)
            orientPhoto();
    }

    update();
}

void RaceView::setLightUniforms(glm::mat4 view) {
    // Positional lights
    glm::vec3* lamps = world.lamp_positions;

    for(unsigned int x = 0; x < 12; ++x) {
        //(buffer, 20, "lamps[%u]", x);
        glm::vec4 tmp = view * glm::vec4(lamps[x], 1.0f);
        std::string tmp2 = "lamps[" + std::to_string(x) + "]";
        shader.setUniform(tmp2.c_str(), glm::vec3(tmp));
    }
    shader.setUniform("lamp_intensity", world.lamp_intensity);

    // Directional lights
    view[3] = glm::vec4(0, 0, 0, 1.0f); //remove translation
    glm::vec4 tmp = (view * glm::vec4(world.sun_direction, 0));
    shader.setUniform("sun_direction", glm::vec3(tmp));
    shader.setUniform("sun_intensity", world.sun_intensity);
}
