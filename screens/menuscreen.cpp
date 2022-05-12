#include "menuscreen.h"
#include "../core/keyboard.h"

MenuScreen::MenuScreen(void (*rcb)(int8_t menu, uint8_t option), void (*hscb)(uint32_t highscore), uint32_t highscore, uint8_t option) {
    printf("Menu screen loading...");
    this->screenId = 1;
    this->type = Type::MENU;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->currentMenuItem = highscore;
    this->selectedMenuItem = highscore;
    this->isAnimating = false;
    this->animationCounter = 0;
    this->currentOptionItem = 1;
    this->option = option;

    uint16_t tileWidth = 40, tileHeight = 40;
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
    tinfo.color = Color(253, 201, 48);
    tinfo.type = COLORFILL;
    this->bgLayer->addTileInfo(1, tinfo);
    tinfo.color = Color(252, 183, 35);
    this->bgLayer->addTileInfo(2, tinfo);
    printf("Done\n");
}

MenuScreen::~MenuScreen() {
}

void MenuScreen::update(uint16_t deltaTimeMS) {
    if(this->isAnimating) {
        this->animationCounter += 16;
        if(this->animationCounter > this->menuItemGap) {
            this->animationCounter = 0;
            this->isAnimating = false;
            this->currentMenuItem = this->selectedMenuItem;
            this->currentOptionItem = 1;
        }
    }
}

void MenuScreen::draw(Display *display) {
    this->bgLayer->draw(display, 0, 0);
    for (int i = 0; i < menuCount; i++) {
        int posx = (i-this->currentMenuItem) * this->menuItemGap;
        if(isAnimating)
            posx += (this->currentMenuItem - this->selectedMenuItem) * this->animationCounter;
        int16_t deltaGap = (abs(posx) * 32) / this->menuItemGap;
        uint8_t alpha = 255 - (deltaGap * 4) - 1;
        uint8_t size = 96 - deltaGap;
        Rect2 itemRect = Rect2(posx + (DISPLAY_WIDTH - size)/2, (DISPLAY_HEIGHT - size)/2, size, size);
        menuSprite.drawSprite(display, menuItemFrames[i], itemRect, alpha);
    }
    if(!isAnimating) {
        uint16_t width = alphanumfont.getTextWidth(this->menuItemNames[this->currentMenuItem][0], 2);
        alphanumfont.drawText(display, this->menuItemNames[this->currentMenuItem][0], Vec2((DISPLAY_WIDTH - width)/2, 170), 255, 2);
        if(this->menuItemNames[this->currentMenuItem][this->currentOptionItem].length() > 0) {
            std::string option = this->menuItemNames[this->currentMenuItem][this->currentOptionItem];
            width = alphanumfont.getTextWidth(option);
            alphanumfont.drawText(display, option, Vec2((DISPLAY_WIDTH - width)/2, 200));
        }
    }
}

void MenuScreen::keyPressed(uint8_t key) {
    // const char c[6] = {'U', 'D', 'L', 'R', 'A', 'B'};
    // printf("Key Pressed: %c\n", c[key]);
    if(this->isAnimating)
        return;

    if(key == KEY_RIGHT) {
        if(this->selectedMenuItem < this->menuCount-1) {
            this->selectedMenuItem++;
            this->isAnimating = true;
            this->audioController->beep(2000);
        }
    } else if (key == KEY_LEFT) {
        if(this->selectedMenuItem != 0) {
            this->selectedMenuItem--;
            this->isAnimating = true;
            this->audioController->beep(2500);
        }
    } else if (key == KEY_UP) {
        if(!isAnimating && this->menuItemNames[this->currentMenuItem][this->currentOptionItem-1].length() > 0 && this->currentOptionItem > 1)
            this->currentOptionItem--;
    } else if (key == KEY_DOWN) {
        if(!isAnimating && this->menuItemNames[this->currentMenuItem][this->currentOptionItem+1].length() > 0 && this->currentOptionItem < 3)
            this->currentOptionItem++;
    } else if(key == KEY_A) {
        this->returnCallBack(this->currentMenuItem, this->currentOptionItem);
    }
}

void MenuScreen::keyReleased(uint8_t key) {
    // const char c[6] = {'U', 'D', 'L', 'R', 'A', 'B'};
    // printf("Key Released: %c\n", c[key]);
}

void MenuScreen::keyDown(uint8_t key){
    // const char c[6] = {'U', 'D', 'L', 'R', 'A', 'B'};
    // printf("Key Down: %c\n", c[key]);
}
