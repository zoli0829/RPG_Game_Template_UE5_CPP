#pragma once

#include "Enemy/AIBehaviour/ICombatStrategy.h"

class AEnemy;

class AttackStrategy : public ICombatStrategy
{
public:
	AttackStrategy();
	virtual void Execute(AEnemy* Enemy) override;
};
