#pragma once
#include "Vec2.h"
#include <vector>
#include "Graphics.h"

class shape
{
	std::vector<Vec2> Vertices;
protected:
	void overWrite(std::vector<Vec2> shape_in)
	{
		Vertices = shape_in;
	}
public:
	shape() = default;
	shape(std::vector<Vec2> in)
		:
		Vertices(in)
	{
	}
	std::vector<Vec2> GetShape()
	{
		return Vertices;
	}
};

class star : public shape
{
public:
	star(float outerRad, float innerRad, int flares)
	{
		std::vector<Vec2> star;
		star.reserve(flares * 2);
		const float dTheta = 2.0f * 3.14159f / float(flares * 2);
		for (int i = 0; i < flares * 2; i++)
		{
			const float rad = i % 2 == 0 ? outerRad : innerRad;
			star.emplace_back(rad * (float)cos(float(i) * dTheta), rad * (float)sin(float(i) * dTheta));
		}
		shape::overWrite(star);
	}
};