#pragma once
#include "Vec2.h"
#include <vector>
#include "Graphics.h"

class shape
{
	std::vector<Vec2> Vertices;
	void saveTLBR()
	{
		Vec2 TL;
		Vec2 BR;
		TL = *Vertices.begin();
		BR = *Vertices.begin();
		for (auto i = Vertices.begin() + 1; i < Vertices.end(); i++)
		{
			if (i->x < TL.x)
			{
				TL.x = i->x;
			}
			if (i->y < TL.y)
			{
				TL.y = i->y;
			}
			if (i->x > BR.x)
			{
				BR.x = i->x;
			}
			if (i->y > BR.y)
			{
				BR.y = i->y;
			}
		}
		TLBR[0] = TL;
		TLBR[1] = BR;
	}
protected:
	void overWrite(std::vector<Vec2> shape_in)
	{
		Vertices = shape_in;
		saveTLBR();
	}
public:
	Vec2 TLBR[2];
	shape() = default;
	shape(std::vector<Vec2> in)
		:
		Vertices(in)
	{
		saveTLBR();
	}
	std::vector<Vec2> GetShape()
	{
		return Vertices;
	}
	Vec2 GetTL() const { return TLBR[0]; }
	Vec2 GetBR() const { return TLBR[1]; }
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