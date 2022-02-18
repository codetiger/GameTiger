from PIL import Image
import struct

im = Image.open("tiger.jpg")
im.thumbnail((128, 128))
im = im.convert('P', palette=Image.ADAPTIVE, colors=256)
im = im.convert('RGB')

imageData = bytearray()
image_height = im.size[1]
image_width = im.size[0]
print ("Image Width:%d Height:%d" % (image_width, image_height))

binoutfile = open("logo.565","wb")
pix = im.load()
for i in range(image_height):
    for j in range(image_width):
        rgbData = int(pix[j,i][2]>>3) | (int(pix[j,i][1]>>2)<<5) | (int(pix[j,i][0]>>3)<<11)
        rgbData = ((rgbData & 0xFF) << 8) | ((rgbData >> 8) & 0xFF)
        binoutfile.write(struct.pack('<BB', ((rgbData >> 8) & 0xFF), (rgbData & 0xFF)))
binoutfile.close()
