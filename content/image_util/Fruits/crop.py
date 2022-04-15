import enum
from PIL import Image, ImageChops
import os

origTileSize = 32

def are_images_equal(img1, img2):
    equal_size = img1.height == img2.height and img1.width == img2.width

    if img1.mode == img2.mode == "RGBA":
        img1_alphas = [pixel[3] for pixel in img1.getdata()]
        img2_alphas = [pixel[3] for pixel in img2.getdata()]
        equal_alphas = img1_alphas == img2_alphas
    else:
        equal_alphas = True

    equal_content = not ImageChops.difference(
        img1.convert("RGB"), img2.convert("RGB")
    ).getbbox()

    return equal_size and equal_alphas and equal_content

def image_in_array(tiles, image):
    for (i, tile) in enumerate(tiles):
        if(are_images_equal(tile, image)):
            return i
    return None


imageList = [
    ("Apple", 8, 8, 5, 9),
    ("Bananas", 7, 7, 5, 8),
    ("Cherries", 7, 7, 7, 7),
    ("Kiwi", 8, 8, 8, 8),
    ("Melon", 5, 5, 8, 8),
    ("Orange", 8, 4, 8, 8),
    ("Pineapple", 9, 9, 6, 5),
    ("Strawberry", 9, 9, 8, 8),
    ("Collected", 5, 5, 5, 5)
]

spriteSheet = []
spriteData = ""
tileCount = 0
spriteSheetWidth = 0
spriteSheetHeight = 0
posx = 0
posy = 0
for props in imageList:
    filename, cropLeft, cropRight, cropTop, cropBottom = props
    img = Image.open(filename + ".png")
    width, height = img.size
    tileWidth = origTileSize-cropLeft-cropRight
    tileHeight = origTileSize-cropTop-cropBottom
    animSeq = []
    sprites = []
    for i in range(0, width, origTileSize):
        imgTile = img.crop((i + cropLeft, cropTop, i + origTileSize - cropRight, height - cropBottom))
        index = image_in_array(sprites, imgTile)
        if(index == None):
            sprites.append(imgTile)
            animSeq.append(tileCount + len(sprites) - 1)
        else:
            animSeq.append(tileCount + index)

    posx = 0
    spriteWidth = tileWidth*len(sprites)
    spriteSheetWidth = spriteWidth if spriteWidth > spriteSheetWidth else spriteSheetWidth
    spriteSheetHeight += tileHeight
    sprite = Image.new(mode='RGBA', size=(spriteWidth, tileHeight), color=(0,0,0,0))
    for tile in sprites:
        sprite.paste(tile,(posx, 0))
        spriteData += "\t{" + hex(tileCount) + ", {{" + str(posx) + ", " + str(posy) + ", " + str(tileWidth) + ", " + str(tileHeight) + "}}},\n"
        posx += tileWidth
        tileCount += 1
    print("\t{", *animSeq, "},", sep=',')
    spriteSheet.append(sprite)
    posy += tileHeight

print("const std::map<int16_t, std::array<uint16_t, 5> > fruits_sprite_data = {" )
print(spriteData)
print("};")

pos = 0
spriteSheetImg = Image.new(mode='RGBA', size=(spriteSheetWidth, spriteSheetHeight), color=(0,0,0,0))
for ssImg in spriteSheet:
    spriteSheetImg.paste(ssImg, (0, pos))
    width, height = ssImg.size
    pos += height

spriteSheetImg.save("fruits.png")
