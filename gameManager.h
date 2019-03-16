#pragma once
#include <Siv3d.hpp>
# include <HamFramework.hpp>

class obstacleManager;

class gameManager {
private:
	bool endflag = false;

	Stopwatch timer;
	std::unique_ptr<obstacleManager> obstacleManage;
	//std::unique_ptr<PhysicsWorld> world;

	PhysicsWorld world;
	PhysicsBody wall;
	PhysicsBody broom;
	Array<std::pair<PhysicsBody,String>> balls;
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
	void addObstacle(Vec2 vec,double size,PhysicsMaterial material, String name);
};

//1秒毎に現在のタイムに応じて、障害物を投げるかを決める。
class obstacleManager {
	gameManager* gameManage;
	//前回障害物を投げたタイム
	double previousTime;

public:
	obstacleManager(gameManager* setGame);
	void update();
};