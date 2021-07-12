import maya.cmds as cmds
import pymel.core as pm
import pymel.core.datatypes as dt

import attachedLocalSpace as als

import importlib
importlib.reload(als)

def connectMeshToTree(meshNode, treeNode, index):
    meshNode.outMesh.connect(treeNode.inMeshes[index].inMesh)
    meshNode.parent(0).worldMatrix.connect(treeNode.inMeshes[index].inWorldMatrix)

def connectMeshToDeform(treeNode,chainId,meshId,subIds):
    deformIdx = treeNode.deformedMeshes.get(s=True)
    curDeformMesh = treeNode.deformedMeshes[deformIdx]
    curDeformMesh.deformedChainId.set(chainId)
    curDeformMesh.deformedMeshId.set(meshId)
    curDeformMesh.deformedPointIds.set(subIds)

def connectRootFrame(rootJoint, treeNode, idx, attachedMatrix):
    rootFrameAttr = treeNode.chains[idx].rootFrame
    rootP = dt.Point(rootJoint.getTranslation("world"))
    locP = rootP * attachedMatrix
    rootFrameAttr.offset.set([locP.x, locP.y, locP.z])
    q = rootJoint.getRotation(quaternion=True) * rootJoint.getOrientation()
    q *= dt.Quaternion(attachedMatrix)
    rootFrameAttr.rootQuat.set([q.x, q.y, q.z, q.w])

def connectFrame(joint, treeNode, idx, idx2):
    frame = treeNode.chains[idx].frames[idx2]
    frame.xOffset.set(joint.tx.get())
    q = joint.getRotation(quaternion=True) * joint.getOrientation()
    frame.frameQuat.set([q.x, q.y, q.z, q.w])

def connectJointHierarchyToTree(joint, treeNode, chainId, idx2):
    connectFrame(joint, treeNode, chainId, idx2)
    for subJoint in joint.getChildren():
        connectJointHierarchyToTree(subJoint, treeNode, chainId, idx2+1)

def inputChain(treeNode, rootJoint, chainId, attachedMatrix):
    # input joints
    rootJointNode = pm.PyNode(rootJoint)
    attachedMatrix = attachedMatrix.inverse()
    connectRootFrame(rootJointNode, treeNode, chainId, attachedMatrix)

    for subJoint in rootJointNode.getChildren():
        connectJointHierarchyToTree(subJoint, treeNode, chainId, 0)


def setTreeInputForDeformedMesh(treeNode, meshId, subIds, rootJoint, chainId, rootMesh=None, rootMeshId=None):
    attachedMatrix = dt.Matrix()
    attachedMatrix.setToIdentity()
    # print(treeNode, meshId, subIds, rootJoint, chainId)
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

    # input chain
    inputChain(treeNode, rootJoint, chainId, attachedMatrix)

    connectMeshToDeform(treeNode, chainId, meshId, subIds)

def inputFoliages(treeNode, foliageData, rootMesh, meshId, rootMeshId):
    vIdList=[]
    uvList = []
    qList = []
    print("foliage begin")
    for i in range(len(foliageData["pnt"])):
        print(i)
        rootP = foliageData["pnt"][i]
        rootP = dt.Point(rootP)
        vIds, u, v = als.closestPolygonAndWeights(rootP, rootMesh)

        vIdList.append(vIds[0])
        vIdList.append(vIds[1])
        vIdList.append(vIds[2])

        uvList.append(u)
        uvList.append(v)

        qList += foliageData["q"][i]

    attachedPointAttr = treeNode.foliages
    attachedPointAttr.foliageMeshId.set(meshId)
    attachedPointAttr.foliageAttachedMeshId.set(rootMeshId)
    attachedPointAttr.foliageAttachedPointId.set(vIdList)
    attachedPointAttr.foliageAttachedWeight.set(uvList)
    attachedPointAttr.foliageQuat.set(qList)
    print("foliage end")

def skelTreeCreator(meshDataDict):
    if not cmds.pluginInfo("skelTree",q=True,l=True):
        cmds.loadPlugin("skelTree")

    treeNode = pm.PyNode(cmds.createNode('skelTreeCreator'))
    visNode = pm.PyNode(cmds.createNode('skelTreeVisualization'))

    visNode.windDirection.set([1,0,0])
    visNode.dispChainScale.set(0.1)

    visNode.dispEnableDeformedPoints.set(False)
    visNode.dispEnableDeformedMeshes.set(False)
    visNode.dispEnableChainBoxes.set(False)
    visNode.dispEnableChainLine.set(True)
    visNode.dispEnableAttachedPoint.set(False)
    visNode.dispEnableFoliages.set(False)

    cmds.expression(s="{}.time=frame*0.01;".format(visNode.name()),o=visNode.name(), ae=1, uc="all")

    # input meshes
    meshList = meshDataDict["mesh"]
    idx = 0
    for mesh in meshList:
        meshNode = pm.PyNode(mesh)
        connectMeshToTree(meshNode, treeNode, idx)
        idx += 1

    deformDataList = meshDataDict["deform"]

    # input deformed meshes and chains
    for meshData in deformDataList:
        if meshData[3]:
            setTreeInputForDeformedMesh(treeNode, meshList.index(meshData[0]), meshData[1], meshData[2], meshDataDict["chain"].index(meshData[2]), 
                meshData[3], meshList.index(meshData[3]))
        else:
            setTreeInputForDeformedMesh(treeNode, meshList.index(meshData[0]), meshData[1], meshData[2], meshDataDict["chain"].index(meshData[2]))

    treeNode.outSkelTreeData.connect(visNode.inSkelTreeData)

    # input foliages
    rootMesh = meshList[0]
    print("rootMesh {}".format(rootMesh))
    inputFoliages(treeNode, meshDataDict["foliage"], rootMesh, 1, 0)

# skelTreeCreator({
#     "mesh":["pPlaneShape1", "pCylinderShape1", "pCylinderShape2","pCylinderShape3"],
#     "chain":["joint1","joint10","joint16"],
#     "deform":[
#         ["pCylinderShape1", "joint1", "pPlaneShape1"], 
#         ["pCylinderShape2", "joint10", "pCylinderShape1"],
#         ["pCylinderShape3", "joint16", "pCylinderShape1"]
#     ]
# })

# skelTreeCreator({
#     "mesh":["pCylinderShape1"],
#     "chain":["joint1"],
#     "deform":[["pCylinderShape1","joint1",None]]
# })