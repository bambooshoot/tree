import sys
import math
import xml.dom.minidom as xmldom

def xmlElementToType(node, tag, typeCast):
    return [typeCast(elt) for elt in node.getElementsByTagName(tag)[0].firstChild.toxml().split()]

def meshVertices(lod):
    vtx = lod.getElementsByTagName("Vertices")[0]
    dataList = [ xmlElementToType(vtx, tag, float) for tag in ["X","Y","Z","TexcoordU","TexcoordV"] ]

    return {
        "pnt":list(zip(dataList[0],dataList[1],dataList[2])),
        "u":dataList[3],
        "v":dataList[4]
    }

def meshQuadIndices(lod):
    return xmlElementToType(lod, "QuadIndices", int)

def meshData(mesh):
    return {
        "id":mesh.attributes["ID"].value,
        "name":mesh.attributes["Name"].value,
        "LOD":[{
            "level":float(lod.attributes["Level"].value),
            "scale":float(lod.attributes["OriginalScale"].value),
            "quadIdx":meshQuadIndices(lod),
            "vtx":meshVertices(lod)} for lod in mesh.getElementsByTagName("LOD")]
        }

def readMeshes(doc):
    meshes = doc.getElementsByTagName('Meshes')
    print(meshes[0].firstChild.toxml())
    meshList = []
    
    for mesh in meshes[0].getElementsByTagName('Mesh'):
        meshList.append(meshData(mesh))

    return meshList

def readFoliages(doc):
    LeafReferences = doc.getElementsByTagName('LeafReferences')[0]
    pntList = [ xmlElementToType(LeafReferences, tag, float) for tag in ["X","Y","Z"] ]
    rotAxisList = [ xmlElementToType(LeafReferences, tag, float) for tag in ["RotAxisX","RotAxisY","RotAxisZ"] ]

    return {
        "pnt":list(zip(*pntList)),
        "rotAxis":list(zip(*rotAxisList)),
        "rotRadian":[math.radians(angle) for angle in xmlElementToType(LeafReferences, "RotAngle", float)],
        "scale":xmlElementToType(LeafReferences, "Scale", float),
        "meshId":xmlElementToType(LeafReferences, "MeshID", int),
        "boneId":xmlElementToType(LeafReferences, "BoneID", int)
    }

def boneData(bone):
    return {
        "id":int(bone.attributes["ID"].value),
        "parentId":int(bone.attributes["ParentID"].value),
        "radius":float(bone.attributes["Radius"].value),
        "mass":float(bone.attributes["Mass"].value),
        "start":[float(bone.attributes["StartX"].value),float(bone.attributes["StartY"].value),float(bone.attributes["StartZ"].value)],
        "end":[float(bone.attributes["EndX"].value),float(bone.attributes["EndY"].value),float(bone.attributes["EndZ"].value)]
    }

def spineData(doc):
    spine = doc.getElementsByTagName('Spine')[0]
    pntList = [ xmlElementToType(spine, tag, float) for tag in ["X","Y","Z"] ]
    return {
        "pnt":list(zip(*pntList)),
        "radius":xmlElementToType(spine, "Radius", float)
    }

def readBones(doc):
    return {
        "spine":spineData(doc),
        "bones":[boneData(bone) for bone in doc.getElementsByTagName('Bone')]
        }

def readObjects(doc):
    objList = []
    for obj in doc.getElementsByTagName('Object'):
        objData={}
        objData['name'] = obj.attributes["Name"].value
        objData['id'] = int(obj.attributes["ID"].value)
        
        points = obj.getElementsByTagName('Points')[0]
        objData['pntNum'] = int(points.attributes["Count"].value)
        pntList = [ xmlElementToType(points, tag, float) for tag in ["X","Y","Z"] ]
        objData['pnt'] = list(zip(*pntList))

        vtx = obj.getElementsByTagName('Vertices')[0]
        objData['u'] = xmlElementToType(vtx, "TexcoordU", float)
        objData['v'] = xmlElementToType(vtx, "TexcoordV", float)

        objData["boneId"] = xmlElementToType(vtx, "BoneID", int)

        tri = obj.getElementsByTagName('Triangles')[0]
        objData["triangleNum"] = int(tri.attributes["Count"].value)
        objData["pntIdx"] = xmlElementToType(tri, "PointIndices", int)
        objData["vtxIdx"] = xmlElementToType(tri, "VertexIndices", int)

        objList.append(objData)

    return objList

def readTree(xmlFile):
    doc = xmldom.parse(xmlFile)
    return {
        "objects":readObjects(doc),
        "meshes":readMeshes(doc),
        "foliages":readFoliages(doc),
        "bones":readBones(doc)
    }

if __name__ == "__main__":
    readTree(sys.argv[1])
