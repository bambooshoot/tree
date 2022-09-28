# Trunk is a mesh hierarchy having all branches exported from speed tree
# FoliageMeshList has Foliage meshes extracted from a merged Foliage mesh exported from speed tree
# FoliageElement  a Foliage mesh relocated at original point and z direction from which root is aimed to its tip
def build(Trunk, FoliageMeshList, FoliageElement):
    # Trunk and Branches
    # build skeleton with attachedPoint branches
    # convert skeletons to Frame Chain
    # get pair of Trunk or Bruanch and related Frame Chain preparing weight calculation.
    # weight calculation to get deformed point packs
    # 
    # Foliage
    # store Foliage element in a list for index below.
    # to every foliage mesh a searching process is started to create a attachedPoint and assign a FoliageElement on it.
    # Only one sort of foliage element is supported so far.