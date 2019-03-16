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

//1�b���Ɍ��݂̃^�C���ɉ����āA��Q���𓊂��邩�����߂�B
class obstacleManager {
	gameManager* gameManage;
	//�O���Q���𓊂����^�C��
	double previousTime;

public:
	obstacleManager(gameManager* setGame);
	void update();
};