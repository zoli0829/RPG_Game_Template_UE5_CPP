// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "EnemyRightWeapon.generated.h"

// Declarations
class AEnemy;

/**
 * 
 */
UCLASS()
class PALADINTUTORIAL_API UEnemyRightWeapon : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY()
	AEnemy* Enemy;

	virtual void NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference
	) override;
	
	virtual void NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
	) override;
};
