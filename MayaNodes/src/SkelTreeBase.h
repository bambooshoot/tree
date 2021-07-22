#pragma once

#include <vector>
#include <array>
#include <map>
#include <limits>
#include <algorithm>
#include <assert.h>
#include <memory.h>
#include <functional>

#include <OpenEXR/ImathVec.h>
#include <OpenEXR/ImathQuat.h>
#include <OpenEXR/ImathMatrix.h>
#include <OpenEXR/ImathMatrixAlgo.h>
#include <OpenEXR/ImathBox.h>
#include <OpenEXR/ImathRandom.h>

#define NS_BEGIN namespace skelTree {
#define NS_END }

NS_BEGIN

// micro defination
#define DECL_VAR_TYPE(T) \
using C##T = const T; \
using R##T = T&; \
using CR##T = const T&; \
using T##P = T*; \
using C##T##P = const T*; \
using U##T##Ptr = std::unique_ptr<T>;

#define MAKE_UNIQUE std::make_unique

#define DECL_TYPE(T) \
DECL_VAR_TYPE(T) \
using T##List = std::vector<T>; \
using T##PList = std::vector<T##P>; \
DECL_VAR_TYPE(T##List)

#define DECL_ARRAY(M,T,N) \
using M = std::array<T,N>; \
DECL_VAR_TYPE(M) 

#define DECL_ALIAS(D, T) \
using T = D; \
DECL_TYPE(T)

#define CLASSDEF(T, N, C, ...) \
T N __VA_ARGS__ { \
C \
};\
DECL_TYPE(N)

#define STRUCT(N, C) CLASSDEF(struct, N, C )
#define CLASS(N, C) CLASSDEF(class, N, C )
#define CLASSDERIVED(N, P, C) CLASSDEF(class, N, C, :public P)

DECL_ALIAS(int, Int)
DECL_ALIAS(unsigned int, Uint)
DECL_ALIAS(unsigned short, Ushort)
DECL_ALIAS(float, Float)
DECL_ALIAS(char, Char)
DECL_ALIAS(unsigned char, Uchar)

//using FLOAT2  = std::array<float, 2>;
//using FLOAT3  = std::array<float, 3>;
//using FLOAT4  = std::array<float, 4>;
//using FLOAT44 = std::array<float, 16>;

DECL_ALIAS(Imath::Vec2<Float>, Vec2)
DECL_ALIAS(Imath::Vec3<Float>, Vec)
DECL_ALIAS(Imath::Quat<Float>, Quat)
DECL_ALIAS(Imath::Matrix44<Float>, Matrix44)
DECL_ALIAS(Imath::Matrix33<Float>, Matrix33)
DECL_ALIAS(Imath::Box<Vec>, Box)

using Random = Imath::Rand32;

using SpaceTypeId = Ushort;
using CSpaceTypeId = CUshort;

using Void = void;
using VoidP = void*;
using CVoidP = const void*;
using VecListList = std::vector<VecList>;

//constant value
#define USHORT_MAX Ushort(-1)
#define FLOATMAX std::numeric_limits<Float>::max()

#define DELETE_POINTER(P) { \
if (P) { \
	delete P; \
	P = nullptr; \
} \
}

#define DELETE_ARRAY_POINTER(P) { \
if (P) { \
	delete [] P; \
	P = nullptr; \
} \
}

inline Matrix33 Matrix4To3(CRMatrix44 mat44)
{
	return Matrix33(
		mat44[0][0], mat44[0][1], mat44[0][2],
		mat44[1][0], mat44[1][1], mat44[1][2],
		mat44[2][0], mat44[2][1], mat44[2][2] );
}

template<typename T,typename D, Int DIM>
void MatrixTranslate(T & srcMat, D & tarMat)
{
	for (Uint i = 0; i < DIM; ++i) {
		for (Uint j = 0; j < DIM; ++j) {
			tarMat[i][j] = srcMat[i][j];
		}
	}
}

inline Quat Matrix44ToQuat(CRMatrix44 mat)
{
	return extractQuat(mat);
}

NS_END