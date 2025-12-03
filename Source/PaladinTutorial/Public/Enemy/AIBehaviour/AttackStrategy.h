#pragma once

#include "Enemy/AIBehaviour/ICombatStrategy.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"

class AEnemy;

class AttackStrategy : public ICombatStrategy
{
public:
	AttackStrategy();
	virtual void Execute(AEnemy* Enemy) override;
	virtual void OnMoveCompleted(FAIRequestID, const FPathFollowingResult& Result, AEnemy* Enemy);
};
