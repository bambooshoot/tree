import maya.cmds as cmds
import pymel.core as pm
import pymel.core.datatypes as dt

import attachedLocalSpace as als

def connectMeshToTree(meshNode, treeNode, index):
    meshNode.outMesh.connect(treeNode.inMeshes[index].inMesh)
    meshNode.parent(0).worldMatrix.connect(treeNode.inMeshes[index].inWorldMatrix)

def connectMeshToDeform(treeNode,chainId,meshId):
    deformIdx = treeNode.deformedMeshes.get(s=True)
    curDeformMesh = treeNode.deformedMeshes[deformIdx]
    curDeformMesh.deformedChainId.set(chainId)
    curDeformMesh.deformedMeshId.set(meshId)

def connectRootFrame(rootJoint, treeNode, idx, attachedMatrix):
    rootFrameAttr = treeNode.chains[idx].rootFrame
    rootP = dt.Point(rootJoint.getTranslation("world"))
    locP = rootP * attachedMatrix
    rootFrameAttr.offset.set([locP.x, locP.y, locP.z])
    q = rootJoint.getOrientation() * dt.Quaternion(attachedMatrix)
    rootFrameAttr.rootQuat.set([q.x, q.y, q.z, q.w])

def connectFrame(joint, treeNode, idx, idx2):
    frame = treeNode.chains[idx].frames[idx2]
    frame.xOffset.set(joint.tx.get())
    q = joint.getOrientation()
    frame.frameQuat.set([q.x, q.y, q.z, q.w])

def connectJointHierarchyToTree(joint, treeNode, chainId, idx2):
    connectFrame(joint, treeNode, chainId, idx2)
    for subJoint in joint.getChildren():
        connectJointHierarchyToTree(subJoint, treeNode, chainId, idx2+1)

def setTreeInputForMesh(treeNode, meshId, rootJoint, chainId, rootMesh=None, rootMeshId=None):
    attachedMatrix = dt.Matrix()
    attachedMatrix.setToIdentity()
    # input rootMeshes
    if rootMesh:
        rootP = cmds.xform(rootJoint, q=True, ws=True, t=True)
        rootP = dt.Point(rootP)
        vIds, u, v = als.closestPolygonAndWeights(rootP, rootMesh)
        attachedPointAttr = treeNode.chains[chainId].attachedPoint
        attachedPointAttr.targetMeshId.set(rootMeshId)
        attachedPointAttr.attachedPointId.set(vIds[0],vIds[1],vIds[2])
        attachedPointAttr.attachedWeight.set(u,v)
        attachedMatrix = als.closestMatrix(u, v, rootMesh, vIds)

    # input joints
    rootJointNode = pm.PyNode(rootJoint)
    attachedMatrix = attachedMatrix.inverse()
    connectRootFrame(rootJointNode, treeNode, chainId, attachedMatrix)

    for subJoint in rootJointNode.getChildren():
        connectJointHierarchyToTree(subJoint, treeNode, chainId, 0)

    connectMeshToDeform(treeNode, chainId, meshId)


def skelTreeCreator(meshDataDict):
    if not cmds.pluginInfo("skelTree",q=True,l=True):
        cmds.loadPlugin("skelTree")

    treeNode = pm.PyNode(cmds.createNode('skelTreeCreator'))
    visNode = pm.PyNode(cmds.createNode('skelTreeVisualization'))

    # input meshes
    meshList = meshDataDict["mesh"]
    idx = 0
    for mesh in meshList:
        meshNode = pm.PyNode(mesh)
        connectMeshToTree(meshNode, treeNode, idx)
        idx += 1

    deformDataList = meshDataDict["deform"]

    for meshData in deformDataList:
        setTreeInputForMesh(treeNode, meshList.index(meshData[0]), meshData[1], meshDataDict["chain"].index(meshData[1]), meshData[2], meshList.index(meshData[2]))

    treeNode.outSkelTreeData.connect(visNode.inSkelTreeData)

skelTreeCreator({
    "mesh":["pPlaneShape1", "pCylinderShape1", "pCylinderShape2"],
    "chain":["joint1","joint13"],
    "deform":[
        ["pCylinderShape1", "joint1", "pPlaneShape1"], 
        ["pCylinderShape2", "joint13", "pCylinderShape1"] 
    ]
})