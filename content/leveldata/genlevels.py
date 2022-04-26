from xml.dom import minidom
import json

heroPos = []
enemyCounts = []
enemyPos = []
enemyLimits = []
enemyIndex = []
fruitCounts = []
fruitPos = []
fruitIndex = []
terrainSizes = []
terrainIndex = []

levelList = ["level01.tmx"]
for levelFile in levelList:
    levelData = {}
    dom = minidom.parse(levelFile)
    layer = dom.getElementsByTagName('layer')[0]

    terrainSizes.append(int(layer.attributes['width'].value))
    terrainSizes.append(int(layer.attributes['height'].value))

    startID = int(dom.getElementsByTagName('tileset')[1].attributes['firstgid'].value)
    ld = dom.getElementsByTagName('data')[0].firstChild.nodeValue.strip().replace('\n', '').split(',')
    terrainIndex.extend(list(map(int, ld)))

    objects = dom.getElementsByTagName('object')
    enemyCount = 0
    fruitCount = 0
    for obj in objects:
        if(int(obj.attributes['gid'].value) - startID == 40):
            heroPos.append(int(obj.attributes['x'].value))
            heroPos.append(int(obj.attributes['y'].value))
        elif(int(obj.attributes['gid'].value) - startID >= 20):
            enemyCount += 1
            enemyPos.append(int(obj.attributes['x'].value))
            enemyPos.append(int(obj.attributes['y'].value))
            enemyIndex.append(int(obj.attributes['gid'].value) - startID - 20)
            properties = obj.getElementsByTagName('property')
            minaxis = 0
            maxaxis = 0
            for props in properties:
                if(props.attributes['name'].value == "maxaxis"):
                    maxaxis = int(props.attributes['value'].value)
                elif(props.attributes['name'].value == "minaxis"):
                    minaxis = int(props.attributes['value'].value)
            enemyLimits.append(minaxis)
            enemyLimits.append(maxaxis)
        else:
            fruitCount += 1
            fruitPos.append(int(obj.attributes['x'].value))
            fruitPos.append(int(obj.attributes['y'].value))
            fruitIndex.append(int(obj.attributes['gid'].value) - startID)
    enemyCounts.append(enemyCount)
    fruitCounts.append(fruitCount)


file = open("../pa2levels.h", "w")
file.write("const uint8_t terrainSizes[] = {" + ','.join(str(x) for x in terrainSizes) + "};\n")
file.write("const uint16_t heroPos[] = {" + ','.join(str(x) for x in heroPos) + "};\n")
file.write("const uint8_t enemyCounts[] = {" + ','.join(str(x) for x in enemyCounts) + "};\n")
file.write("const uint16_t enemyPos[] = {" + ','.join(str(x) for x in enemyPos) + "};\n")
file.write("const uint8_t enemyIndex[] = {" + ','.join(str(x) for x in enemyIndex) + "};\n")
file.write("const uint16_t enemyLimits[] = {" + ','.join(str(x) for x in enemyLimits) + "};\n")
file.write("const uint8_t fruitCounts[] = {" + ','.join(str(x) for x in fruitCounts) + "};\n")
file.write("const uint16_t fruitPos[] = {" + ','.join(str(x) for x in fruitPos) + "};\n")
file.write("const uint8_t fruitIndex[] = {" + ','.join(str(x) for x in fruitIndex) + "};\n")
file.write("const uint8_t terrainIndex[] = {" + ','.join(str(x) for x in terrainIndex) + "};\n")

file.close()