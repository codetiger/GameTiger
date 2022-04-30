#include "level.h"

Level::Level() {
    this->bgScroll = 0;
    this->gameScrollX = 0;
    this->gameScrollY = 0;
    this->bgLayer = 0;
    this->gameLayer = 0;
    hero.inertia = 0;
}

Level::~Level() {
}

void Level::updateGameItem(GameItem &item) {
    if(item.state == DISAPPEAR)
        return;

    item.curFrameIndex++;
    if(item.state == HIT && item.curFrameIndex >= item.numHitFrames) {
        item.curFrameIndex = 0;
        item.state = DISAPPEAR;
    } else if(item.state == IDLE && item.curFrameIndex >= item.numIdleFrames) {
        item.curFrameIndex = 0;
        if(item.movementType == HORIZONTAL || item.movementType == VERTICAL) {
            item.state = MOVING;
            item.direction = !item.direction;
        }
    } else if(item.state == MOVING && item.curFrameIndex >= item.numRunFrames) {
        item.curFrameIndex = 0;
    }
    
    if(item.state == MOVING) {
        if(item.movementType == HORIZONTAL) {
            int8_t delta = item.speed * (item.direction ? 1 : -1);
            item.x = item.x + delta;
            item.x = std::max(item.minAxis, std::min(item.x, item.maxAxis));
            if(item.x == item.minAxis || item.x == item.maxAxis) {
                item.curFrameIndex = 0;
                item.state = IDLE;
            }
        }
    }
}

bool Level::checkGameItemCollision(GameItem &item) {
    if(hero.x + hero.width - 6 < item.x || hero.x + 4 > item.x + item.width)
        return false;
    if(hero.y + hero.height < item.y || hero.y + 4 > item.y + item.height) 
        return false;

    return true;
}

void Level::updateHeroCollision() {
    for(GameItem &item : gameItems) {
        if(item.state == DISAPPEAR || item.state == HIT) 
            continue;

        if(checkGameItemCollision(item)) {
            if(item.type == FRIEND) {
                item.state = HIT;
                item.curFrameIndex = 0;
            } else if(item.type == ENEMY) {
                uint8_t bigWidth = item.width > hero.width ? item.width : hero.width;
                if(abs(hero.x - item.x) < bigWidth && hero.y + hero.height/2 < item.y && hero.state == FALLING) {
                    item.state = HIT;
                    item.curFrameIndex = 0;
                    hero.state = JUMPING;
                    hero.curFrameIndex = 0;
                    hero.inertia = JUMPINERTIA/2;
                } else {
                    hero.state = HURT;
                    hero.inertia = JUMPINERTIA/2;
                    hero.curFrameIndex = 0;
                }
            }
        }
    }
}

bool Level::checkHeroMovable(uint8_t direction, int8_t delta) {
    if(direction == 0) {
        if(this->gameLayer->getCollisionTile(hero.x + delta + hero.width - 6, hero.y + 2) == HALLOW && 
            this->gameLayer->getCollisionTile(hero.x + delta + hero.width - 6, hero.y + hero.height - 2) == HALLOW)
            return true;
    } else if(direction == 1) {
        if(this->gameLayer->getCollisionTile(hero.x + delta + 4, hero.y + 2) == HALLOW &&
            this->gameLayer->getCollisionTile(hero.x + delta + 4, hero.y + hero.height - 2) == HALLOW)
            return true;
    } else if(direction == 2) {
        if(this->gameLayer->getCollisionTile(hero.x + 4, hero.y + hero.height + 4) == HALLOW &&
            this->gameLayer->getCollisionTile(hero.x + hero.width - 4, hero.y + hero.height + 4) == HALLOW)
            return true;
    } else if(direction == 3) {
        TileCollisionType left = this->gameLayer->getCollisionTile(hero.x + 4, hero.y + delta + 2);
        TileCollisionType right = this->gameLayer->getCollisionTile(hero.x + hero.width - 4, hero.y + delta + 2);

        if(left == HALLOW && right == HALLOW)
            return true;
        else if(left == ONEWAY && right == ONEWAY)
            return true;
        else if(left == HALLOW && right == ONEWAY)
            return true;
        else if(left == ONEWAY && right == HALLOW)
            return true;
    }
    return false;
}

void Level::updateHero() {
    hero.curFrameIndex++;
    if(hero.state == STANDING && hero.curFrameIndex >= hero.numStandingFrames) {
        hero.curFrameIndex = 0;
    } else if(hero.state == WALKING && hero.curFrameIndex >= hero.numWalkFrames) {
        hero.curFrameIndex = 0;
    } else if(hero.state == DOUBLEJUMPING && hero.curFrameIndex >= hero.numDoubleJumpFrames) {
        hero.curFrameIndex = hero.numDoubleJumpFrames - 1;
    } else if(hero.state == HURT && hero.curFrameIndex >= hero.numHurtFrames) {
        hero.curFrameIndex = hero.numHurtFrames-1;
    }
    if(hero.state == WALKING) {
        if(this->checkHeroMovable(2, 0)) {
            hero.curFrameIndex = 0;   
            hero.state = FALLING;
        } else {
            int8_t delta = 4 * (hero.direction ? -1 : 1);
            if(this->checkHeroMovable(hero.direction ? 1 : 0, delta))
                hero.x += delta;
        }
    } else if(hero.state == FALLING) {
        int8_t delta = 8;
        if(this->checkHeroMovable(2, 0))
            hero.y += delta;
        else {
            hero.curFrameIndex = 0;
            hero.state = STANDING;
        }
    } else if(hero.state == JUMPING || hero.state == DOUBLEJUMPING) {
        if(hero.inertia > 0) {
            hero.inertia--;
            int8_t delta = -8;
            if(this->checkHeroMovable(3, delta))
                hero.y += delta;
            else {
                hero.inertia = 0;
                hero.curFrameIndex = 0;
                hero.state = FALLING;
            }
        } else {
            hero.inertia = 0;
            hero.curFrameIndex = 0;
            hero.state = FALLING;
        }
    } else if(hero.state == HURT) {
        if(hero.inertia > 0) {
            hero.inertia--;
            int8_t delta = -8;
            hero.y += delta;
            hero.x += hero.direction ? 2: -2;
        } else if(hero.y < this->gameLayer->height + 50) {
            int8_t delta = 8;
            hero.y += delta;
            hero.x += hero.direction ? 2: -2;
        }
    }
}

void Level::updateScreenScroll() {
    if(hero.x <= DISPLAY_WIDTH / 2)
        this->gameScrollX = 0;
    else {
        this->gameScrollX = hero.x - DISPLAY_WIDTH/2;
        if(this->gameScrollX > (this->gameLayer->width - DISPLAY_WIDTH))
            this->gameScrollX = this->gameLayer->width - DISPLAY_WIDTH;
    }
    if(hero.y < DISPLAY_HEIGHT / 2)
        this->gameScrollY = 0;
    else {
        this->gameScrollY = hero.y - DISPLAY_HEIGHT/2;
        if(this->gameScrollY > (this->gameLayer->height - DISPLAY_HEIGHT))
            this->gameScrollY = this->gameLayer->height - DISPLAY_HEIGHT;
    }
}

void Level::update(uint16_t deltaTimeMS) {
    uint16_t timeDiff = getTimeDiffMS(this->lastUpdate);
    if(timeDiff > 50) {
        if(this->bgScroll == 0)
            this->bgScroll = this->bgMaxScroll;
        else
            this->bgScroll--;

        for(GameItem &item : gameItems)
            this->updateGameItem(item);

        this->updateHero();
        this->updateScreenScroll();
        if(hero.state != HURT)
            this->updateHeroCollision();
        this->lastUpdate = getTime();
    }
}

void Level::draw(Display *display) {
    if(this->bgLayer)
        this->bgLayer->draw(display, this->isHorizontalScroll ? this->bgScroll : 0, this->isHorizontalScroll ? 0 : this->bgScroll);
    if(this->gameLayer)
        this->gameLayer->draw(display, this->gameScrollX, this->gameScrollY);
    for(GameItem item : gameItems) {
        if(item.state != DISAPPEAR) {
            uint16_t frame;
            if(item.state == IDLE)
                frame = item.idleSeq[item.curFrameIndex];
            else if(item.state == HIT)
                frame = item.hitSeq[item.curFrameIndex];
            else if(item.state == MOVING)
                frame = item.runSeq[item.curFrameIndex];
            item.sprite->drawSprite(display, frame, Pos2(item.x-this->gameScrollX, item.y-this->gameScrollY), item.direction);
        }
    }
    uint16_t frame;
    if(hero.state == STANDING)
        frame = hero.standingSeq[hero.curFrameIndex];
    else if(hero.state == WALKING)
        frame = hero.walkSeq[hero.curFrameIndex];
    else if(hero.state == JUMPING)
        frame = hero.jumpFrame;
    else if(hero.state == DOUBLEJUMPING)
        frame = hero.doubleJumpSeq[hero.curFrameIndex];
    else if(hero.state == FALLING)
        frame = hero.fallFrame;
    else if(hero.state == HURT)
        frame = hero.hurtSeq[hero.curFrameIndex];
    hero.sprite->drawSprite(display, frame, Pos2(hero.x-this->gameScrollX, hero.y-this->gameScrollY), hero.direction);
}

void Level::setBGLayer(Image *sprite, uint16_t spriteID, bool isHSroll) {
    uint16_t tileWidth = sprite->getSpriteWidth(spriteID);
    uint16_t tileHeight = sprite->getSpriteHeight(spriteID);
    this->bgMaxScroll = tileWidth;
    this->isHorizontalScroll = isHSroll;
    uint8_t xCount = (DISPLAY_WIDTH / tileWidth) + 2;
    uint8_t yCount = (DISPLAY_HEIGHT / tileHeight) + 2;
    uint16_t *ts = new uint16_t[xCount*yCount];
    for (int i = 0; i < xCount*yCount; i++)
        ts[i] = 1;
    this->bgLayer = new TileMap(xCount, yCount, tileWidth, tileHeight, ts);

    TileInfo tinfo;
    tinfo.type = SPRITE;
    tinfo.sprite = sprite;
    tinfo.textureID = spriteID;
    this->bgLayer->addTileInfo(1, tinfo);
}

void Level::setGameLayer(Image *sprite, uint8_t xCount, uint8_t yCount, uint16_t *ts, uint16_t emptyTileIndex) {
    uint8_t tileWidth = sprite->getSpriteWidth(ts[0]);
    uint8_t tileHeight = sprite->getSpriteHeight(ts[0]);
    this->gameLayer = new TileMap(xCount, yCount, tileWidth, tileHeight, ts);
    for (int i = 0; i < xCount*yCount; i++) {
        TileInfo tinfo;
        tinfo.collisionType = (ts[i] != emptyTileIndex) ? NOPASS : HALLOW;
        if(sprite->getSpriteHeight(ts[i]) == 5)
            tinfo.collisionType = ONEWAY;
        tinfo.type = (ts[i] != emptyTileIndex) ? SPRITE : EMPTY;
        tinfo.sprite = sprite;
        tinfo.textureID = ts[i];
        this->gameLayer->addTileInfo(ts[i], tinfo);
    }
}

void Level::addGameItem(GameItem &gi) {
    if(gi.movementType != STATIC)
        gi.direction = ((rand() % 10) % 2 == 0);
    gameItems.push_back(gi);
}

void Level::keyPressed(uint8_t key) {
    if(hero.state != HURT) {
        if(key == KEY_RIGHT || key == KEY_LEFT) {
            hero.direction = (key == KEY_LEFT);
            if(hero.state == STANDING) {
                hero.state = WALKING;
                hero.curFrameIndex = 0;
            } else if(hero.state == JUMPING || hero.state == DOUBLEJUMPING || hero.state == FALLING) {
                int8_t delta = 2 * (hero.direction ? -1 : 1);
                if(this->checkHeroMovable(hero.direction ? 1 : 0, delta))
                    hero.x += delta;
            }
        } else if(key == KEY_UP) {
            if(hero.state == STANDING) {
                hero.state = JUMPING;
                hero.inertia += JUMPINERTIA;
                hero.curFrameIndex = 0;
            } else if(hero.state == JUMPING && hero.inertia < JUMPINERTIA/2) {
                hero.state = DOUBLEJUMPING;
                hero.inertia += JUMPINERTIA;
                hero.curFrameIndex = 0;
            }
        }
    }
}

void Level::keyReleased(uint8_t key) {
    if(hero.state == WALKING) {
        hero.state = STANDING;
        hero.curFrameIndex = 0;
    }
}

void Level::keyDown(uint8_t key) {
    this->keyPressed(key);
}
