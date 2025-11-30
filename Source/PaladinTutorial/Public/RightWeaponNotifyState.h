// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaladinCharacter.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "RightWeaponNotifyState.generated.h"

// Declarations
class APaladinCharacter;

/**
 * 
 */
UCLASS()
class PALADINTUTORIAL_API URightWeaponNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY()
	APaladinCharacter* PaladinCharacter;
	
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
