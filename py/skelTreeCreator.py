import maya.cmds as cmds
import pymel.core as pm

def connectMeshToTree(meshNode,treeNode,index):
    meshNode.outMesh.connect(treeNode.inMeshes[index].inMesh)
    meshNode.parent(0).worldMatrix.connect(treeNode.inMeshes[index].inWorldMatrix)

def connectRootFrame(rootJoint, treeNode, idx):
    rootFrameAttr = treeNode.chains[idx].rootFrame
    rootJoint.t.connect(rootFrameAttr.offset)
    q = rootJoint.getOrientation()
    rootFrameAttr.rootQuat.set([q.x, q.y, q.z, q.w])

def connectFrame(joint, treeNode, idx, idx2):
    frame = treeNode.chains[idx].frames[idx2]
    frame.zOffset.set(joint.tx.get())
    q = joint.getOrientation()
    frame.frameQuat.set([q.x, q.y, q.z, q.w])

def connectJointHierarchyToTree(joint, treeNode, idx, idx2):
    if idx2 == -1:
        connectRootFrame(joint, treeNode, idx)
    else:
        connectFrame(joint, treeNode, idx, idx2)

    for subJoint in joint.getChildren():
        connectJointHierarchyToTree(subJoint, treeNode, idx, idx2+1)

def skelTreeCreator(mesh, rootJoint):
    treeNode = pm.PyNode(cmds.createNode('skelTreeCreator'))
    visNode = pm.PyNode(cmds.createNode('skelTreeVisualization'))
    meshNode = pm.PyNode(mesh)

    #input meshes
    connectMeshToTree(meshNode, treeNode, 0)

    # #input rootMeshes
    # connectMeshToTree(pm.PyNode(rootMesh), treeNode, 0)

    #input joints
    connectJointHierarchyToTree(pm.PyNode(rootJoint), treeNode, 0, -1)

    treeNode.outSkelTreeData.connect(visNode.inSkelTreeData)

def skelTreeCreatorSel():
    meshList = cmds.ls(sl=True,dag=True,type="mesh")
    jointList = cmds.ls(sl=True,dag=True,type="joint")

    mesh = meshList[0]
    rootJoint = jointList[0]
    #rootMesh = meshList[1]

    skelTreeCreator(mesh,rootJoint)

cmds.select(['joint1','pCylinder1'])
skelTreeCreatorSel()