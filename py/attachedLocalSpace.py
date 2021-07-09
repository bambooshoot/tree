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
import maya.cmds as cmds

def pointInTriangle(a,b,c,p):
    v0 = b - a
    v1 = c - a
    v2 = p - a

    dot00 = v0.dot(v0)
    dot01 = v0.dot(v1)
    dot02 = v0.dot(v2)
    dot11 = v1.dot(v1)
    dot12 = v1.dot(v2)

    inverDeno = 1 / (dot00 * dot11 - dot01 * dot01)

    u = (dot11 * dot02 - dot01 * dot12) * inverDeno
    v = (dot00 * dot12 - dot01 * dot02) * inverDeno

    if v < -1e-3 or v > 1 or u < -1e-3 or u > 1:
        return False,u,v

    return u + v <= 1 + 1e-3,u,v

def meshNodeAndMatrix(mesh):
    meshNode = pm.PyNode(mesh)
    parentNode = meshNode.parent(0)
    invWorldMat = parentNode.worldInverseMatrix.get()
    worldMat = parentNode.worldMatrix.get()
    return meshNode, worldMat, invWorldMat

def closestPolygonAndWeights(p, mesh):
    meshNode, worldMat, invWorldMat = meshNodeAndMatrix(mesh)

    wp = p * invWorldMat
    cp, polyId = meshNode.getClosestPoint(wp)

    vIds = meshNode.getPolygonVertices(polyId)
    cp = dt.Point(cp)

    triNum = len(vIds) - 2
    inTri = None
    u = 0
    v = 0
    triPList = None
    for triId in range(triNum):
        triVids = meshNode.getPolygonTriangleVertices(polyId, triId)
        triPList = [dt.Point(meshNode.getPoint(i)) for i in triVids]
        inTri,u,v = pointInTriangle(triPList[0],triPList[1],triPList[2],cp)
        if inTri:
            return triVids,u,v

    # raise NameError("{} {} {}".format(triVids,u,v))
    return triVids,u,v

def closestMatrix(u, v, mesh, vids):
    meshNode, worldMat, invWorldMat = meshNodeAndMatrix(mesh)

    p3 = [ dt.Point(meshNode.getPoint(i)) * worldMat for i in vids ]
    xAxis = p3[1] - p3[0];
    zAxis = p3[2] - p3[0];

    orgP = p3[0] + xAxis * u + zAxis * v;

    yAxis = xAxis.cross(zAxis);
    yAxis.normalize();
    xAxis.normalize();
    zAxis = xAxis.cross(yAxis);
    zAxis.normalize();

    return dt.Matrix(
        xAxis.x, xAxis.y, xAxis.z, 0.0,
        yAxis.x, yAxis.y, yAxis.z, 0.0,
        zAxis.x, zAxis.y, zAxis.z, 0.0,
        orgP.x, orgP.y, orgP.z, 1.0 );

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