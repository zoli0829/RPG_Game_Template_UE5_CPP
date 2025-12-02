#include "Enemy/AIBehaviour/PatrolStrategy.h"
#include "NavigationSystem.h"
#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Enemy/EnemyAIController.h"
#include "Math/UnrealMathUtility.h"

PatrolStrategy::PatrolStrategy()
{
}

void PatrolStrategy::Execute(AEnemy* Enemy)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("Inside Execute Patrol Strategy"));

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
