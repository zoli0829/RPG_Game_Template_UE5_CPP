#include "Enemy/AIBehaviour/UPatrolStrategy.h"
#include "NavigationSystem.h"
#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Enemy/EnemyAIController.h"
#include "Math/UnrealMathUtility.h"
#include "Navigation/PathFollowingComponent.h"

UPatrolStrategy::UPatrolStrategy() {}

void UPatrolStrategy::Execute(AEnemy* Enemy)
{
	// Get NavMesh data
	FNavAgentProperties NavAgentProps;
	const ANavigationData* NavData = UNavigationSystemV1::GetCurrent(Enemy->GetWorld())->GetNavDataForProps(NavAgentProps);

	if (NavData != nullptr)
	{
		// Placeholder for our result
		FNavLocation ResultLocation;

		// Get Point in Radius
		UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Enemy->GetWorld());
		if (NavSystem && NavSystem->GetRandomReachablePointInRadius(Enemy->GetActorLocation(), 800.0f, ResultLocation))
		{
			// Use the AI controller to move to the result location
			AEnemyAIController* AIController = Cast<AEnemyAIController>(Enemy->GetController());

			// Check if we got controller and move to the point
			if (AIController != nullptr)
			{
				AIController->MoveToLocation(ResultLocation.Location);
			}
		}
	}
}

bool UPatrolStrategy::HasReachedDestination(AEnemy* Enemy)
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
