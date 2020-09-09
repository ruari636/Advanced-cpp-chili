/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include "Mat2.h"

Game::Game(MainWindow& wnd)
    :
    wnd(wnd),
    gfx(wnd),
    rng(std::random_device()())
{
    float FieldWidth = sqrt(widthSq) * starRad;
    float FieldHeight = sqrt(heightSq) * starRad;
    std::uniform_real_distribution<float>locX(-0.5f * FieldWidth, 0.5f * FieldWidth);
    std::uniform_real_distribution<float>locY(-0.5f * FieldHeight, 0.5f * FieldHeight);
    std::uniform_real_distribution<float>outerRad(120.0f, starRad);
    std::uniform_real_distribution<float>innerRad(15.0f, 120.0f);
    std::uniform_int_distribution<int>prongs(3, 12);
    std::uniform_real_distribution<float>timeToChange(0.1f, 2.0f);
    std::uniform_real_distribution<float>timeToRotate(0.0f, 20.0f);
    std::uniform_real_distribution<float>zeroToOne(0.1f, 0.9f);
    float curStarRad = 0.0f;

    while (mp.AcessMembers().size() < nStars)
    {
        float maxRad = outerRad(rng);
        Vec2 pos(locX(rng), locY(rng));
        if (std::any_of(mp.AcessMembers().begin(), mp.AcessMembers().end(),
            [&](const std::shared_ptr<Drawer>& d)
            {
                return (d->GetPos() - pos).LenSq() < sq(maxRad + d->GetShape()->GetRad());
            }))
        {
            continue;
        }

            std::uniform_int_distribution<int>hue(0, 255);

            Color c1 = Color(hue(rng), hue(rng), hue(rng));
            Color c2 = Color(hue(rng), hue(rng), hue(rng));
            if (c1.GetR() > c2.GetR())
            {
                std::swap(c1, c2);
            }
            star s(maxRad, innerRad(rng), prongs(rng));
            auto add = Effect(std::make_shared<star>(s), c1, c2, zeroToOne(rng), timeToChange(rng) * 5.0f,
                timeToChange(rng), timeToRotate(rng) - 10.0f);
            add.MoveTo(pos);
            mp.Add(add);
    }
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
    float deltaT = ft.Mark();
    if (wnd.mouse.LeftIsPressed())
    {
        if (mouseIsPressed)
        {
            Vec2 curMousePos = { float(wnd.mouse.GetPosX()), float(wnd.mouse.GetPosY()) };
            Vec2 deltaMove = curMousePos - prevMousePos;
            prevMousePos = curMousePos;
            mp.Move(deltaMove);
        }
        if (!mouseIsPressed)
        {
            mouseIsPressed = true;
            prevMousePos = { float(wnd.mouse.GetPosX()), float(wnd.mouse.GetPosY()) };
        }
    }
    else
    {
        mouseIsPressed = false;
    }
    
    while (!wnd.mouse.IsEmpty())
    {
        const auto e = wnd.mouse.Read();
        if (e.GetType() == Mouse::Event::Type::WheelDown)
        {
            mp.ScaleFrom(0.95f, Vec2(gfx.ScreenWidth / 2, gfx.ScreenHeight / 2));
        }
        else if (e.GetType() == Mouse::Event::Type::WheelUp)
        {
            mp.ScaleFrom(1.05f, Vec2(gfx.ScreenWidth / 2, gfx.ScreenHeight / 2));
        }
    }
    if (wnd.kbd.KeyIsPressed(VK_RIGHT))
    {
        mp.RotateCenter(PI * 2.0f * deltaT, //{ 0.0f,0.0f });
            { (float)gfx.ScreenWidth / 2.0f, (float)gfx.ScreenHeight / 2.0f });
        //test.RotateCenter(PI * 2.0f * deltaT, { (float)gfx.ScreenWidth / 2.0f, (float)gfx.ScreenHeight / 2.0f });
    }
    if (wnd.kbd.KeyIsPressed(VK_LEFT))
    {
        mp.RotateCenter(-PI * 2.0f * deltaT,
            { (float)gfx.ScreenWidth / 2.0f, (float)gfx.ScreenHeight / 2.0f });
    }
    mp.Update(deltaT);
}

void Game::ComposeFrame()
{
    Vec2 pos((float)wnd.mouse.GetPosX(),(float)wnd.mouse.GetPosY());
    mp.Draw(gfx);
    //test.Draw(gfx);
    /*for (auto& s : mp.AcessMembers())
    {
        gfx.DrawLine({ 400.0,300.0 }, s->GetPos(), Colors::Red);
    }*/
}
