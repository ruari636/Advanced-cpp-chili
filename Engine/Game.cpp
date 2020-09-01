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

Game::Game(MainWindow& wnd)
    :
    wnd(wnd),
    gfx(wnd),
    rng(std::random_device()())
    //test(std::make_shared<star>(star(120.0f, 360.0f, 12)), 
        //Colors::Red, Colors::Cyan, 0.5f, 3.0f, 3.0f)
{
    std::uniform_real_distribution<float>innerRad(15.0f, minStarRad);
    std::uniform_real_distribution<float>outerRad(minStarRad, starRad);
    std::uniform_int_distribution<int>prongs(3, 12);
    std::uniform_int_distribution<int>hue(0, 255);
    Vec2 loc{ -(starRad) * width + gfx.ScreenWidth,
        -(starRad - minStarRad) * height + gfx.ScreenHeight};
    float curStarRad = 0.0f;

    //test.MoveTo({ 100.0f,100.0f });

    for (int x = 0; x < width; x++)
    {
        loc.y = -starRad * height + gfx.ScreenHeight;
        for (int y = 0; y < height; y++)
        {
            Effect Star(Effect::RandomStar(starRad));
            curStarRad = Star.GetShape()->GetRad();
            Star.MoveTo(loc + Vec2{ 0, curStarRad });
            mp.Add(Star);
            loc.y += curStarRad * 2;
        }
        loc.x += starRad * 2;
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
            //test.ScaleFrom(0.95f, Vec2(gfx.ScreenWidth / 2, gfx.ScreenHeight / 2));
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
