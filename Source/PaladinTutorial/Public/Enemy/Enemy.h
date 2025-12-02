// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HitInterface.h"
#include "AIBehaviour/AttackStrategy.h"
#include "AIBehaviour/PatrolStrategy.h"
#include "Components/BoxComponent.h"
#include "Enemy.generated.h"

class UAnimMontage;
class AEnemyAIController;

UCLASS()
class PALADINTUTORIAL_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	// Enter and exit combat
	void EnterCombat();
	void ExitCombat();

	// Activate and deactivate weapon boxes
	virtual void ActivateRightWeapon();
	virtual void DeactivateRightWeapon();
	
	// Called every frame
    virtual void Tick(float DeltaTime) override;

	// Override hit interface
	virtual void HitInterface_Implementation(FHitResult HitResult) override;
	
	virtual float TakeDamage(float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
		) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Enemy AI Controller
	UPROPERTY()
	AEnemyAIController* EnemyAIController;

	// Right weapon overlap
	UFUNCTION()
	void OnRightWeaponOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void MeleeAttack();
	void ResetAttack();

	FName GetAttackSectionName(int32 SectionCount);

private:
	// Combat strategy logic
	PatrolStrategy PatrolStrategy;
	AttackStrategy AttackStrategy;
	TSharedPtr<ICombatStrategy> CombatStrategy;

	// This is used in tick for patrolling
	bool bIsWaiting;
	bool bCanPatrol;
	FTimerHandle PatrolDelayTimer;
	void EnemyPatrol();
	
	FTimerHandle TimerAttack;

	// Collisions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	UBoxComponent* RightWeaponCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	FName RightWeaponSocketName = "RightWeaponSocket";
	
	// Base damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	float BaseDamage;

	// Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	float MaxHealth;

	// Attack montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	UAnimMontage* AttackMontage;
};
