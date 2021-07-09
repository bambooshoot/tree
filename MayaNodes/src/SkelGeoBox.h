#pragma once
#include <SkelTreeBase.h>

NS_BEGIN

#define BOX_POINT_NUM				8
#define BOX_POINT_6SIDE_NUM			4*6
#define BOX_TRIANGLE_IDX_NUM		36
#define BOX_TRIANGLE_WIREFRAME_NUM	24
#define BOX_POINT_DATA_SIZE         BOX_POINT_6SIDE_NUM * 3
#define BOX_COLOR_DATA_SIZE         BOX_POINT_6SIDE_NUM * 4

DECL_ARRAY(BoxPoints8, Float, BOX_POINT_NUM * 3)
DECL_ARRAY(BoxPoints6Sides, Float, BOX_POINT_6SIDE_NUM * 3)
DECL_ARRAY(BoxColors, Float, BOX_POINT_6SIDE_NUM * 4)
DECL_ARRAY(BoxTriIndices, Uint, BOX_TRIANGLE_IDX_NUM)
DECL_ARRAY(BoxWireframeIndices, Uint, BOX_TRIANGLE_WIREFRAME_NUM)

struct GeoBox
{
	static CBoxPoints6Sides			points;
	static CBoxColors				colors;
	static CBoxTriIndices			triIndices;
	static CBoxWireframeIndices		wireframeIndices;
};

NS_END