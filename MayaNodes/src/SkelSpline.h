#pragma once

#include <SkelTreeBase.h>

NS_BEGIN

#define SPLINE_WEIGHT_NUM 3

template<Int WEIGHT_NUM>
struct SplineWeight
{
	Ushort id[WEIGHT_NUM];
	Float w[WEIGHT_NUM];
};

template<Int WEIGHT_NUM>
class UParamTrain
{
public:
	virtual ~UParamTrain() {}
	void reset(CFloatList uList)
	{
		_uList = uList;
	}

	Float u(CUint idx) const
	{
		return _uList[idx];
	}

	virtual void weights(SplineWeight<WEIGHT_NUM> & w, CFloat x, CUint n_idx) const = 0;

	template<typename T>
	T interpolate(CFloat u, const std::vector<T> & dataList) const
	{
		Uint n_idx = _segementId(u);
		SplineWeight<WEIGHT_NUM> w;
		weights(w, u, n_idx);

		T value = 0;
		for (Uint i = 0; i < WEIGHT_NUM; ++i)
			value += dataList[w.id[i]] * w.w[i];
		
		return value;
	}

	static UParamTrain* create(CFloatList uList);

protected:
	FloatList _uList;

	Uint _segementId(CFloat u) const
	{
		Uint idx = 0;
		for (; idx < _uList.size(); ++idx) {
			if (u >= _uList[idx])
				break;
		}
		return idx;
	}
};

template<Int WEIGHT_NUM>
class UParamTrain3 : public UParamTrain<WEIGHT_NUM>
{
public:
	~UParamTrain3() override {}
	UParamTrain3(CFloatList uList)
	{
		reset(uList);
	}
	void weights(SplineWeight<WEIGHT_NUM> & w, CFloat x, CUint n_idx) const override
	{
		Float xi[WEIGHT_NUM];
		if (n_idx == 0) {
			for (Uint k = 0; k < WEIGHT_NUM; ++k) {
				w.id[k] = n_idx + k;
				xi[k] = _uList[w.id[k]];
			}
		}
		else {
			for (Uint k = 0; k < WEIGHT_NUM; ++k) {
				w.id[k] = n_idx + k - 1;
				xi[k] = _uList[w.id[k]];
			}
		}

		for (Uint k = 0; k < WEIGHT_NUM; ++k) {
			w.w[k] = 1.0f;
			for (Uint i = 0; i < WEIGHT_NUM; ++i) {
				if (i == k)
					continue;

				w.w[k] *= (x - xi[i]) / (xi[k] - xi[i]);
			}
		}
	}
};

template<Int WEIGHT_NUM>
class UParamTrain2 : public UParamTrain<WEIGHT_NUM>
{
public:
	~UParamTrain2() override {}
	UParamTrain2(CFloatList uList)
	{
		reset(uList);
	}
	void weights(SplineWeight<WEIGHT_NUM> & w, CFloat x, CUint n_idx) const override
	{
		w.id[0] = 0;
		w.id[1] = 1;
		w.id[2] = 1;

		w.w[1] = (x - _uList[0]) / (_uList[1] - _uList[0]);
		w.w[0] = 1.0f - w.w[1];
		w.w[2] = 0;
	}
};

template<Int WEIGHT_NUM>
UParamTrain<WEIGHT_NUM>* UParamTrain<WEIGHT_NUM>::create(CFloatList uList)
{
	if (uList.size() > 2)
		return new UParamTrain3<WEIGHT_NUM>(uList);

	return new UParamTrain2<WEIGHT_NUM>(uList);
}

class Spline1D
{
public:
	Spline1D(CRFloatList valueList, CRFloatList uList) : _uTrain(uList)
	{
		_valueList = valueList;
	}

	Float value(CFloat u) const
	{
		return _uTrain.interpolate<Float>(u, _valueList);
	}

private:
	FloatList	_valueList;
	UParamTrain3<5> _uTrain;
};

class Spline3D
{
public:
	Spline3D(CRVecList pList)
	{
		_pList = pList;
		for (auto iter = _pList.begin(); iter != _pList.end() - 1; ++iter) {
			_locUList.push_back((*(iter+1) - *iter).length());
		}

		Float curU = 0.0f;
		FloatList uList;
		uList.push_back(0.0f);

		for (auto u : _locUList) {
			curU += u;
			uList.push_back(curU);
		}

		_pUTrain = UParamTrain<3>::create(uList);
	}
	virtual ~Spline3D()
	{
		DELETE_POINTER(_pUTrain);
	}
	
	void weights(SplineWeight<3> & w, CRVec p) const
	{
		Uint n_idx;
		Float x = _nearestU(n_idx, p);
		_pUTrain->weights(w, x, n_idx);
	}

	static Spline3D* create(CRVecList pList);

protected:
	VecList		_pList;
	FloatList   _locUList;
	UParamTrain<3> *_pUTrain;

	virtual Float _nearestU(RUint n_idx, CRVec sp) const = 0;
};

class Spline3 : public Spline3D
{
public:
	~Spline3() override {}
	Spline3(CRVecList pList) : Spline3D(pList) {}

protected:
	Vec _lineSegment(CUint idx) const
	{
		return (_pList[idx + 1u] - _pList[idx]).normalize();
	}
	Float _nearestU(RUint n_idx, CRVec sp) const override
	{
		Float nearestDist2 = FLOATMAX, dist2;
		Uint nearestIdx = 0, idx = 0;
		for (auto iter = _pList.begin(); iter != _pList.end() - 1; ++iter) {
			dist2 = (*iter - sp).length2();
			if (dist2 < nearestDist2) {
				nearestDist2 = dist2;
				nearestIdx = idx;
			}
			++idx;
		}

		Vec lineV = _lineSegment(nearestIdx);
		Float locU = lineV.dot(sp - _pList[nearestIdx]);
		if (locU < 0) {
			CInt lastIdx = nearestIdx - 1;
			if (lastIdx < 0) {
				n_idx = 0;
				return 0.0f;
			}

			nearestIdx = lastIdx;
			lineV = _lineSegment(nearestIdx);
			locU = lineV.dot(sp - _pList[nearestIdx]);
			assert(locU > 0.0f);
		}

		n_idx = nearestIdx;
		locU = std::min(_locUList[n_idx], locU);
		return _pUTrain->u(n_idx) + locU;
	}
};

class Line2 : public Spline3D
{
public:
	~Line2() override {}
	Line2(CRVecList pList) : Spline3D(pList) {}

protected:
	Float _nearestU(RUint n_idx, CRVec sp) const override
	{
		n_idx = 0;
		CVec lineV = _pList[1] - _pList[0];
		Vec pV = sp - _pList[0];
		Float pf = pV.dot(lineV);
		CFloat l=lineV.length();
		
		pf = std::max(0.0f, std::min(pf, l));
		return pf;
	};
};

inline Spline3D* Spline3D::create(CRVecList pList)
{
	if (pList.size() > 2)
		return new Spline3(pList);

	return new Line2(pList);
};

NS_END