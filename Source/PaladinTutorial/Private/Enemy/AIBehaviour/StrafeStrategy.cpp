// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AIBehaviour/StrafeStrategy.h"
#include "Enemy/EnemyAIController.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Math/UnrealMathUtility.h"

UStrafeStrategy::UStrafeStrategy() {}

void UStrafeStrategy::Execute(AEnemy* Enemy)
{
	if (!Enemy) return;

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Enemy->GetWorld());
	if (!NavSystem) return;

	// Pick a random direction: left, right, or backward
	const TArray<float> YawOffsets = { -90.f, +90.f, 180.f };
	const float RandomYaw = YawOffsets[FMath::RandRange(0, YawOffsets.Num() - 1)];

	// Enemy rotation + offset
	const FRotator Rotation = Enemy->GetActorRotation();
	const FRotator TargetRotation(0.f, Rotation.Yaw + RandomYaw, 0.f);

	// Convert to vector
	const FVector Dir = TargetRotation.Vector();

	// Destination 500 units in that direction
	const FVector TargetPoint = Enemy->GetActorLocation() + Dir * 500.f;

	FNavLocation Result;
	if (NavSystem->GetRandomPointInNavigableRadius(TargetPoint, 300.f, Result))
	{
		if (AEnemyAIController* AI = Cast<AEnemyAIController>(Enemy->GetController()))
		{
			AI->MoveToLocation(Result);
		}
	}
}

bool UStrafeStrategy::HasReachedDestination(AEnemy* Enemy)
{
	AEnemyAIController* AIController = Cast<AEnemyAIController>(Enemy->GetController());
	if (AIController != nullptr)
	{
		EPathFollowingStatus::Type Status = AIController->GetMoveStatus();
		if (Status == EPathFollowingStatus::Type::Idle || Status == EPathFollowingStatus::Type::Waiting)
		{
			return true;
		}
	}
	
	return false;
}
