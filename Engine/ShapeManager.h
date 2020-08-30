#pragma once
#include "Shape.h"
#include <memory>
#include <functional>
#include "ChiliMath.h"

class Drawer
{
	Vec2 TL;
	Vec2 BR;
	Color c;
	Vec2 pos = { 0.0f,0.0f };
	Vec2 deltaOrigin = { 0.0f,0.0f };
	Vec2 scaleCenter = { 1.0f,1.0f };
	Vec2 scaleOrigin = { 1.0f,1.0f };
	Vec2 rPos = pos;
	std::vector<Vec2> Vertices;
	std::vector<std::function<void(float deltaT, Drawer& d)>> effects;
	bool notInView;

protected:
	std::shared_ptr<shape> s;
	float rotation = 0.0f;
	float rotationOrigin = 0.0f;

public:
	Drawer(std::shared_ptr<shape> s, const Color c)
		:
		c(c),
		s(s)
	{
		if (s->GetShape().size() > 0)
		{
			Refresh();
		}
	}
	Vec2 GetPos() { return rPos; }
	Vec2 GetPosVert(int index) { return Vertices[index] + rPos; }
	Vec2 GetScale() { return scaleCenter; }
	virtual void Move(Vec2 deltaPos) 
	{ 
		pos += deltaPos;
		Refresh();
	}
	void MoveTo(Vec2 newPos) 
	{
		pos = newPos;
		Refresh();
	}
	void Scale(float deltaScale) {
		scaleOrigin *= deltaScale; 
		for (Vec2& v : Vertices)
		{
			v *= deltaScale;
		}
	}
	void Scale(Vec2 deltaScale) 
	{ 
		scaleOrigin *= deltaScale;
		for (Vec2& v : Vertices)
		{
			v *= deltaScale;
		}
	}
	void ScaleCenter(Vec2 deltaScale)
	{
		scaleCenter += deltaScale;
		Refresh();
	}
	void SetScaleCenter(Vec2 Scale)
	{
		scaleCenter = Scale;
		Refresh();
	}
	void SetScaleCenter(float Scale)
	{
		SetScaleCenter({ Scale, Scale });
	}
	void ChangeColor(Color c_in) { c = c_in; }
	virtual void Rotate(float theta)
	{
		rotation += theta;
		while (rotation > (2 * PI))
		{
			rotation -= (2 * PI);
		}
		Refresh();
	}
	virtual void RotateCenter(float theta)
	{
		rotationOrigin += theta;
		while (rotationOrigin > (2 * PI))
		{
			rotationOrigin -= (2 * PI);
		}
		Refresh();
	}
	void Refresh()
	{
		Vertices = s->GetShape();

		float sinTheta = sin(rotation);
		float cosTheta = cos(rotation);

		float sinO = sin(rotationOrigin);
		float cosO = cos(rotationOrigin);

		auto transform = [&](Vec2& cur)
		{
			cur.Rotate(sinTheta, cosTheta);
			cur *= scaleCenter;
			cur -= deltaOrigin;
			cur *= scaleOrigin;
			cur.Rotate(sinO, cosO);
		};
		rPos = pos.GetRotated(rotationOrigin);

		if (Vertices.size() > 0)
		{
			TL = Vertices[0];
			BR = Vertices[0];
			for (Vec2& v : Vertices)
			{
				transform(v);
				TL.x = std::min(TL.x, v.x);
				TL.y = std::min(TL.y, v.y);
				BR.x = std::max(BR.x, v.x);
				BR.y = std::max(BR.y, v.y);
			}
		}
	}
	void Draw(Graphics& gfx)
	{
		float sinO = sin(rotationOrigin);
		float cosO = cos(rotationOrigin);

		Vec2 curTL = TL + rPos;
		Vec2 curBR = BR + rPos;
		notInView = (curTL.x > gfx.ScreenWidth || curTL.y > gfx.ScreenHeight) || (curBR.x < 0 || curBR.y < 0);
		if (!notInView)
		{
			for (auto cur = Vertices.begin() + 1; cur != Vertices.end(); cur++)
				gfx.DrawLine(*(cur - 1) + rPos, *cur + rPos, c);
			gfx.DrawLine(*(Vertices.end() - 1) + rPos, *Vertices.begin() + rPos, c);
		}
	}
	void MoveOriginTo(Vec2 newPos) 
	{
		Vec2 deltaPos = newPos - pos;
		pos = newPos;
		deltaOrigin += (deltaPos / scaleOrigin);
		Refresh();
	}
	bool InView() { return !notInView; }
	Color& GetCol(){ return c; }
	void OverlayEffect(std::function<void(float deltaT, Drawer& d)> e)
	{
		effects.push_back(e);
	}
	virtual void Effect(float deltaT)
	{
		for (auto& e : effects)
		{
			e(deltaT, *this);
		}
	}
	virtual void Update(float deltaT, const std::vector<std::shared_ptr<Drawer>> shape)
	{
	}
	const shape* GetShape() const { return s.get(); }
};

class Effect : public Drawer
{
	Color c1;
	Color c2;
	bool reached = false;
	float timeToChange = 1.0f;
	float timeToShrink = 1.0f;
	float curR;
	float curG;
	float curB;

	bool max = true;
	float EffectScale = 1.0f;
	float minScale = 0.5f;
	float sinMultiplier = (EffectScale - minScale) / 2.0f;
	float startScale = (EffectScale + minScale) / 2.0f;
	float timePassed = 0.0f;

public:
	Effect(std::shared_ptr<shape> s, Color c, Color c2, float minScale, float timeToChange, float timeToShrink)
		:
		Drawer(s, c),
		c1(c),
		c2(c2),
		curR((float)c.GetR()),
		curG((float)c.GetG()),
		curB((float)c.GetB()),
		timeToChange(timeToChange),
		timeToShrink(timeToShrink),
		minScale(minScale)
	{
	}
	static Effect RandomStar(float starRad = 420.0f)
	{
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_real_distribution<float>innerRad(15.0f, 120.0f);
		std::uniform_real_distribution<float>outerRad(120.0f, starRad);
		std::uniform_real_distribution<float>timeToChange(0.1f, 2.0f);
		std::uniform_real_distribution<float>zeroToOne(0.1f, 0.9f);
		std::uniform_int_distribution<int>prongs(3, 12);
		std::uniform_int_distribution<int>hue(0, 255);

		Color c1 = Color(hue(rng), hue(rng), hue(rng));
		Color c2 = Color(hue(rng), hue(rng), hue(rng));
		if (c1.GetR() > c2.GetR())
		{
			std::swap(c1, c2);
		}

		star Star = star(outerRad(rng), innerRad(rng), prongs(rng));
		float min = zeroToOne(rng);
		return Effect(std::make_shared<star>(Star), c1, c2, min, timeToChange(rng) * 5.0f, timeToChange(rng));
	}
	void Update(float deltaT, const std::vector<std::shared_ptr<Drawer>> shape) override
	{
		Color& cur = Drawer::GetCol();
		float changeAmount = deltaT / timeToChange;
		if (!reached)
		{
			curR += changeAmount * (c2.GetR() - c1.GetR());
			curG += changeAmount * (c2.GetG() - c1.GetG());
			curB += changeAmount * (c2.GetB() - c1.GetB());
			reached = (int)curR >= c2.GetR() - 1;
		}
		else
		{
			curR -= changeAmount * (c2.GetR() - c1.GetR());
			curG -= changeAmount * (c2.GetG() - c1.GetG());
			curB -= changeAmount * (c2.GetB() - c1.GetB());
			reached = !((int)curR <= c1.GetR() + 1);
		}
		cur = Color((int)curR, (int)curG, (int)curB);

		timePassed += deltaT;
		SetScaleCenter((sin(timePassed / timeToShrink) * sinMultiplier) + startScale);
	}
};

class Plank : public Drawer
{
	std::vector<Vec2> plank;

public:
	Plank(Vec2 anchor, Vec2 moveable, float thickness, const Color c)
		:
		Drawer(std::make_shared<shape>(), c)
	{
		MoveTo(anchor);
		moveable -= anchor;
		plank.push_back({0.0f,0.0f});
		plank.push_back(moveable);
		plank.push_back(Vec2(moveable.x, moveable.y + thickness));
		plank.push_back(Vec2(0.0f, thickness));
		s->overWrite(plank);
		Refresh();
	}
	void MoveMoveable(Vec2 deltaPos)
	{
		plank[1] += deltaPos;
		plank[2] += deltaPos;
		s->overWrite(plank);
		Refresh();
	}
	void Rotate(float theta) override
	{
		Drawer::Rotate(theta);
		plank = s->GetShape();
	}
	std::pair<Vec2, Vec2> GetCorners()
	{
		return { GetPos(), GetPosVert(1)};
	}
};

class Ball : public Drawer
{
	Vec2 vel;
	float radius;

public:
	Ball(float radius, Vec2 loc, Vec2 vel, Color c = Colors::Red)
		:
		vel(vel),
		Drawer(std::make_shared<star>(star(radius, radius, (std::min)(int(radius), 36))), c),
		radius(radius)
	{
		MoveTo(loc);
	}
	void Update(float deltaT, const std::vector<std::shared_ptr<Drawer>> collisions) override
	{
		Move(vel * deltaT);
	}
	float GetRadius() const
	{
		return radius;
	}
	Vec2 GetVel() const
	{
		return vel;
	}
	void SetVel(Vec2 vel_in)
	{
		vel = vel_in;
	}
	void RotateCenter(float theta) override
	{
		Drawer::RotateCenter(theta);
		vel.Rotate(theta);
	}
};

struct MoveSpace
{
	std::vector<std::shared_ptr<Drawer>> shapes;
	std::vector<std::pair<std::shared_ptr<Drawer>, std::shared_ptr<Drawer>>> collisions;
	void Move(Vec2 deltaPos)
	{
		pos += deltaPos;
		for (auto& d : shapes)
			d->Move(deltaPos);
	}
	void MoveTo(Vec2 newPos)
	{
		for (auto& d : shapes)
		{
			Vec2 curLoc = d->GetPos();
			d->MoveTo(curLoc + newPos - pos);
		}
	}
	void Scale(float deltaScale)
	{
		for (auto& drawer : shapes)
		{
			drawer->Scale(deltaScale);
			drawer->Move((drawer->GetPos() - pos) * deltaScale);
		}
	}
	void Scale(Vec2 deltaScale)
	{
		for (auto& drawer : shapes)
		{
			drawer->Move((drawer->GetPos() - pos) * deltaScale);
			drawer->Scale(deltaScale);
		}
	}
	void Rotate(float theta)
	{
		for (auto& s : shapes)
		{
			s->RotateCenter(theta);
			s->Refresh();
		}
	}
	void Draw(Graphics& gfx)
	{
		for (auto& drawer : shapes)
		{
			drawer->Draw(gfx);
		}
	}
	void Add(std::shared_ptr<Drawer> d)
	{
		shapes.push_back(d);
	}
	template <typename T>
	void Add(T d)
	{
		shapes.push_back(std::make_shared<T>(d));
	}
	void SetOrigin(Vec2 newPos)
	{
		pos = newPos;
		for (auto& drawer : shapes)
		{
			drawer->MoveOriginTo(newPos);
		}
	}
	void Update(float deltaT)
	{
		for (auto& s : shapes)
		{
			s->Update(deltaT, shapes);
		}
	}
	std::vector<std::shared_ptr<Drawer>>& AcessMembers()
	{
		return shapes;
	}
	const std::vector<std::shared_ptr<Drawer>>& AcessMembers() const
	{
		return shapes;
	}

private:
	Vec2 pos = { 0.0f, 0.0f };
	float scale = 1.0f;
};