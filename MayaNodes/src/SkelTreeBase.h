#pragma once

#include <vector>
#include <array>
#include <map>

#include <OpenEXR/ImathVec.h>
#include <OpenEXR/ImathQuat.h>
#include <OpenEXR/ImathMatrix.h>

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

#define DECL_TYPE(T) \
DECL_VAR_TYPE(T) \
using T##List = std::vector<T>; \
using T##PList = std::vector<T##P>; \
DECL_VAR_TYPE(T##List)

#define DECL_ALIAS(D, T) \
using T = D; \
DECL_TYPE(T)

#define CLASSDEF(T, N, C, P) \
T N P { \
C \
};\
DECL_TYPE(N)

#define STRUCT(N, C) CLASSDEF(struct, N, C, )
#define CLASS(N, C) CLASSDEF(class, N, C, )
#define CLASSDERIVED(N, P, C) CLASSDEF(class, N, C, :public P)

DECL_ALIAS(int, Int)
DECL_ALIAS(unsigned int, Uint)
DECL_ALIAS(unsigned short, Ushort)
DECL_ALIAS(float, Float)

//using FLOAT2  = std::array<float, 2>;
//using FLOAT3  = std::array<float, 3>;
//using FLOAT4  = std::array<float, 4>;
//using FLOAT44 = std::array<float, 16>;

DECL_ALIAS(Imath::Vec2<Float>, Vec2)
DECL_ALIAS(Imath::Vec3<Float>, Vec)
DECL_ALIAS(Imath::Quat<Float>, Quat)
DECL_ALIAS(Imath::Matrix44<Float>, Matrix44)

using SpaceTypeId = Ushort;
using CSpaceTypeId = CUshort;

using Void = void;
using VoidP = void*;
using CVoidP = const void*;
using VecListList = std::vector<VecList>;

NS_END