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
    mp.Add(Plank({ 200.0f,300.0f }, { 600.0f,300.0f }, 5.0f, Colors::Yellow));
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
    clock += deltaT;
    const auto newEnd = std::remove_if(mp.AcessMembers().begin() + 1, mp.AcessMembers().end(),
        [](std::shared_ptr<Drawer> d)
        {
            return !d->InView();
        });
    mp.AcessMembers().erase(newEnd, mp.AcessMembers().end());
    if (clock > 0.5f)
    {
        float xv = vx(rng);
        float yv = -sqrt(100 * 100 - xv * xv);
        
        if (other)
        {
            mp.Add(Ball(36.0f, Vec2(400.0f, 200.0f), Vec2(-xv, -yv)));
        }
        else
        {
            mp.Add(Ball(36.0f, Vec2(400.0f, 400.0f), Vec2(xv, yv)));
        }
        other = !other;
        clock -= 0.5f;
    }
    if (wnd.kbd.KeyIsPressed(VK_DOWN))
    {
        std::static_pointer_cast<Plank>(mp.AcessMembers()[0])->MoveMoveable({ 0.0f,2.0f });
    }
    if (wnd.kbd.KeyIsPressed(VK_UP))
    {
        std::static_pointer_cast<Plank>(mp.AcessMembers()[0])->MoveMoveable({ 0.0f, -2.0f });
    }
    if (wnd.kbd.KeyIsPressed(VK_LEFT))
    {
        mp.RotateCenter(deltaT, { (float)gfx.ScreenWidth / 2.0f, (float)gfx.ScreenHeight / 2.0f });
    }
    if (wnd.kbd.KeyIsPressed(VK_RIGHT))
    {
        mp.RotateCenter(-deltaT, { (float)gfx.ScreenWidth / 2.0f, (float)gfx.ScreenHeight / 2.0f });
    }

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
    
    //Scrolling, doesn't scale the balls yet
    /*while (!wnd.mouse.IsEmpty())
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
    }*/

    std::pair<Vec2,Vec2> plank = 
        std::static_pointer_cast<Plank>(*mp.AcessMembers().begin())->GetCorners();
    Vec2 plankDir = (plank.first - plank.second).GetNormalized();

    for (auto d = mp.AcessMembers().begin() + 1; d != mp.AcessMembers().end(); d++)
    {
        std::shared_ptr<Ball> b = std::static_pointer_cast<Ball>(*d);
        Vec2 pos = b->GetPos();
        float radius = b->GetRadius();

        //Commemt if not wanted
        Vec2 test = GetDistVec(plank.first, plank.second, pos);
        Vec2 t2 = pos - test;
        gfx.DrawLine(pos, t2, Colors::Green);
        gfx.DrawLine(pos, pos + b->GetVel().SetLen(radius), Colors::Yellow);
        ///////////////////////

        float distFromPlankSqr = GetDistSq(plank.first, plank.second, pos);
        if (square(radius) > distFromPlankSqr)
        {
            float depthIntoPlank = std::sqrt(square(radius) - distFromPlankSqr);
            Vec2 dir = b->GetVel().GetNormalized();
            Vec2 pToBall = GetDistVec(plank.first, plank.second, pos);
            Vec2 oldPToBall = pToBall;
            pToBall.SetLen(radius);
            b->Move(pToBall - oldPToBall);
            //
            float towardFirst = dir.GetSharedLen(plank.first - pos);
            float towardSecond = dir.GetSharedLen(plank.second - pos);
            float towardPlank = dir.GetSharedLen(-pToBall);
            //

            bool ballMovingToPlank = (dir.GetSharedLen(plank.first - pos) > 0.0f ||
                dir.GetSharedLen(plank.second - pos) > 0.0f) && dir.GetSharedLen(-pToBall) > 0.0f;
            if (ballMovingToPlank)
            {
                Vec2 newDirNormalised = GetReboundDir(dir, plankDir);
                b->SetVel(newDirNormalised * (b->GetVel().Len()));
                b->ChangeColor(Colors::Blue);
            }
        }
    }

    mp.Update(deltaT);
}

void Game::ComposeFrame()
{
    mp.Draw(gfx);
}
