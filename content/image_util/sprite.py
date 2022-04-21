from PIL import Image, ImageChops
from rectpack import newPacker

class Sprite:
    tiles = []
    packer = None
    animSeq = {}

    def __init__(self):
        self.tiles = []
        self.packer = newPacker(rotation=False)
        self.animSeq = {}

    def areImagesSame(self, img1, img2):
        equal_size = img1.height == img2.height and img1.width == img2.width

        if img1.mode == img2.mode == "RGBA":
            img1_alphas = [pixel[3] for pixel in img1.getdata()]
            img2_alphas = [pixel[3] for pixel in img2.getdata()]
            equal_alphas = img1_alphas == img2_alphas
        else:
            equal_alphas = True

        equal_content = not ImageChops.difference(img1.convert("RGB"), img2.convert("RGB")).getbbox()
        return equal_size and equal_alphas and equal_content

    def imageExists(self, image):
        for i, tile in enumerate(self.tiles):
            if(self.areImagesSame(tile, image)):
                return i
        return None

    def addImage(self, img):
        index = self.imageExists(img)
        if(index == None):
            index = len(self.tiles)
            self.tiles.append(img)
            self.packer.add_rect(img.size[0], img.size[1], index)
        return index

    def addImages(self, images, name):
        for img in images:
            index = self.addImage(img)
            if name not in self.animSeq:
                self.animSeq[name] = []
            self.animSeq[name].append(index)

    def save(self, width, height, filename):
        self.packer.add_bin(width, height)
        self.packer.pack()
        rects = {}
        for rect in self.packer.rect_list():
            b, x, y, w, h, rid = rect
            rects[rid] = (x, y, w, h)
        spriteImg = None
        spriteData = ""
        if(len(rects) == len(self.tiles)):
            spriteImg = Image.new(mode='RGBA', size=(width, height), color=(0,0,0,0))
            for rid in range(len(rects)):
                x, y, w, h = rects[rid]
                spriteData += "\t" + str(x) + ", " + str(y) + ", " + str(w) + ", " + str(h) + ",\n"
                spriteImg.paste(self.tiles[rid], (x, y))
            spriteImg.save(filename + ".png")
        else:
            print("Unable to fit")
            exit(0)

        spriteImg = spriteImg.convert('RGBA')

        palette = []
        pixelStr = "const uint8_t " + filename + "PixelData[] = {\n"
        paletteStr = "const Color " + filename + "PaletteColors[] = {\n"
        alphaStr = "const uint8_t " + filename + "PaletteAlphas[] = {\n"

        pix = spriteImg.load()
        for i in range(height):
            for j in range(width):
                if pix[j,i] not in palette:
                    palette.append(pix[j,i])
                    paletteStr += "\tColor(" + hex(pix[j,i][0]) + ", " + hex(pix[j,i][1]) + ", " + hex(pix[j,i][2]) + "),\n"
                    alphaStr += "\t" + hex(pix[j,i][3]) + ",\n"
                pixelStr += " " + hex(palette.index(pix[j,i])) + ","
            pixelStr += "\n"

        paletteStr += "};\n"
        alphaStr += "};\n"
        pixelStr += "};\n"
        color_count = len(palette)

        print ("Image Width:%d Height:%d Color Count:%d" % (width, height, color_count))
        file = open("../" + filename + '.h', "w")
        file.write("const uint16_t " + filename + "_img_width = " + str(width) + ";\n")
        file.write("const uint16_t " + filename + "_img_height = " + str(height) + ";\n")
        file.write("const uint16_t " + filename + "_color_count = " + str(color_count) + ";\n\n")
        file.write(paletteStr)
        file.write(alphaStr)
        file.write(pixelStr)
        file.write("\nconst uint16_t " + filename + "SpriteData[] = {\n")
        file.write(spriteData)
        file.write("};\n")
        for key, seq in self.animSeq.items():
            if(len(seq) > 1):
                file.write("\nconst uint16_t " + key + "[" + str(len(seq)) + "] = {" + ','.join(map(str, seq)) + "};")
            else:
                file.write("\nconst uint16_t " + key + " = " + str(seq[0]) + ";")
        file.close()


def processSprite(imageList, image_name, image_width, image_height):
    sprite = Sprite()
    for props in imageList:
        filename, name, origTileWidth, origTileHeight, colorCount, cropLeft, cropRight, cropTop, cropBottom = props
        img = Image.open(filename).convert('RGBA')
        width, height = img.size
        origColorCount = 0 if img.getcolors() == None else len(img.getcolors())
        if(origColorCount != colorCount):
            img = img.convert('P', palette=Image.ADAPTIVE, colors=colorCount)
        img = img.convert('RGBA')
        colorCount = 0 if img.getcolors() == None else len(img.getcolors())
        print(filename, origColorCount, colorCount, "colours")
        tiles = []
        for iy in range(0, height, origTileHeight):
            for ix in range(0, width, origTileWidth):
                imgTile = img.crop((ix + cropLeft, iy + cropTop, ix + origTileWidth - cropRight, iy + origTileHeight - cropBottom))
                tiles.append(imgTile)
        sprite.addImages(tiles, name)
    sprite.save(image_width, image_height, image_name)
    sprite = None


allGameSpriteList = [
    ("ninjafrog/fruits/Apple.png", "AppleAnimSeq", 32, 32, 10, 8, 8, 5, 9),
    ("ninjafrog/fruits/Bananas.png", "BananasAnimSeq", 32, 32, 12, 7, 7, 5, 8),
    ("ninjafrog/fruits/Cherries.png", "CherriesAnimSeq", 32, 32, 8, 7, 7, 7, 7),
    ("ninjafrog/fruits/Kiwi.png", "KiwiAnimSeq", 32, 32, 12, 8, 8, 8, 8),
    ("ninjafrog/fruits/Melon.png", "MelonAnimSeq", 32, 32, 12, 5, 5, 8, 8),
    ("ninjafrog/fruits/Orange.png", "OrangeAnimSeq", 32, 32, 14, 8, 4, 8, 8),
    ("ninjafrog/fruits/Pineapple.png", "PineappleAnimSeq", 32, 32, 9, 9, 9, 6, 5),
    ("ninjafrog/fruits/Strawberry.png", "StrawberryAnimSeq", 32, 32, 14, 9, 9, 8, 8),
    ("ninjafrog/fruits/Collected.png", "CollectedAnimSeq", 32, 32, 6, 5, 5, 5, 5),
    ("ninjafrog/enemies/mushroomHit.png", "mushroomHitAnimSeq", 32, 32, 9, 2, 2, 11, 0),
    ("ninjafrog/enemies/mushroomIdle.png", "mushroomIdleAnimSeq", 32, 32, 9, 2, 2, 11, 0),
    ("ninjafrog/enemies/mushroomRun.png", "mushroomRunAnimSeq", 32, 32, 12, 2, 2, 11, 0),
    ("ninjafrog/enemies/bunnyHit.png", "bunnyHitAnimSeq", 34, 44, 10, 0, 0, 0, 0),
    ("ninjafrog/enemies/bunnyIdle.png", "bunnyIdleAnimSeq", 34, 44, 10, 0, 0, 0, 0),
    ("ninjafrog/enemies/bunnyRun.png", "bunnyRunAnimSeq", 34, 44, 10, 0, 0, 0, 0),
    ("ninjafrog/enemies/chickenHit.png", "chickenHitAnimSeq", 32, 34, 40, 0, 0, 0, 0),
    ("ninjafrog/enemies/chickenIdle.png", "chickenIdleAnimSeq", 32, 34, 51, 0, 0, 0, 0),
    ("ninjafrog/enemies/chickenRun.png", "chickenRunAnimSeq", 32, 34, 41, 0, 0, 0, 0),
    ("ninjafrog/enemies/rinoHit.png", "rinoHitAnimSeq", 52, 34, 10, 0, 0, 4, 0),
    ("ninjafrog/enemies/rinoIdle.png", "rinoIdleAnimSeq", 52, 34, 10, 0, 0, 4, 0),
    ("ninjafrog/enemies/rinoRun.png", "rinoRunAnimSeq", 52, 34, 10, 0, 0, 4, 0),
    ("ninjafrog/background/Blue.png", "bgFrames", 64, 64, 2, 0, 0, 0, 0),
    ("ninjafrog/background/Brown.png", "bgFrames", 64, 64, 2, 0, 0, 0, 0),
    ("ninjafrog/background/Gray.png", "bgFrames", 64, 64, 2, 0, 0, 0, 0),
    ("ninjafrog/background/Green.png", "bgFrames", 64, 64, 2, 0, 0, 0, 0),
    ("ninjafrog/background/Pink.png", "bgFrames", 64, 64, 2, 0, 0, 0, 0),
    ("ninjafrog/background/Purple.png", "bgFrames", 64, 64, 2, 0, 0, 0, 0),
    ("ninjafrog/background/Yellow.png", "bgFrames", 64, 64, 2, 0, 0, 0, 0),
    ("ninjafrog/terrain/terrain.png", "terrainFrames", 16, 16, 48, 0, 0, 0, 0),
    ("ninjafrog/hero/doublejump.png", "heroDoubleJumpAnimSeq", 32, 32, 12, 0, 0, 0, 0),
    ("ninjafrog/hero/jump.png", "heroJumpFrame", 32, 32, 12, 0, 0, 0, 0),
    ("ninjafrog/hero/fall.png", "heroFallFrame", 32, 32, 12, 0, 0, 0, 0),
    ("ninjafrog/hero/hit.png", "heroHitAnimSeq", 32, 32, 12, 0, 0, 0, 0),
    ("ninjafrog/hero/idle.png", "heroIdleAnimSeq", 32, 32, 12, 0, 0, 0, 0),
    ("ninjafrog/hero/run.png", "heroRunAnimSeq", 32, 32, 12, 0, 0, 0, 0),
    ("sweeper.png", "sweeperFrames", 16, 16, 32, 0, 0, 0, 0),
    ("tttblocks.png", "tttBlockFrames", 60, 60, 32, 0, 0, 0, 0),
]
processSprite(allGameSpriteList, "allGameSprite", 480, 480)

menuSpriteList = [
    ("menu/snake.png", "menuItemFrames", 96, 96, 16, 0, 0, 0, 0),
    ("menu/g2048.png", "menuItemFrames", 96, 96, 24, 0, 0, 0, 0),
    ("menu/tetris.png", "menuItemFrames", 96, 96, 24, 0, 0, 0, 0),
    ("menu/mine.png", "menuItemFrames", 96, 96, 24, 0, 0, 0, 0),
    ("menu/ttt.png", "menuItemFrames", 96, 96, 24, 0, 0, 0, 0),
    ("menu/ninjafrog.png", "menuItemFrames", 96, 96, 24, 0, 0, 0, 0),
    ("menu/settings.png", "menuItemFrames", 96, 96, 24, 0, 0, 0, 0),
    ("menu/about.png", "menuItemFrames", 96, 96, 24, 0, 0, 0, 0),
    ("menu/tiger.png", "tigerFrame", 128, 128, 64, 0, 0, 0, 0),
]
processSprite(menuSpriteList, "menuSprite", 320, 320)


