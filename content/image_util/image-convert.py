import struct
from PIL import Image
from collections import defaultdict

color_count = 64
image_name = "pa2enemies"

im = Image.open(image_name + '.png').convert('RGBA')
# im = im.resize((96, 96), Image.NEAREST)
im = im.convert('P', palette=Image.ADAPTIVE, colors=color_count)
im = im.convert('RGBA')
im.save(image_name + '2.png')

image_width = im.size[0]
image_height = im.size[1]

palette = []
pixelStr = "const uint8_t " + image_name + "_pixel_data[] = {\n"
paletteStr = "const uint8_t " + image_name + "_palette[] = {\n"

pix = im.load()
for i in range(image_height):
    for j in range(image_width):
        if pix[j,i] not in palette:
            palette.append(pix[j,i])
            paletteStr += " " + hex(pix[j,i][0]) + ", " + hex(pix[j,i][1]) + ", " + hex(pix[j,i][2]) + ", " + hex(pix[j,i][3]) + ",\n"
        pixelStr += " " + hex(palette.index(pix[j,i])) + ","
    pixelStr += "\n"

paletteStr += "};\n"
pixelStr += "};\n"
color_count = len(palette)

print ("Image Width:%d Height:%d Color Count:%d" % (image_width, image_height, color_count))
file = open(image_name + '.h', "w")
file.write("const uint16_t " + image_name + "_img_width = " + str(image_width) + ";\n")
file.write("const uint16_t " + image_name + "_img_height = " + str(image_height) + ";\n")
file.write("const uint16_t " + image_name + "_color_count = " + str(color_count) + ";\n\n")

file.write(paletteStr)
file.write(pixelStr)
file.close()
