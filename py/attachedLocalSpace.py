# attachedLocalSpace
# 
# closestPolygonAndWeights
#   input:
#     p     a point in world space
#     mesh  a mesh node the p attach to
#   
#   return:
#     the vertex id list of polygon nearest to p
#     vertex weights with which the point on nearest polygon can be combined.
#     

import pymel.core as pm
import pymel.core.datatypes as dt
import trimesh
import numpy as np
import skeletor as sk
import maya.cmds as cmds

def closestPolygonAndWeights(p, mesh):
    meshNode = pm.PyNode(mesh)
    parentNode = meshNode.parent(0)
    invWorldMat = parentNode.worldInverseMatrix.get()
    worldMat = parentNode.worldMatrix.get()

    wp = p * invWorldMat
    cp, polyId = meshNode.getClosestPoint(wp)
    cp *= worldMat

    vIds = meshNode.getPolygonVertices(polyId)
    vpList = [meshNode.getPoint(i) * worldMat for i in vIds]
    pNum = len(vpList)

    bTb = [vpList[i] * vpList[j] for i in range(pNum) for j in range(pNum)]
    y = [p * cp for p in vpList]
    np_bTb = np.array(bTb)
    np_bTb = np_bTb.reshape(pNum,pNum)
    np_y = np.array(y)
    w = np.linalg.lstsq(np_bTb, np_y,rcond=None)[0]

    return vIds, w

def makeAttachLocalSpace(attachedTransNode, mesh, pIds, wList):
    if not cmds.pluginInfo("skelTree",q=True,l=True):
        cmds.loadPlugin("skelTree")

    node = cmds.createNode("attachedLocalSpace")
    cmds.connectAttr("{}.outMesh".format(mesh),"{}.inMesh".format(node))

    transNode = cmds.listRelatives(mesh, p=True)[0]
    cmds.connectAttr("{}.worldMatrix".format(transNode),"{}.inWorldMatrix".format(node))
    for i in range(len(pIds)):
        cmds.setAttr("{}.mInPList[{}].inPntId".format(node, i), pIds[i])
        cmds.setAttr("{}.mInPList[{}].inWeight".format(node, i), wList[i])

    rootTransNode = cmds.group(em=True, n="attachedGroup")
    cmds.connectAttr("{}.outTranslate".format(node), "{}.translate".format(rootTransNode),f=True)
    cmds.connectAttr("{}.outRotate".format(node), "{}.rotate".format(rootTransNode),f=True)
    cmds.parent(attachedTransNode, rootTransNode)

def transformAttachToMesh(transNode, mesh):
    p=cmds.xform(transNode, q=True, ws=True, t=True)
    vIds, w = closestPolygonAndWeights(dt.Point(p), mesh)
    makeAttachLocalSpace(transNode, mesh, vIds, w)

def transformAttachToMeshSel():
    transList = cmds.ls(sl=True)
    if len(transList) < 2:
        print("Two nodes should be selected! the first one should be attached node and the second one should be target mesh.")
    
    targetMesh = cmds.ls(transList[-1],dag=True,type="mesh",noIntermediate=True)[0]
    for trans in transList[:-1]:
        transformAttachToMesh(trans, targetMesh)
        
# transformAttachToMeshSel()