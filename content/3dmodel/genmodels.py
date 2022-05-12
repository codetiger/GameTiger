class OBJ:
    def __init__(self, filename, swapyz=False):
        """Loads a Wavefront OBJ file. """
        self.vertices = []
        self.normals = []
        self.texcoords = []
        self.faces = []

        material = None
        for line in open(filename, "r"):
            if line.startswith('#'): continue
            values = line.split()
            if not values: continue
            if values[0] == 'v':
                v = map(float, values[1:4])
                if swapyz:
                    v = v[0], v[2], v[1]
                self.vertices.append(v)
            elif values[0] == 'vn':
                v = map(float, values[1:4])
                if swapyz:
                    v = v[0], v[2], v[1]
                self.normals.append(v)
            elif values[0] == 'vt':
                self.texcoords.append(map(float, values[1:3]))
            elif values[0] in ('usemtl', 'usemat'):
                material = values[1]
            elif values[0] == 'mtllib':
                self.mtl = MTL(values[1])
            elif values[0] == 'f':
                face = []
                texcoords = []
                norms = []
                for v in values[1:]:
                    w = v.split('/')
                    face.append(int(w[0]))
                    if len(w) >= 2 and len(w[1]) > 0:
                        texcoords.append(int(w[1]))
                    else:
                        texcoords.append(0)
                    if len(w) >= 3 and len(w[2]) > 0:
                        norms.append(int(w[2]))
                    else:
                        norms.append(0)
                self.faces.append((face, norms, texcoords, material))


file = open("../models.h", "w")

modelList = [("bunny.obj", "bunny", 1), ("diablo.obj", "diablo", 512), ("cube.obj", "cube", 512)]
for modelFile, modelName, scale in modelList:
    object = OBJ(modelFile)

    if(len(object.vertices) > 0):
        file.write("const Unit " + modelName + "Vertices[] = {")
        for v in object.vertices:
            v = list(v)
            file.write(str(int(v[0]*scale)) + "," + str(int(v[1]*scale)) + "," + str(int(v[2]*scale)) + ",")
        file.write("};\n")
    file.write("const Index " + modelName + "VertexCount = " + str(len(object.vertices)) + ";\n\n")

    if(len(object.texcoords) > 0):
        file.write("const Unit " + modelName + "TextureCoords[] = {")
        for t in object.texcoords:
            t = list(t)
            file.write(str(int(t[0]*scale)) + "," + str(int(t[1]*scale)) + ",")
        file.write("};\n")
    file.write("const Index " + modelName + "TextureCoordCount = " + str(len(object.texcoords)) + ";\n\n")

    if(len(object.normals) > 0):
        file.write("const Unit " + modelName + "Normals[] = {")
        for n in object.normals:
            n = list(n)
            file.write(str(int(n[0]*scale)) + "," + str(int(n[1]*scale)) + "," + str(int(n[2]*scale)) + ",")
        file.write("};\n")
    file.write("const Index " + modelName + "NormalCount = " + str(len(object.normals)) + ";\n\n")

    if(len(object.faces) > 0):
        file.write("const Index " + modelName + "Faces[] = {")
        for f in object.faces:
            vIndex = f[0]
            file.write(str(vIndex[0]-1) + "," + str(vIndex[1]-1) + "," + str(vIndex[2]-1) + ",")
            if(len(object.texcoords) > 0):
                tIndex = f[2]
                print(tIndex)
                file.write(str(tIndex[0]-1) + "," + str(tIndex[1]-1) + "," + str(tIndex[2]-1) + ",")
            if(len(object.normals) > 0):
                nIndex = f[1]
                print(nIndex)
                file.write(str(nIndex[0]-1) + "," + str(nIndex[1]-1) + "," + str(nIndex[2]-1) + ",")
        file.write("};\n")
    file.write("const Index " + modelName + "FaceCount = " + str(len(object.faces)) + ";\n\n")


file.close()