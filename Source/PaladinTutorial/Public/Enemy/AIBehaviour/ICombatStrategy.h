#pragma once

class AEnemy;

class ICombatStrategy
{
public:
	virtual void Execute(AEnemy* Enemy) = 0;
};
