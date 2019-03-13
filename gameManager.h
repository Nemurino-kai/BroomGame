#pragma once
#include <Siv3d.hpp>
# include <HamFramework.hpp>


class gameManager {
private:
	bool endflag = false;

	Stopwatch timer;
	//std::unique_ptr<PhysicsWorld> world;

	PhysicsWorld world;
	PhysicsBody wall;
	PhysicsBody broom;
	Array<PhysicsBody> balls;
	PhysicsWheelJoint wheel;
	CameraBox2D camera;
	Vec2 previousPos;
	Vec2 previousDelta;

public:
	gameManager();
	bool isOver();
	void update();
	void draw();
	double getTime();
};