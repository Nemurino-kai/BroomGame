#include <Siv3d.hpp>
#include "gameManager.h"

gameManager::gameManager():world(Vec2(0, -2.0)),camera(Vec2(1, 2.5), 120)
{
	wall = world.createLine(Vec2(0, 0), Line({ -0.01, 0 }, { 0.01, 0 }), none, none, PhysicsBodyType::Kinematic);
	broom = world.createRect(Vec2(0, 0), RectF(Vec2(0, 0), Vec2(0.08, 4)));
	wheel = world.createWheelJoint(wall, broom, broom.getPos() + Vec2(0.04, 0), Vec2(0, 1), WheelJointState(false, 20.0, 60, 0.5));
	previousPos = Window::GetState().windowPos;
	previousDelta = Vec2(0, 0);
	
	timer.start();
}

bool gameManager::isOver()
{
	return endflag;
}

void gameManager::update()
{
	if (Input::MouseL.clicked)
	{
		balls.push_back(world.createCircle(Vec2(0, 4), 0.1));
	}

	Vec2 currentPos = Window::GetState().windowPos;

	const Vec2 currentDelta = (currentPos - previousPos);

	Vec2 impulse = currentDelta-previousDelta;
	//previousDelta = currentDelta;

	impulse = Vec2(impulse.x, 0);
	if (Input::KeyLeft.pressed) {
		impulse = Vec2(1, 0);
	}
	if (Input::KeyRight.pressed) {
		impulse = Vec2(-1, 0);
	}
	if (Input::KeyQ.clicked) {
		//balls.push_back(world.createCircle(Vec2(-4, 3), 0.1));
		balls.push_back(world.createCircle(Vec2(-4,3),0.24,PhysicsMaterial(0.3)));
		balls[int(balls.size()) - 1].applyLinearImpulse(Vec2(3.0, 0)*balls[int(balls.size()) - 1].getMass());
	}
	if (Input::KeyW.clicked) {
		//balls.push_back(world.createCircle(Vec2(4, 3), 0.1));
		balls.push_back(world.createCircle(Vec2(4, 3), 0.3, PhysicsMaterial(0.3)));
		balls[int(balls.size()) - 1].applyLinearImpulse(Vec2(-3.0, 0)*balls[int(balls.size()) - 1].getMass());
	}

	previousPos = currentPos;

	Println(broom.getAngle());
	if (broom.getAngle() > Pi / 2 || broom.getAngle() < -Pi / 2) { Println(L"GameOver!"); 
	endflag = true;
	}
	broom.applyForce(-impulse * broom.getMass() / 5.0);

	world.update();

}

void gameManager::draw()
{
	TextureAsset(L"classroom").draw();
	TextureAsset(L"arm").draw(0,500);
	double ydif = 280 + TextureAsset(L"broom").height *( 1 - cos(-broom.getAngle()) )/ 2.0;
	double xdif = Window::Width() / 2.0 + TextureAsset(L"broom").height *sin(-broom.getAngle()) / 2.0;

	Texture(TextureAsset(L"broom")).rotate(-broom.getAngle()).drawAt(xdif,ydif);

	//ŽžŠÔ•\Ž¦
	FontAsset(L"ScoreList")(getTime(),L"s").draw(10,0);
	auto t = camera.createTransformer();

	for (const auto& ball : balls)
	{
		ball.draw(HSV(0.5, 0.4, 1.0));
		Texture(TextureAsset(L"basket")).scale(1.0 / camera.getScale()).drawAt(ball.getPos());
		//Texture(TextureAsset(L"soccer")).scale(1.0 / camera.getScale()).drawAt(ball.getPos());
		Println(camera.getScale());
	}

	//wall.draw();
	//broom.draw();

	camera.draw(Palette::Orange);

}

double gameManager::getTime()
{
	return 	timer.ms()/10/100.0;
}
