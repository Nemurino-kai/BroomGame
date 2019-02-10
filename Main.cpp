# include <Siv3D.hpp> // OpenSiv3D v0.4.0
# include <HamFramework.hpp>

void Main()
{
	Window::Resize(400, 600);

	PhysicsWorld world(Vec2(0,-2.0));

	PhysicsBody wall = world.createLine(Vec2(0, 0), Line({ -0.01, 0 }, { 0.01, 0 }),none,none, PhysicsBodyType::Kinematic);

	PhysicsBody broom = world.createRect(Vec2(0, 0), RectF(Vec2(0, 0), Vec2(0.08, 3)));

	Array<PhysicsBody> balls;
	
	PhysicsWheelJoint wheel= world.createWheelJoint(wall, broom, broom.getPos()+Vec2(0.04,0), Vec2(0, 1), WheelJointState(false, 20.0, 60,0.5));

	CameraBox2D camera(Vec2(1, 2.5), 120);
	Vec2 previousPos = Window::GetState().windowPos;
	Vec2 previousDelta(0, 0);
	while (System::Update())
	{

		if (Input::MouseL.clicked)
		{
			balls.push_back(world.createCircle(Vec2(0, 4), 0.1));
		}

		Vec2 currentPos = Window::GetState().windowPos;

		const Vec2 currentDelta = (currentPos - previousPos);

		//Vec2 impulse = (currentDelta - previousDelta);
		Vec2 impulse = currentDelta;
		impulse = Vec2(impulse.x, 0);
		if (Input::KeyLeft.pressed) {
			impulse = Vec2(1, 0);
		}
		if (Input::KeyRight.pressed) {
			impulse = Vec2(-1, 0);
		}
		if (Input::KeyQ.clicked) {
			balls.push_back(world.createCircle(Vec2(-4, 3), 0.1));
			balls[int(balls.size()) - 1].applyLinearImpulse(Vec2(3.0, 0)*balls[int(balls.size()) - 1].getMass());
		}
		if (Input::KeyW.clicked) {
			balls.push_back(world.createCircle(Vec2(4, 3), 0.1));
			balls[int(balls.size()) - 1].applyLinearImpulse(Vec2(-3.0, 0)*balls[int(balls.size()) - 1].getMass());
		}

		previousDelta = currentDelta;
		previousPos = currentPos;

			Println(broom.getAngle());
			if (broom.getAngle() > Pi / 2 || broom.getAngle() < -Pi / 2) Println(L"GameOver!");
			//ball.applyLinearImpulse(-impulse * ball.getMass()/4.0);
			broom.applyForce(-impulse * broom.getMass()/5.0);

		world.update();
		auto t = camera.createTransformer();

		for (const auto& ball : balls)
		{
			ball.draw(HSV(0.5, 0.4, 1.0));
		}

		wall.draw();
		broom.draw();
		camera.draw(Palette::Orange);
	}
}