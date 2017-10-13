#pragma once

#include <QOpenGLWidget>

#include "camera.h"
#include "world.h"

class RaceView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core {
    Shader shader;
    World world; // The car is in the world

    Camera chase;
    Camera photo;
    Camera observer;

    float mouse_x;
    float mouse_y;

    void orientChase();
    void orientPhoto();

    void setLightUniforms(glm::mat4 view);

protected:
    enum CameraMode {
        CHASE,
        PHOTO,
        OBSERVER
    } camera_mode;

    /// Flag enum to keep track of some specific keys
    enum Keys {
        KEYS_NONE  = 0x0000,
        KEYS_W     = 0x0001,
        KEYS_A     = 0x0002,
        KEYS_S     = 0x0004,
        KEYS_D     = 0x0008,
        KEYS_Q     = 0x0010,
        KEYS_E     = 0x0020,
        KEYS_SPACE = 0x0040,
        KEYS_SHIFT = 0x0080
    };
    int depressed_keys;

    static int getKey(Qt::Key);

    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    virtual void keyPressEvent(QKeyEvent*);
    virtual void keyReleaseEvent(QKeyEvent*);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void timerEvent(QTimerEvent*);

    virtual QSize sizeHint() const { return QSize(800, 600); }

public:
    RaceView() : world("race.json", shader) { setFocusPolicy(Qt::FocusPolicy::StrongFocus); }
    ~RaceView() {}
};
