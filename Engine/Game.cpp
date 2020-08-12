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

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
    rng(std::random_device()())
{
    std::uniform_real_distribution<float>innerRad(15.0f, 120.0f);
    std::uniform_real_distribution<float>outerRad(120.0f, starRad);
    std::uniform_int_distribution<int>prongs(3, 12);
    std::uniform_int_distribution<int>hue(0, 255);
    Vec2 loc{ -starRad * width + gfx.ScreenWidth, -starRad * height + gfx.ScreenHeight};

    for (int x = 0; x < width; x++)
    {
        loc.y = -starRad * height + gfx.ScreenHeight;
        for (int y = 0; y < height; y++)
        {
            Effect star(Effect::RandomStar());
            star.MoveTo(loc);
            mp.Add(std::make_unique<Effect>(star));
            loc.y += starRad * 2;
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
    if (wnd.kbd.KeyIsPressed(VK_LEFT))
    {
        mp.Move({ deltaT * 50, 0.0f });
    }
    if (wnd.kbd.KeyIsPressed(VK_RIGHT))
    {
        mp.Move({ -deltaT * 50, 0.0f });
    }
    if (wnd.kbd.KeyIsPressed(VK_UP))
    {
        mp.Move({ 0.0f, deltaT * 50 });
    }
    if (wnd.kbd.KeyIsPressed(VK_DOWN))
    {
        mp.Move({ 0.0f, -deltaT * 50 });
    }
    
    while (!wnd.mouse.IsEmpty())
    {
        const auto e = wnd.mouse.Read();
        if (e.GetType() == Mouse::Event::Type::WheelDown)
        {
            mp.SetOrigin(Vec2(gfx.ScreenWidth / 2, gfx.ScreenHeight / 2));
            mp.Scale(0.95f);
        }
        else if (e.GetType() == Mouse::Event::Type::WheelUp)
        {
            mp.SetOrigin(Vec2(gfx.ScreenWidth / 2, gfx.ScreenHeight / 2));
            mp.Scale(1.05f);
        }
    }

    mp.Update(deltaT);
}

void Game::ComposeFrame()
{
    Vec2 pos((float)wnd.mouse.GetPosX(),(float)wnd.mouse.GetPosY());
    mp.Draw(gfx);
    gfx.DrawLine({ 400.0f,300.0f }, pos, Colors::Blue);
}
