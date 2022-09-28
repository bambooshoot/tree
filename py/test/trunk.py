import pymel.core as pm
import pymel.core.datatypes as dt
import trimesh
import numpy as np
import skeletor as sk
import maya.cmds as cmds
import attachedLocalSpace as als
import skelTreeCreator as stc
import importlib
importlib.reload(stc)
importlib.reload(als)

def meshToTriMesh(meshShape):
    node=pm.PyNode(meshShape)
    transNode = node.parent(0)
    worldMtx = transNode.getMatrix(worldSpace=True)
    triCounts, triIds = node.getTriangles()
    print(triCounts,triIds)
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

def majorMatrix(pList):
    pArray = np.array(pList) 
    value, v = np.linalg.eig(np.cov(pArray,rowvar=False)) 
    transMatrix = [[a[0],a[1],a[2],0] for a in v.transpose()] 
    meanP = np.mean(pArray,axis=0) 
    transMatrix.append([meanP[0],meanP[1],meanP[2],1])
    return np.matrix(transMatrix)
    
def removeOverlayPoints(pList,stepLen):
    pList2=[pList[0]]
    for i in range(len(pList)-1):
        distX = abs(pList[i].x - pList2[-1].x)
        if distX > stepLen:
            pList2.append(pList[i])
            
    pList2.append(pList[-1])
    return pList2

def covMatrix(pList):
    matrixList = majorMatrix(pList).flatten().tolist()[0]
    majorMat = dt.Matrix(matrixList) 
    return majorMat

def makeJointsByMesh(mesh, sortRootP, stepLen):
    triMsh=meshToTriMesh(mesh)
    
    cont = sk.pre.fix_mesh(triMsh)
    skel = sk.skeletonize.by_wavefront(cont, waves=20, step_size=1)

    w_matrix = covMatrix(skel.vertices)
    w_invMatrix = w_matrix.inverse()
    pList = [dt.Point(p[0],p[1],p[2]) * w_invMatrix for p in skel.vertices]

    sortRootP2 = sortRootP * w_invMatrix
    pList.sort(key=lambda p : abs(p.x - sortRootP2.x) )
    pList = [ p * w_matrix for p in removeOverlayPoints(pList, stepLen) ]
    pList = [ pList[i] for i in range(len(pList)-1) if (pList[i+1] - pList[i]).length() > 1e-8 ]

    if len(pList) > 1:
        cmds.select( d=True )
        jointList = []
        for p in pList:
            jointNode = cmds.joint( p=[p.x,p.y,p.z] )
            cmds.joint(jointNode, e=True, radius=0.1 )
            if len(jointList)>0:
                cmds.joint( jointNode, jointList[-1], e=True, zso=True, oj='xyz', sao="yup" )

            jointList.append(jointNode)

        return pList, jointList[0]

    return None,None

def makeJointsByDag(dagNode, rootMesh, sortRootP, stepLen, meshDataList):
    meshNode = cmds.listRelatives(dagNode, type="mesh")[0]
    pList, rootJoint = makeJointsByMesh(meshNode, sortRootP, stepLen)
    if not pList:
        return

    meshDataList.append([meshNode, rootJoint, rootMesh])

    subNodeList = cmds.listRelatives(dagNode, type="transform")
    if subNodeList:
        for subDagNode in subNodeList:
            rootP = cmds.xform(subDagNode, q=True, ws=True, t=True)
            rootP = dt.Point(rootP)
            makeJointsByDag(subDagNode, meshNode, rootP, stepLen, meshDataList)

        
def buildTree(rootNode):
    meshDataList = []
    makeJointsByDag(rootNode, None, dt.Point(0,0,0), 0.5, meshDataList)

    from functools import reduce
    import operator

    meshList = [meshData[0] for meshData in meshDataList] + [meshData[2] for meshData in meshDataList]
    meshList = list(set(meshList))
    if None in meshList:
        meshList.remove(None)

    dataDict = {
        "mesh":meshList,
        "chain":[meshData[1] for meshData in meshDataList],
        "deform":meshDataList
        }

    # print(dataDict)

    stc.skelTreeCreator(dataDict)

buildTree("Trunk")