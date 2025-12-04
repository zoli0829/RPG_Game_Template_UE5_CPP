#pragma once

#include "Enemy/AIBehaviour/ICombatStrategy.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "UAttackStrategy.generated.h"

class AEnemy;


UCLASS()
class UAttackStrategy : public UObject, public ICombatStrategy
{
	GENERATED_BODY()
	
public:
	UAttackStrategy();
	virtual void Execute(AEnemy* Enemy) override;
	virtual void OnMoveCompleted(FAIRequestID, const FPathFollowingResult& Result, AEnemy* Enemy);
};
