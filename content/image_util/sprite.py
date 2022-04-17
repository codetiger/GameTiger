from PIL import Image, ImageChops
from rectpack import newPacker

class Sprite:
    tiles = {}
    packer = newPacker(rotation=False)
    animSeq = {}

    def __init__(self):
        pass

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
        for i, tile in self.tiles.items():
            if(self.areImagesSame(tile, image)):
                return i
        return None

    def addImage(self, img):
        index = self.imageExists(img)
        if(index == None):
            index = len(self.tiles)
            self.tiles[index] = img
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
        rects = self.packer.rect_list()
        spriteImg = None
        spriteData = ""
        if(len(rects) == len(self.tiles)):
            spriteImg = Image.new(mode='RGBA', size=(width, height), color=(0,0,0,0))
            for rect in rects:
                b, x, y, w, h, rid = rect
                spriteData += "\t{" + hex(rid) + ", {{" + str(x) + ", " + str(y) + ", " + str(w) + ", " + str(h) + "}}},\n"
                spriteImg.paste(self.tiles[rid], (x, y))
            spriteImg.save(filename + ".png")
        else:
            print("Unable to fit")
            exit(0)

        spriteImg = spriteImg.convert('P', palette=Image.ADAPTIVE) #, colors=color_count
        spriteImg = spriteImg.convert('RGBA')

        palette = []
        pixelStr = "const uint8_t " + filename + "_pixel_data[] = {\n"
        paletteStr = "const uint8_t " + filename + "_palette[] = {\n"

        pix = spriteImg.load()
        for i in range(height):
            for j in range(width):
                if pix[j,i] not in palette:
                    palette.append(pix[j,i])
                    paletteStr += " " + hex(pix[j,i][0]) + ", " + hex(pix[j,i][1]) + ", " + hex(pix[j,i][2]) + ", " + hex(pix[j,i][3]) + ",\n"
                pixelStr += " " + hex(palette.index(pix[j,i])) + ","
            pixelStr += "\n"

        paletteStr += "};\n"
        pixelStr += "};\n"
        color_count = len(palette)

        print ("Image Width:%d Height:%d Color Count:%d" % (width, height, color_count))
        file = open(filename + '.h', "w")
        file.write("const uint16_t " + filename + "_img_width = " + str(width) + ";\n")
        file.write("const uint16_t " + filename + "_img_height = " + str(height) + ";\n")
        file.write("const uint16_t " + filename + "_color_count = " + str(color_count) + ";\n\n")
        file.write(paletteStr)
        file.write(pixelStr)
        file.write("\nconst std::map<int16_t, std::array<uint16_t, 5> > " + filename + "_sprite_data = {\n")
        file.write(spriteData)
        file.write("};\n")
        for key, seq in self.animSeq.items():
            if(len(seq) > 1):
                file.write("\nconst uint16_t " + key + "[" + str(len(seq)) + "] = {" + ','.join(map(str, seq)) + "};")
            else:
                file.write("\nconst uint16_t " + key + " = " + str(seq[0]) + ";")
        file.close()


sprite = Sprite()

imageList = [
    ("ninjafrog/fruits/Apple.png", "AppleAnimSeq", 32, 32, 8, 8, 5, 9),
    ("ninjafrog/fruits/Bananas.png", "BananasAnimSeq", 32, 32, 7, 7, 5, 8),
    ("ninjafrog/fruits/Cherries.png", "CherriesAnimSeq", 32, 32, 7, 7, 7, 7),
    ("ninjafrog/fruits/Kiwi.png", "KiwiAnimSeq", 32, 32, 8, 8, 8, 8),
    ("ninjafrog/fruits/Melon.png", "MelonAnimSeq", 32, 32, 5, 5, 8, 8),
    ("ninjafrog/fruits/Orange.png", "OrangeAnimSeq", 32, 32, 8, 4, 8, 8),
    ("ninjafrog/fruits/Pineapple.png", "PineappleAnimSeq", 32, 32, 9, 9, 6, 5),
    ("ninjafrog/fruits/Strawberry.png", "StrawberryAnimSeq", 32, 32, 9, 9, 8, 8),
    ("ninjafrog/fruits/Collected.png", "CollectedAnimSeq", 32, 32, 5, 5, 5, 5),
    ("ninjafrog/enemies/mushroomHit.png", "mushroomHitAnimSeq", 32, 32, 2, 2, 11, 0),
    ("ninjafrog/enemies/mushroomIdle.png", "mushroomIdleAnimSeq", 32, 32, 2, 2, 11, 0),
    ("ninjafrog/enemies/mushroomRun.png", "mushroomRunAnimSeq", 32, 32, 2, 2, 11, 0),
    ("ninjafrog/enemies/bunnyHit.png", "bunnyHitAnimSeq", 34, 44, 0, 0, 0, 0),
    ("ninjafrog/enemies/bunnyIdle.png", "bunnyIdleAnimSeq", 34, 44, 0, 0, 0, 0),
    ("ninjafrog/enemies/bunnyRun.png", "bunnyRunAnimSeq", 34, 44, 0, 0, 0, 0),
    ("ninjafrog/enemies/chickenHit.png", "chickenHitAnimSeq", 32, 34, 0, 0, 0, 0),
    ("ninjafrog/enemies/chickenIdle.png", "chickenIdleAnimSeq", 32, 34, 0, 0, 0, 0),
    ("ninjafrog/enemies/chickenRun.png", "chickenRunAnimSeq", 32, 34, 0, 0, 0, 0),
    ("ninjafrog/enemies/rinoHit.png", "rinoHitAnimSeq", 52, 34, 0, 0, 4, 0),
    ("ninjafrog/enemies/rinoIdle.png", "rinoIdleAnimSeq", 52, 34, 0, 0, 4, 0),
    ("ninjafrog/enemies/rinoRun.png", "rinoRunAnimSeq", 52, 34, 0, 0, 4, 0),
    ("ninjafrog/background/Blue.png", "bgFrames", 64, 64, 0, 0, 0, 0),
    ("ninjafrog/background/Brown.png", "bgFrames", 64, 64, 0, 0, 0, 0),
    ("ninjafrog/background/Gray.png", "bgFrames", 64, 64, 0, 0, 0, 0),
    ("ninjafrog/background/Green.png", "bgFrames", 64, 64, 0, 0, 0, 0),
    ("ninjafrog/background/Pink.png", "bgFrames", 64, 64, 0, 0, 0, 0),
    ("ninjafrog/background/Purple.png", "bgFrames", 64, 64, 0, 0, 0, 0),
    ("ninjafrog/background/Yellow.png", "bgFrames", 64, 64, 0, 0, 0, 0),
    ("ninjafrog/terrain/terrain.png", "terrainFrames", 16, 16, 0, 0, 0, 0),
    ("ninjafrog/hero/doublejump.png", "heroDoubleJumpAnimSeq", 32, 32, 0, 0, 0, 0),
    ("ninjafrog/hero/jump.png", "heroJumpFrame", 32, 32, 0, 0, 0, 0),
    ("ninjafrog/hero/fall.png", "heroFallFrame", 32, 32, 0, 0, 0, 0),
    ("ninjafrog/hero/hit.png", "heroHitAnimSeq", 32, 32, 0, 0, 0, 0),
    ("ninjafrog/hero/idle.png", "heroIdleAnimSeq", 32, 32, 0, 0, 0, 0),
    ("ninjafrog/hero/run.png", "heroRunAnimSeq", 32, 32, 0, 0, 0, 0),
]

for props in imageList:
    filename, name, origTileWidth, origTileHeight, cropLeft, cropRight, cropTop, cropBottom = props
    img = Image.open(filename)
    width, height = img.size
    tileWidth = origTileWidth-cropLeft-cropRight
    tileHeight = origTileHeight-cropTop-cropBottom
    tiles = []
    for ix in range(0, width, origTileWidth):
        for iy in range(0, height, origTileHeight):
            imgTile = img.crop((ix + cropLeft, iy + cropTop, ix + origTileWidth - cropRight, iy + origTileHeight - cropBottom))
            tiles.append(imgTile)
    sprite.addImages(tiles, name)
    
image_name = "ninjafrogSprite"
image_width = 480
image_height = 448

sprite.save(image_width, image_height, image_name)


