// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PaladinAnimInstance.generated.h"

// Declarations
class APaladinCharacter;

/**
 * 
 */
UCLASS()
class PALADINTUTORIAL_API UPaladinAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// Used in animation blueprints
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	// We can initialize variables and use them in blueprints
	virtual void NativeInitializeAnimation() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	APaladinCharacter* PaladinCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	bool bIsBlocking;
};
