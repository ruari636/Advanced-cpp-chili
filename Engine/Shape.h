#pragma once
#include "Vec2.h"
#include <vector>
#include "Graphics.h"

class shape
{
	std::vector<Vec2> Vertices;

public:
	shape() = default;
	shape(std::vector<Vec2> in)
		:
		Vertices(in)
	{
	}
	std::vector<Vec2> GetShape() const
	{
		return Vertices;
	}
	virtual float GetRad() const { return 0.0f; }
	void overWrite(std::vector<Vec2> shape_in)
	{
		Vertices = shape_in;
	}
};

class star : public shape
{
	float radius = 0.0f;
public:
	star(float outerRad, float innerRad, int flares)
		:
		radius(outerRad)
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
	float GetRad() const override { return radius; }
};