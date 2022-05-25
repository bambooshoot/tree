import maya.cmds as cmds
import pymel.core as pm
import maya.api.OpenMaya as om

import speedTreeRawXml as strx
import skelTreeCreator as stc

import importlib
importlib.reload(strx)
importlib.reload(stc)

# import json

def meshToTriMesh(meshShape):
    node=pm.PyNode(meshShape)
    transNode = node.parent(0)
    worldMtx = transNode.getMatrix(worldSpace=True)
    triCounts, triIds = node.getTriangles()
    triVertices=[]
    triId=0
    for triCount in triCounts:
        for locTriId in range(triCount):
            triVertices.append([ triIds[triId * 3 + i] for i in range(3) ])
            triId += 1

    pnts=[]
    for p in node.getPoints():
        p *= worldMtx
        pnts.append([p.x, p.y, p.z])

    trimsh = trimesh.Trimesh(np.array(pnts), np.array(triVertices))
    return trimsh

def createSpine(spine):
    cmds.select( d=True )
    idx = 0
    beginJointNode = None
    rootJoint = None
    for p in spine["pnt"]:
        jointNode = cmds.joint(p=p)
        cmds.joint(jointNode, e=True, radius=0.1 )
        if idx > 0:
            cmds.joint( jointNode, beginJointNode, e=True, zso=True, oj='xyz', sao="yup" )
        else:
            rootJoint = jointNode

        beginJointNode = jointNode
        idx += 1

    return rootJoint

def createBones(bones):
    rootJointList=[]
    for bone in bones["bones"]:
        cmds.select( d=True )
        rootJoint = cmds.joint(p=bone["start"])
        cmds.joint(rootJoint, e=True, radius=0.1 )

        rootJointList.append(rootJoint)

        endJointNode = cmds.joint(p=bone["end"])
        cmds.joint( endJointNode, e=True, radius=0.1 )
        cmds.joint( endJointNode, rootJoint, e=True, zso=True, oj='xyz', sao="yup" )

    rootJointList[0] = createSpine(bones["spine"])
    return rootJointList

def createObjects(objects):
    objNameList=[]
    for obj in objects:
        fnMesh = om.MFnMesh()
        pArray = om.MPointArray()
        for p in obj["pnt"]:
            pArray.append(om.MPoint(p))

        triCounts = [3]*obj["triangleNum"]
        meshNode = fnMesh.create(
            pArray,
            triCounts,
            obj["pntIdx"],
            obj["u"], 
            obj["v"]
        )

        fnMesh.assignUVs(triCounts, obj["vtxIdx"], "map1")
        meshDagNode = om.MFnDagNode(meshNode)
        meshDagNode.setName(obj["name"])
        objNameList.append(pm.PyNode(obj["name"]).getShape().fullPathName())

    print("objects",objNameList)
    return objNameList

def createMeshes(meshes):
    objNameList=[]
    for mesh in meshes:
        fnMesh = om.MFnMesh()
        pArray = om.MPointArray()

        lod = mesh["LOD"][0]

        for p in lod["vtx"]["pnt"]:
            pArray.append(om.MPoint(p))

        triCounts = [3]*int(len(lod["triIdx"])/3)

        uvSetName = "map1"

        meshNode = fnMesh.create(
            pArray,
            triCounts,
            lod["triIdx"],
            lod["vtx"]["u"], 
            lod["vtx"]["v"]
        )

        fnMesh.assignUVs(triCounts, lod["triIdx"], uvSetName)

        meshDagNode = om.MFnDagNode(meshNode)
        meshDagNode.setName(mesh["name"])
        objNameList.append(pm.PyNode(mesh["name"]).getShape().fullPathName())

    return objNameList

def deformData(meshList, objList, rootJointList):
    meshDataList=[]
    for i in range(len(meshList)):
        mesh = meshList[i]
        obj = objList[i]
        subIdWithBoneDict = {}
        ii = 0
        for vtxId in obj["vtxIdx"]:
            boneId = max(obj["boneId"][vtxId], 0)
            pId = obj["pntIdx"][ii]
            if boneId in subIdWithBoneDict:
                subIdWithBoneDict[boneId].append(pId)
            else:
                subIdWithBoneDict[boneId]=[pId]

            ii += 1

        for boneId in range(len(subIdWithBoneDict)):
            subId = subIdWithBoneDict[boneId]
            if len(subId) > 0:
                subId = list(set(subId))
                if boneId == 0:
                    meshDataList.append([mesh, subId, rootJointList[boneId], None])
                else:
                    meshDataList.append([mesh, subId, rootJointList[boneId], mesh])

    return meshDataList

def foliageData(meshOffset, foliageData):
    for meshId in foliageData["meshId"]:
        meshId += meshOffset

    return foliageData


def create(xmlFile):
    treeData = strx.readTree(xmlFile)
    objList = createObjects(treeData["objects"])
    meshList = createMeshes(treeData["meshes"])
    rootJointList = createBones(treeData["bones"])
    meshDataList = deformData(objList, treeData["objects"], rootJointList)
    meshOffset = len(objList)
    foliageList = foliageData(meshOffset, treeData["foliages"])

    dataDict = {
        "mesh":objList + meshList,
        "chain":rootJointList,
        "deform":meshDataList,
        "foliage":foliageList
        }

    # print(dataDict["foliage"]["q"])
    # meshDataStateList = [ [meshData[0], len(meshData[1]), meshData[2], meshData[3] ] for meshData in meshDataList ]
    # data = json.dumps(meshDataStateList)
    # with open(r'D:/asunlab/github/tree/example/tree.json','w')as f:
    #     f.write(data)

    stc.skelTreeCreator(dataDict)

# import speedTreeCreator
# speedTreeCreator.create("D:/asunlab/github/infTree/example/pine.xml")