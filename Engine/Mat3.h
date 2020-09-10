#pragma once
#include "Vec3.h"

template <typename T>
struct _Mat3
{
	_Mat3() = default;
	_Mat3 operator*(const _Mat3& rhs)
	{
		_Mat3 out;
		for (int rowL = 0; rowL < 3; rowL++)
		{
			for (int colR = 0; colR < 3; colR++)
			{
				out.cells[colR][rowL] = (T)0;
				for (int i = 0; i < 3; i++)
				{
					out.cells[colR][rowL] += cells[i][rowL] * rhs.cells[colR][i];
				}
			}
		}
		return out;
	}
	_Vec3<T> operator*(const _Vec3<T>& rhs)
	{
		_Vec3<T> out;
		out.x = 0;
		for (int i = 0; i < 3; i++)
		{
			out.x += rhs.x * cells[i][0];
		}
		out.y = 0;
		for (int i = 0; i < 3; i++)
		{
			out.y += rhs.y * cells[i][1];
		}
		out.z = 0;
		for (int i = 0; i < 3; i++)
		{
			out.z += rhs.z * cells[i][2];
		}
		return out;
	}
	static _Mat3 Scale(T scalar)
	{
		return { scalar, T(0),	
			T(0), scalar };
	}
	static _Mat3 Identity()
	{
		return { T(1), T(0), T(0),		
				T(0), T(1), T(0),		
				T(0), T(0), T(1) };
	}
	static _Mat3 FlipY()
	{
		return { T(1), T(0), T(0),
				T(0), T(-1), T(0),
				T(0), T(0), T(1) };
	}
	static _Mat3 Rotate(T theta)
	{
		const auto cosT = cos(theta);
		const auto sinT = sin(theta);
		return { cosT, -sinT, T(0),
				sinT, cosT,T(0),
				T(0), T(0), T(1) };
	}
	T cells[3][3];
};

using Mat3 = _Mat3<float>;