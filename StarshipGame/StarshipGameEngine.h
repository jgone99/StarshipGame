#pragma once
#define OLC_PGE_APPLICATION
#define PI 3.141592653f
#include "olcPixelGameEngine.h"
#include <vector>
class StarshipGameEngine : public olc::PixelGameEngine
{
private:
	struct Missle
	{
		float x, y;
		float idx, idy;
		float dx, dy;
	};

	struct Starship
	{
		float x, y;
		float dx, dy;
		float angle = 0;

		float sx[3] = { 0.0f, 4.0f, -4.0f };
		float sy[3] = { -7.0f, 7.0f, 7.0f };

		float tx[3];
		float ty[3];

		bool dampener = true;
		

		olc::vf2d velocity()
		{
			return { dx, dy };
		}

		void rotate(float rad)
		{
			for (int i = 0; i < 3; i++)
			{
				angle += rad;
				tx[i] = sx[i] * cosf(angle) - sy[i] * sinf(angle);
				ty[i] = sx[i] * sinf(angle) + sy[i] * cosf(angle);
			}
		}

	};

	Starship ship;
	int32_t sWidth;
	int32_t sHeight;
	float halfScreenWidth;
	float halfScreenHeight;
	std::vector<Missle> missles;
	bool upHeld, leftHeld, rightHeld, spacePressed;

public:

	StarshipGameEngine()
	{
		sAppName = "StarShip";
	}

	bool OnUserCreate() override
	{
		sWidth = ScreenWidth();
		sHeight = ScreenHeight();
		halfScreenWidth = sWidth * 0.5f;
		halfScreenHeight = sHeight * 0.5f;
		ship = {halfScreenWidth, halfScreenHeight, 0.0f, 0.0f, 0.0f};
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		//clear screen
		Clear(olc::BLACK);

		//get inputs
		upHeld = GetKey(olc::UP).bHeld;
		leftHeld = GetKey(olc::LEFT).bHeld;
		rightHeld = GetKey(olc::RIGHT).bHeld;
		spacePressed = GetKey(olc::SPACE).bPressed;

		//set dampener
		ship.dampener = GetKey(olc::D).bPressed * !ship.dampener + !GetKey(olc::D).bPressed * ship.dampener;
		
		//wrap ship coordinates
		wrapCoordinates(ship.x, ship.y, ship.x, ship.y);

		//Update missle position
		for (Missle& missle : missles)
		{
			missle.x += (missle.idx + missle.dx) * fElapsedTime;
			missle.y += (missle.idy + missle.dy) * fElapsedTime;
			DrawLine({ (int32_t)missle.x, (int32_t)missle.y }, 
					{ (int32_t)missle.x, (int32_t)missle.y });
		}

		//remove off-screen missles
		if (missles.size() > 0)
		{
			auto it = std::remove_if(missles.begin(), missles.end(), [&](Missle m) { return m.x <= 0 || m.y <= 0 || m.x >= sWidth || m.y >= sHeight; });
			if (it != missles.end())
			{
				missles.erase(it, missles.end());
			}
		}
		
		//Update ship thrust
		ship.dx += upHeld * 40.0f * sinf(ship.angle) * fElapsedTime + ship.dampener * !upHeld * (ship.dx != 0.0f) * -ship.dx * fElapsedTime;
		ship.dy += upHeld * 40.0f * -cosf(ship.angle) * fElapsedTime + ship.dampener * !upHeld * (ship.dy != 0.0f) * -ship.dy * fElapsedTime;
		ship.dx = fabsf(ship.dx) < 0.5f && !upHeld && ship.dampener ? 0.0f : ship.dx;
		ship.dy = fabsf(ship.dy) < 0.5f && !upHeld && ship.dampener ? 0.0f : ship.dy;

		//Update ship position
		ship.x += ship.dx * fElapsedTime;
		ship.y += ship.dy * fElapsedTime;

		//update ship rotation
		ship.rotate(leftHeld * PI * -0.25f * fElapsedTime);
		ship.rotate(rightHeld * PI * 0.25f * fElapsedTime);

		//create missle
		if (spacePressed)
		{
			missles.push_back({ ship.tx[0] + ship.x, ship.ty[0] + ship.y, ship.dx, ship.dy, 80.0f * sinf(ship.angle), 80.0f * -cosf(ship.angle) });
		}
		
		//draw ship
		DrawTriangle({ (int32_t)(ship.tx[0] + ship.x), (int32_t)(ship.ty[0] + ship.y) },
					{ (int32_t)(ship.tx[1] + ship.x), (int32_t)(ship.ty[1] + ship.y) },
					{ (int32_t)(ship.tx[2] + ship.x), (int32_t)(ship.ty[2] + ship.y) });

		return true;
	}

	virtual bool Draw(int32_t x, int32_t y, olc::Pixel p = olc::WHITE) override
	{
		float ox, oy;
		wrapCoordinates((float)x, (float)y, ox, oy);
		return olc::PixelGameEngine::Draw((int32_t)ox, (int32_t)oy);
	}

	void wrapCoordinates(float ix, float iy, float &ox, float &oy)
	{
		ox = ix;
		oy = iy;
		if (ix < 0.0f) ox = ix + (float)ScreenWidth();
		if (ix > (float)ScreenWidth()) ox = ix - (float)ScreenWidth();
		if (iy < 0.0f) oy = iy + (float)ScreenHeight();
		if (iy > (float)ScreenHeight()) oy = iy - (float)ScreenHeight();
	}

};

int main()
{
	StarshipGameEngine game;
	if (game.Construct(516, 400, 2, 2))
		game.Start();
	return 0;
}

