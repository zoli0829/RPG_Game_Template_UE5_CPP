// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PaladinCharacter.generated.h"

// Declarations
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;

UCLASS()
class PALADINTUTORIAL_API APaladinCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APaladinCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Input actions
	UPROPERTY(EditAnywhere, Category="EnhancedInput")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, Category="EnhancedInput")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category="EnhancedInput")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category="EnhancedInput")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category="EnhancedInput")
	UInputAction* RunAction;

	UPROPERTY(EditAnywhere, Category="EnhancedInput")
	UInputAction* BasicAttackAction;

	UPROPERTY(EditAnywhere, Category="EnhancedInput")
	UInputAction* HeavyAttackAction;

	UPROPERTY(EditAnywhere, Category="EnhancedInput")
	UInputAction* SpinAttackAction;

	UPROPERTY(EditAnywhere, Category="EnhancedInput")
	UInputAction* JumpAttackAction;

	// Walk speed
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Movement")
	float WalkSpeed;

	// Run speed
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Movement")
	float RunSpeed;

	// Movement
	void Move(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	void Jump();
	void Running();
	void StopRunning();

	// Attacks
	void BasicAttack();
	void HeavyAttack();
	void SpinAttack();
	void JumpAttack();

	void AnimMontagePlay(UAnimMontage* MontageToPlay, FName SectionName = "Default", float PlayRate = 1.0f);

private:
	// Spring arm component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Camera", meta=(AllowPrivateAccess="true"))
	USpringArmComponent* SpringArmComponent;

	// Follow camera component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Camera", meta=(AllowPrivateAccess="true"))
	UCameraComponent* FollowCameraComponent;

	// Montages
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Montages", meta=(AllowPrivateAccess="true"))
	UAnimMontage* AttackMontage;
};
