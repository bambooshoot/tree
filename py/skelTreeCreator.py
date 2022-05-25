import maya.cmds as cmds
import pymel.core as pm
import pymel.core.datatypes as dt
from tqdm import tqdm, trange

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

def connectRootFrame(rootJoint, treeNode, idx, rootMatrix):
    rootFrameAttr = treeNode.chains[idx].rootFrame
    rootP = dt.Point(rootJoint.getTranslation("world"))
    locP = rootP * rootMatrix
    rootFrameAttr.offset.set([locP.x, locP.y, locP.z])
    q = rootJoint.getRotation(quaternion=True) * rootJoint.getOrientation()
    q *= dt.Quaternion(rootMatrix)
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

def inputChain(treeNode, rootJoint, chainId, rootMatrix):
    # input joints
    rootJointNode = pm.PyNode(rootJoint)
    rootMatrix = rootMatrix.inverse()
    connectRootFrame(rootJointNode, treeNode, chainId, rootMatrix)

    for subJoint in rootJointNode.getChildren():
        connectJointHierarchyToTree(subJoint, treeNode, chainId, 0)

def parentChainFrame(parentJoint, rootP):
    jointNodes = pm.ls(parentJoint,dag=True,type="joint")
    nearestDist = 1000000
    curDist = 0
    nearestId = 0
    idx = 0
    nearestJointNode = None
    for joint in jointNodes:
        p = pm.joint(joint,q=True,p=True)
        curDist = (p - rootP).length()
        if nearestDist > curDist:
            nearestDist = curDist
            nearestId = idx
            nearestJointNode = joint

        idx += 1

    return nearestJointNode, nearestId

def setTreeInputForDeformedMesh(treeNode, meshId, subPIds, rootJoint, chainId, parentJoint=None):
    rootMatrix = dt.Matrix()
    rootMatrix.setToIdentity()
    # input rootMeshes
    if parentJoint:
        rootP = cmds.xform(rootJoint, q=True, ws=True, t=True)
        rootP = dt.Point(rootP)
        parentFrameJoint, rootChainFrameId = parentChainFrame(parentJoint, rootP)
        treeNode.chains[chainId].trunkFrameId.set(rootChainFrameId)
        rootMatrix = pm.xform(parentFrameJoint, q=True, ws=True, m=True)
        rootMatrix = dt.Matrix(rootMatrix)

    # input chain
    inputChain(treeNode, rootJoint, chainId, rootMatrix)

    connectMeshToDeform(treeNode, chainId, meshId, subPIds)

def inputFoliages(treeNode, foliageData, rootMesh, meshId, rootMeshId):
    foliageNum = len(foliageData["pnt"])
    vIdList = [0] * foliageNum * 3
    uvList = [0] * foliageNum * 2
    axisList = [0] * foliageNum * 3
    radianList = [0] * foliageNum
    scaleList = [1] * foliageNum

    itersector = als.createMeshIntersector(rootMesh)

    print("Foliage process is beginning")

    # pbar = tqdm(range(foliageNum))
    for i in range(foliageNum):
        # pbar.set_description('Processing {}'.format(i))

        if i % 10000 == 1:
            print("Processing {:.2f}%".format(i/foliageNum*100))

        rootP = foliageData["pnt"][i]
        rootP = dt.Point(rootP)
        vIds, u, v = als.closestPolygonAndWeights(rootP, rootMesh, itersector)

        vId3 = i*3
        vIdList[vId3  ]=vIds[0]
        vIdList[vId3+1]=vIds[1]
        vIdList[vId3+2]=vIds[2]

        uvId2 = i*2
        uvList[uvId2  ]=u
        uvList[uvId2+1]=v

        qId3 = i*3
        axis = foliageData["rotAxis"][i]

        axisList[qId3  ] = axis[0]
        axisList[qId3+1] = axis[1]
        axisList[qId3+2] = axis[2]

        radianList[i] = foliageData["rotRadian"][i]

        scaleList[i] = foliageData["scale"][i]

    print("Foliage process is done")

    attachedPointAttr = treeNode.foliages
    attachedPointAttr.foliageMeshId.set(meshId)
    attachedPointAttr.foliageAttachedMeshId.set(rootMeshId)
    attachedPointAttr.foliageAttachedPointId.set(vIdList)
    attachedPointAttr.foliageAttachedWeight.set(uvList)
    attachedPointAttr.foliageScale.set(scaleList)
    attachedPointAttr.foliageRotAxis.set(axisList)
    attachedPointAttr.foliageRotRadian.set(radianList)

def skelTreeCreator(meshDataDict):
    if not cmds.pluginInfo("skelTree",q=True,l=True):
        cmds.loadPlugin("skelTree")

    treeNode = pm.PyNode(cmds.createNode('skelTreeCreator'))
    visNode = pm.PyNode(cmds.createNode('skelTreeVisualization'))

    visNode.noiseTrunkValueFreqSpeed.set([0.2,.1,0.05,0.1])
    visNode.noiseBranchValueFreqSpeed.set([0.2,.2,1])
    visNode.noiseLeafValueFreqSpeed.set([1,.2,1,2])
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
            rootMeshId = meshList.index(meshData[3])
            setTreeInputForDeformedMesh(treeNode, meshList.index(meshData[0]), meshData[1], meshData[2], meshDataDict["chain"].index(meshData[2]), meshDataDict["chain"][0])
        else:
            setTreeInputForDeformedMesh(treeNode, meshList.index(meshData[0]), meshData[1], meshData[2], meshDataDict["chain"].index(meshData[2]))

    treeNode.outSkelTreeData.connect(visNode.inSkelTreeData)

    if meshDataDict["foliage"]:
        # input foliages
        rootMeshId = 0
        foliageMeshId = 1
        rootMesh = meshList[rootMeshId]
        inputFoliages(treeNode, meshDataDict["foliage"], rootMesh, foliageMeshId, rootMeshId)

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