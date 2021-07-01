import maya.cmds as cmds
import pymel.core as pm

import attachedLocalSpace as als

def connectMeshToTree(meshNode, treeNode):
    index = treeNode.inMeshes.get(s=True)
    meshNode.outMesh.connect(treeNode.inMeshes[index].inMesh)
    meshNode.parent(0).worldMatrix.connect(treeNode.inMeshes[index].inWorldMatrix)
    return index

def connectMeshToDeform(treeNode,chainId,meshId):
    deformIdx = treeNode.deformedMeshes.get(s=True)
    curDeformMesh = treeNode.deformedMeshes[deformIdx]
    curDeformMesh.deformedChainId.set(chainId)
    curDeformMesh.deformedMeshId.set(meshId)

def connectRootFrame(rootJoint, treeNode, idx):
    rootFrameAttr = treeNode.chains[idx].rootFrame
    rootJoint.t.connect(rootFrameAttr.offset)
    q = rootJoint.getOrientation()
    rootFrameAttr.rootQuat.set([q.x, q.y, q.z, q.w])

def connectFrame(joint, treeNode, idx, idx2):
    frame = treeNode.chains[idx].frames[idx2]
    frame.xOffset.set(joint.tx.get())
    q = joint.getOrientation()
    frame.frameQuat.set([q.x, q.y, q.z, q.w])

def connectJointHierarchyToTree(joint, treeNode, chainId, idx2):
    if idx2 == -1:
        connectRootFrame(joint, treeNode, chainId)
    else:
        connectFrame(joint, treeNode, chainId, idx2)

    for subJoint in joint.getChildren():
        connectJointHierarchyToTree(subJoint, treeNode, chainId, idx2+1)

def setTreeInputForMesh(treeNode, mesh, rootJoint, rootMesh=None):
    meshNode = pm.PyNode(mesh)

    # input meshes
    connectMeshToTree(meshNode, treeNode)

    chainId = treeNode.chains.get(s=True)

    # input rootMeshes
    if rootMesh:
        vIds, w = als.closestPolygonAndWeights(rootJoint, rootMesh)
        rootMeshId = connectMeshToTree(pm.PyNode(rootMesh), treeNode)
        attachedPointAttr = treeNode.chain[chainId].attachedPoint
        attachedPointAttr.targetMeshId.set(rootMeshId)
        for i in range(len(vIds)):
            attachedPointAttr.attachedWeights[i].attachedPointId.set(vIds[i])
            attachedPointAttr.attachedWeights[i].attachedWeight.set(w[i])

    # input joints
    connectJointHierarchyToTree(pm.PyNode(rootJoint), treeNode, chainId, -1)

    connectMeshToDeform(treeNode, 0, 0)


def skelTreeCreator(meshDataList):
    if not cmds.pluginInfo("skelTree",q=True,l=True):
        cmds.loadPlugin("skelTree")

    treeNode = pm.PyNode(cmds.createNode('skelTreeCreator'))
    visNode = pm.PyNode(cmds.createNode('skelTreeVisualization'))

    for meshData in meshDataList:
        setTreeInputForMesh(treeNode, meshData[0], meshData[1], meshData[2])

    treeNode.outSkelTreeData.connect(visNode.inSkelTreeData)

def skelTreeCreatorSel():
    meshList = cmds.ls(sl=True,dag=True,type="mesh")
    jointList = cmds.ls(sl=True,dag=True,type="joint")

    mesh = meshList[0]
    rootJoint = jointList[0]
    #rootMesh = meshList[1]

    skelTreeCreator([[mesh,rootJoint, None]])

cmds.select(['joint1','pCylinder1'])
skelTreeCreatorSel()