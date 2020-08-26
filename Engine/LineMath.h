#pragma once
#include "Vec2.h"

template <typename T>
T square(const T& root)
{
	return root * root;
}

template <typename T>
T GetDistSq(const _Vec2<T>& vert1, const _Vec2<T>& vert2, const _Vec2<T>& point)
{
	T a = (vert1.y - vert2.y) / (vert1.x - vert2.x);
	T c = vert1.y - (a * vert1.x);
	T x = point.x;
	T y = point.y;
	return (square(a * x - y + c)) / (square(a) + (T)1);
}

template <typename T>
_Vec2<T> GetReboundDir(const _Vec2<T>& objDir, const _Vec2<T>& edgeDir)
{
	T sharedLen = objDir.GetSharedLen(edgeDir);
	_Vec2<T> edgeParallelComp = edgeDir * sharedLen;
	_Vec2<T> edgePerpendicularComp = objDir - edgeParallelComp;
	_Vec2<T> newDir = objDir - edgePerpendicularComp * 2;
	return newDir;
}