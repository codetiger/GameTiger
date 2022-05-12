#include "settingsscreen.h"
#include "../content/models.h"

SettingsScreen::SettingsScreen(void (*rcb)(int8_t menu, uint8_t option), void (*hscb)(uint32_t highscore), uint32_t highscore, uint8_t option) {
    printf("Setting screen loading...");
    this->screenId = 6;
    this->type = Type::GAME;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->option = option;
    this->scroll = 0;

    uint16_t tileWidth = 32, tileHeight = 16;
    uint8_t xCount = (DISPLAY_WIDTH / tileWidth);
    uint8_t yCount = (DISPLAY_HEIGHT / tileHeight);
    uint16_t *ts = new uint16_t[xCount*yCount];
    bool colorFlip = true;
    for (int y = 0; y < yCount; y++) {
        for (int x = 0; x < xCount; x++) {
            colorFlip = !colorFlip;
            ts[y*xCount+x] = colorFlip ? 1 : 2;
        }
        colorFlip = !colorFlip;
    }
    this->bgLayer = new TileMap(xCount, yCount, tileWidth, tileHeight, ts);

    TileInfo tinfo; 
    tinfo.color = Color(211, 156, 180);
    tinfo.type = COLORFILL;
    this->bgLayer->addTileInfo(1, tinfo);
    tinfo.color = Color(198, 151, 178);
    this->bgLayer->addTileInfo(2, tinfo);

    this->scene = new Scene3D();
    model.vertices = (Unit*)diabloVertices;
    model.vertexCount = diabloVertexCount;
    model.normals = (Unit*)diabloNormals;
    model.normalCount = diabloNormalCount;
    model.textureCoords = (Unit*)diabloTextureCoords;
    model.textureCoordCount = diabloTextureCoordCount;
    model.triangles = (Index*)diabloFaces;
    model.triangleCount = diabloFaceCount;
    // model.vertices = (Unit*)cubeVertices;
    // model.vertexCount = cubeVertexCount;
    // model.normals = (Unit*)cubeNormals;
    // model.normalCount = cubeNormalCount;
    // model.textureCoords = (Unit*)cubeTextureCoords;
    // model.textureCoordCount = cubeTextureCoordCount;
    // model.triangles = (Index*)cubeFaces;
    // model.triangleCount = cubeFaceCount;

    this->scene->camera.lookAt(model.transform.translation);
    this->scene->addModel(&model);
    this->scene->camera.transform.translation.z = FRACTIONS_PER_UNIT*2;

    printf("Done\n");
}

SettingsScreen::~SettingsScreen() {
}

void SettingsScreen::update(uint16_t deltaTimeMS) {
    uint16_t timeDiff = getTimeDiffMS(this->lastUpdate);
    if(timeDiff > 20) {
        this->scroll++;
        if(this->scroll >= 800)
            this->scroll = 0;
        this->lastUpdate = getTime();
    }
    model.transform.rotation.y++;
    if(model.transform.rotation.y > 512)
        model.transform.rotation.y = 0;
    // model.transform.rotation.z++;
    // if(model.transform.rotation.z > 512)
    //     model.transform.rotation.z = 0;
}

void SettingsScreen::draw(Display *display) {
    display->clear(BLACKCOLOR);
    this->scene->render(display);
    // this->bgLayer->draw(display, 0, 0);    
}

void SettingsScreen::keyPressed(uint8_t key) {
    if(key == KEY_B)
        this->returnCallBack(this->screenId, this->option);
}

void SettingsScreen::keyReleased(uint8_t key) {
}

void SettingsScreen::keyDown(uint8_t key){
}
