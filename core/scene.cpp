#include "scene.h"

Scene3D::Scene3D() {
    this->camera.focalLength = FRACTIONS_PER_UNIT;
}

Scene3D::~Scene3D() {
}

void Scene3D::addModel(Model3D *model) {
    this->models.push_back(model);
}

void Scene3D::render(Display *display) {
    Mat4 cameraMat = this->camera.getMat();
    Index sortArrayLength = 0;

    for(Model3D *model : this->models) {
        if(!model->visible)
            continue;

        if(sortArrayLength >= MAX_TRIANGES_DRAWN)
            break;

        Mat4 modelMat;
        if(model->updatedMatrix == false) {
            model->customTransformMatrix.applyTransform(model->transform);
            // model->updatedMatrix = true;
        }
        modelMat = model->customTransformMatrix;
        modelMat = modelMat * cameraMat;
        uint8_t indexStride = 3 + (model->textureCoordCount > 0 ? 3 : 0) + (model->normalCount > 0 ? 3 : 0);

        for (uint16_t tIndex = 0; tIndex < model->triangleCount; tIndex++) {
            Vec2 texs[3];
            Vec4 points[3];
            Unit d = 0;

            for(uint8_t i = 0; i < 3; ++i) {
                Index vertexIndex = model->triangles[tIndex * indexStride + i] * 3;
                points[i].x = model->vertices[vertexIndex + 0];
                points[i].y = model->vertices[vertexIndex + 1];
                points[i].z = model->vertices[vertexIndex + 2];
                points[i].w = FRACTIONS_PER_UNIT;

                points[i] = modelMat * points[i];
                points[i].w = points[i].z;
                d += points[i].z;
                points[i].perspectiveDivide(camera.focalLength);
                points[i] = points[i].getScreenCoords();
    
                Index texIndex = model->triangles[tIndex * indexStride + i + 3] * 2;
                texs[i].x = model->textureCoords[texIndex + 0];
                texs[i].y = model->textureCoords[texIndex + 1];
            }

            if(model->isTriangleVisible(points[2].xy(), points[1].xy(), points[0].xy()))
                this->drawTriangle(display, points[0], points[1], points[2], texs[0], texs[1], texs[2]);
        }
    }
}

void Scene3D::drawTriangle(Display *display, Vec4 p0, Vec4 p1, Vec4 p2, Vec2 t0, Vec2 t1, Vec2 t2) {
    if(p0.y == p1.y && p0.y == p2.y) return;
    if(p0.y > p1.y) {
        std::swap(p0, p1);
        std::swap(t0, t1);
    }
    if(p0.y > p2.y) {
        std::swap(p0, p2);
        std::swap(t0, t2);
    }
    if(p1.y > p2.y) {
        std::swap(p1, p2);
        std::swap(t1, t2);
    }
    Unit depth = p0.z + p1.z + p2.z;
    t0 = (t0 * FRACTIONS_PER_UNIT) / p0.z;
    t1 = (t1 * FRACTIONS_PER_UNIT) / p1.z;
    t2 = (t2 * FRACTIONS_PER_UNIT) / p2.z;

    Unit p0z = (FRACTIONS_PER_UNIT * FRACTIONS_PER_UNIT) / p0.z;
    Unit p1z = (FRACTIONS_PER_UNIT * FRACTIONS_PER_UNIT) / p1.z;
    Unit p2z = (FRACTIONS_PER_UNIT * FRACTIONS_PER_UNIT) / p2.z;
 
    Unit total_height = p2.y - p0.y;
    for (Index i = 0; i < total_height; i++) { 
        bool second_half = i > p1.y-p0.y || p1.y == p0.y; 
        Unit segment_height = second_half ? p2.y-p1.y : p1.y-p0.y;
        Vec2 A = p0.xy() + ((p2.xy() - p0.xy()) * i) / total_height;
        Vec2 B; 
        if(second_half)
            B = p1.xy() + ((p2.xy() - p1.xy()) * (i - p1.y + p0.y)) / segment_height;
        else
            B = p0.xy() + ((p1.xy() - p0.xy()) * i) / segment_height;

        if (A.x > B.x) std::swap(A, B);
        for (Index j = A.x; j < B.x; j++) {
            Index y = p0.y + i;

            Vec2 p = Vec2(j, y);
            Vec4 b = this->baryCentric(p0.xy(), p1.xy(), p2.xy(), p);
            Vec2 uv = interpolate(t0, t1, t2, b.x, b.y, b.z);

            Unit z = ((b.x * p0z) + (b.y * p1z) + (b.z * p2z)) / FRACTIONS_PER_UNIT;
            uv = (uv * FRACTIONS_PER_UNIT) / z;

            Color c = modelSprite.getColor(uv);
            display->setPixel(p, c, 255);
        }
    } 
}

Vec2 Scene3D::interpolate(Vec2 t0, Vec2 t1, Vec2 t2, Unit u, Unit v, Unit w) {
    return ((t0 * u) + (t1 * v) + (t2 * w)) / (Unit)FRACTIONS_PER_UNIT;
}

Vec4 Scene3D::baryCentric(Vec2 a, Vec2 b, Vec2 c, Vec2 p) {
    Vec2 v0 = b - a;
    Vec2 v1 = c - a;
    Vec2 v2 = p - a;

    Unit d00 = v0.dot(v0);
    Unit d01 = v0.dot(v1);
    Unit d11 = v1.dot(v1);
    Unit d20 = v2.dot(v0);
    Unit d21 = v2.dot(v1);
    Unit denom = ((d00 * d11) / FRACTIONS_PER_UNIT) - ((d01 * d01) / FRACTIONS_PER_UNIT);

    Vec4 result;
    result.y = (d11 * d20 - d01 * d21) / denom;
    result.z = (d00 * d21 - d01 * d20) / denom;
    result.x = FRACTIONS_PER_UNIT - result.y - result.z;
    return result;
}

Unit edgeFunction(Vec2 a, Vec2 b, Vec2 c) { 
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x); 
} 