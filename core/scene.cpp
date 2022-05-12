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
    this->clearDepthBuffer();
    Mat4 cameraMat = this->camera.getMat();
    Index sortArrayLength = 0;

    for(uint8_t modelIndex = 0; modelIndex < this->models.size(); modelIndex++) {
        Model3D *model = this->models[modelIndex];
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

            for(uint8_t i = 0; i < 3; ++i) {
                Index vertexIndex = model->triangles[tIndex * indexStride + i] * 3;
                points[i].x = model->vertices[vertexIndex + 0];
                points[i].y = model->vertices[vertexIndex + 1];
                points[i].z = model->vertices[vertexIndex + 2];
                points[i].w = FRACTIONS_PER_UNIT;

                points[i] = modelMat * points[i];
                points[i].w = points[i].z;
                points[i].perspectiveDivide(camera.focalLength);
                points[i] = points[i].getScreenCoords();
    
                Index texIndex = model->triangles[tIndex * indexStride + i + 3] * 2;
                texs[i].x = model->textureCoords[texIndex + 0];
                texs[i].y = model->textureCoords[texIndex + 1];
            }

            if(model->isTriangleVisible(points[2].xy(), points[1].xy(), points[0].xy()))
                this->drawTriangle(display, points[0], points[1], points[2], texs[0], texs[1], texs[2], model->needsPerspectiveCorrection);
        }
    }
}

void Scene3D::drawTriangle(Display *display, Vec4 p0, Vec4 p1, Vec4 p2, Vec2 t0, Vec2 t1, Vec2 t2, bool needsPC) {
    if(p0.y == p1.y && p0.y == p2.y) return;
    if(p0.y > p1.y) { std::swap(p0, p1); std::swap(t0, t1); }
    if(p0.y > p2.y) { std::swap(p0, p2); std::swap(t0, t2); }
    if(p1.y > p2.y) { std::swap(p1, p2); std::swap(t1, t2); }

    int32_t p0z, p1z, p2z;
    if(needsPC) {
        t0 = this->recipro(t0, p0.z);
        t1 = this->recipro(t1, p1.z);
        t2 = this->recipro(t2, p2.z);

        p0z = (FRACTIONS_PER_UNIT * FRACTIONS_PER_UNIT) / p0.z;
        p1z = (FRACTIONS_PER_UNIT * FRACTIONS_PER_UNIT) / p1.z;
        p2z = (FRACTIONS_PER_UNIT * FRACTIONS_PER_UNIT) / p2.z;
    }
 
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

            int16_t depth = (p0.z + p1.z + p2.z) / (3 * ZBUFFER_DEPTH_DIV);
            uint16_t depthIndex = y * DISPLAY_WIDTH + j;
            if(this->depthBuffer[depthIndex] > depth) {
                this->depthBuffer[depthIndex] = depth;

                Vec2 p = Vec2(j, y);
                Vec4 b = this->baryCentric(p0.xy(), p1.xy(), p2.xy(), p);
                Vec2 uv = this->interpolate(t0, t1, t2, b.x, b.y, b.z);
                Color c;

                if(needsPC) {
                    int32_t z = ((b.x * p0z) + (b.y * p1z) + (b.z * p2z)) / FRACTIONS_PER_UNIT;
                    z = (z == 0) ? 1 : z;
                    int32_t s = (int32_t)uv.x * FRACTIONS_PER_UNIT / z;
                    int32_t t = (int32_t)uv.y * FRACTIONS_PER_UNIT / z;

                    c = modelSprite.getColor(Vec2(s, t));
                } else 
                    c = modelSprite.getColor(uv);
                display->setPixel(p, c, 255);
            }
        }
    } 
}

Vec2 Scene3D::recipro(Vec2 t, int32_t z) {
    int32_t tx = (t.x * FRACTIONS_PER_UNIT) / z;
    int32_t ty = (t.y * FRACTIONS_PER_UNIT) / z;
    return Vec2(tx, ty);
}

Vec2 Scene3D::interpolate(Vec2 t0, Vec2 t1, Vec2 t2, Unit u, Unit v, Unit w) {
    int32_t tx = ((t0.x * u) + (t1.x * v) + (t2.x * w)) / FRACTIONS_PER_UNIT;
    int32_t ty = ((t0.y * u) + (t1.y * v) + (t2.y * w)) / FRACTIONS_PER_UNIT;
    return Vec2((Unit)tx, (Unit)ty);
}

Vec4 Scene3D::baryCentric(Vec2 a, Vec2 b, Vec2 c, Vec2 p) {
    Vec2 v0 = b - a;
    Vec2 v1 = c - a;
    Vec2 v2 = p - a;

    int32_t d00 = v0.dot(v0);
    int32_t d01 = v0.dot(v1);
    int32_t d11 = v1.dot(v1);
    int32_t d20 = v2.dot(v0);
    int32_t d21 = v2.dot(v1);
    int32_t denom = (d00 * d11) - (d01 * d01);

    int32_t ry = FRACTIONS_PER_UNIT * (d11 * d20 - d01 * d21) / denom;
    int32_t rz = FRACTIONS_PER_UNIT * (d00 * d21 - d01 * d20) / denom;
    int32_t rx = FRACTIONS_PER_UNIT - ry - rz;
    return Vec4(rx, ry, rz, FRACTIONS_PER_UNIT);
}

void Scene3D::clearDepthBuffer() {
    for (uint16_t y = 0; y < DISPLAY_HEIGHT; y++)
        for (uint16_t x = 0; x < DISPLAY_WIDTH; x++)
            this->depthBuffer[y * DISPLAY_WIDTH + x] = 127;
}