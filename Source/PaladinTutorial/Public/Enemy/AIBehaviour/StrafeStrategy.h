// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICombatStrategy.h"
#include "UObject/Object.h"
#include "StrafeStrategy.generated.h"

class AEnemy;

/**
 * 
 */
UCLASS()
class PALADINTUTORIAL_API UStrafeStrategy : public UObject, public ICombatStrategy
{
	GENERATED_BODY()

public:
	UStrafeStrategy();
	virtual void Execute(AEnemy* Enemy) override;
	bool HasReachedDestination(AEnemy* Enemy);
};
