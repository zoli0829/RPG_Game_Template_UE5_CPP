#pragma once

#include "Enemy/AIBehaviour/ICombatStrategy.h"

class AEnemy;

class PatrolStrategy : public ICombatStrategy
{
public:
	PatrolStrategy();
	virtual void Execute(AEnemy* Enemy) override;
	bool  HasReachedDestination(AEnemy* Enemy);
};
