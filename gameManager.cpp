#include <Siv3d.hpp>
#include "gameManager.h"

gameManager::gameManager()
{
	timer.start();
}

bool gameManager::isOver()
{
	return endflag;
}

void gameManager::update()
{
	if (Input::MouseL.clicked) endflag = true;
}

void gameManager::draw()
{
}

double gameManager::getTime()
{
	return 	timer.ms()/1000.0;
}
