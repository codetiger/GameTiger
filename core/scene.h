#include "common.h"
#include "intmath.h"
#include <vector>

#ifndef _GAME_TIGER_SCENE3D_H
#define _GAME_TIGER_SCENE3D_H

typedef struct Camera {
    Unit focalLength;
    Transform3D transform;

    Camera() {
        this->focalLength = FRACTIONS_PER_UNIT;
    }

    Mat4 getMat() const {
        Mat4 mat;
        Vec4 t = transform.translation;
        t = t * -1;
        mat.makeTranslationMat(t * -1);
        Mat4 r;
        r.makeRotationMat(transform.rotation);
        r = r.transpose();
        mat = mat * r;
        return mat;
    }

    void lookAt(Vec4 pointTo) {
        Vec4 v;
        v.x = pointTo.x - transform.translation.x;
        v.y = pointTo.z - transform.translation.z;

        Unit dx = v.x;
        Unit l = v.length();
        dx = (v.x * FRACTIONS_PER_UNIT) / l;

        transform.rotation.y = -1 * intASin(dx);
        if (v.y < 0)
            transform.rotation.y = FRACTIONS_PER_UNIT / 2 - transform.rotation.y;

        v.x = pointTo.y - transform.translation.y;
        v.y = l;
        l = v.length();

        dx = (v.x * FRACTIONS_PER_UNIT) / l;
        transform.rotation.x = intASin(dx);
    }
} Camera;

enum CULLING {NONE, CLOCKWISE, COUNTERCLOCKWISE};

typedef struct Model3D {
    const Unit *vertices, *textureCoords, *normals;
    Index vertexCount, textureCoordCount, normalCount;

    const Index *triangles;
    Index triangleCount;

    uint8_t backfaceCulling;
    bool visible;

    Transform3D transform;
    Mat4 customTransformMatrix;
    bool updatedMatrix;

    Model3D() {
        visible = true;
        backfaceCulling = COUNTERCLOCKWISE;
        updatedMatrix = false;
    }

    int8_t getTriangleWinding(Vec2 p0, Vec2 p1, Vec2 p2) const {
        int32_t winding = (p1.y - p0.y) * (p2.x - p1.x) - (p1.x - p0.x) * (p2.y - p1.y);
        return winding > 0 ? 1 : (winding < 0 ? -1 : 0);
    }

    bool isTriangleVisible(Vec2 p0, Vec2 p1, Vec2 p2) const {
        if(backfaceCulling != NONE) {
            int8_t winding = getTriangleWinding(p0, p1, p2);

            if ((backfaceCulling == CLOCKWISE && winding > 0) || (backfaceCulling == COUNTERCLOCKWISE && winding < 0))
                return false;
        }

        return true;
    }
} Model3D;

class Scene3D {
private:
    std::vector<Model3D*> models;
public:
    Scene3D();
    ~Scene3D();

    void addModel(Model3D *model);
    void render(Display *display);
    void drawTriangle(Display *display, Vec4 p0, Vec4 p1, Vec4 p2, Vec2 t0, Vec2 t1, Vec2 t2);
    Vec4 baryCentric(Vec2 a, Vec2 b, Vec2 c, Vec2 p);
    Vec2 interpolate(Vec2 t0, Vec2 t1, Vec2 t2, Unit u, Unit v, Unit w);

    Camera camera;
};

#endif