import maya.cmds as cmds
import pymel.core as pm
import maya.api.OpenMaya as om

import skelTreeCreator as stc

import importlib
importlib.reload(stc)

def testDeformWeights():
    dataDict = {
        "mesh":objList,
        "chain":rootJointList,
        "deform":meshDataList,
        "foliage":foliageList
        }

    pointNum = pm.PyNode("pCylinderShape1").numVertices()

    skelTreeCreator({
        "mesh":["pCylinderShape1"],
        "chain":["joint1"],
        "deform":[
            [["pCylinderShape1", i, "joint1", "pPlaneShape1"] for i in range(pointNum)]
        ]
        "foliage":[]
    })

    stc.skelTreeCreator(dataDict)