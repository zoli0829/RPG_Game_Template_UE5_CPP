#pragma once

#include "Enemy/AIBehaviour/ICombatStrategy.h"
#include "UPatrolStrategy.generated.h"

class AEnemy;

UCLASS()
class UPatrolStrategy : public UObject, public ICombatStrategy
{
	GENERATED_BODY()
	
public:
	UPatrolStrategy();
	virtual void Execute(AEnemy* Enemy) override;
	bool  HasReachedDestination(AEnemy* Enemy);
};
