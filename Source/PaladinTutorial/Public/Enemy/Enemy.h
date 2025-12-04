// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HitInterface.h"
#include "AIBehaviour/UAttackStrategy.h"
#include "AIBehaviour/UPatrolStrategy.h"
#include "Components/BoxComponent.h"
#include "Enemy.generated.h"

// List of AI states
UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle	UMETA(DisplayName="Idle"),
	Patrol	UMETA(DisplayName="Patrol"),
	Attack	UMETA(DisplayName="Attack"),
	Combat	UMETA(DisplayName="Combat"),
	Strafe	UMETA(DisplayName="Strafe"),
	Dead	UMETA(DisplayName="Dead")
};

// Declarations
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

	void MeleeAttack();

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

	// Used for AI state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	EAIState CurrentState;
	
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
	void ResetAttack();

	FName GetAttackSectionName(int32 SectionCount);

private:
	// Combat strategy logic
	TWeakObjectPtr<UPatrolStrategy> PatrolStrategy;
	TWeakObjectPtr<UAttackStrategy> AttackStrategy;
	
	// This is used in tick for patrolling
	bool bIsWaiting;
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

	// AI values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	float AcceptanceRange;

public:
	// Getters and setters
	FORCEINLINE float GetAttackRange() const { return AttackRange; }
	FORCEINLINE float GetAcceptanceRange() const { return AcceptanceRange; }

	FORCEINLINE float SetAttackRange(const float AttackRangeSet)
	{
		AttackRange = AttackRangeSet;
		return AttackRange;
	}
};
