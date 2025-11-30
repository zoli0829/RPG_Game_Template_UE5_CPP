// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyRightWeapon.h"

void UEnemyRightWeapon::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		Enemy = Cast<AEnemy>(MeshComp->GetOwner());

		if (Enemy)
		{
			Enemy->ActivateRightWeapon();
		}
	}
}

void UEnemyRightWeapon::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (Enemy)
		{
			Enemy->DeactivateRightWeapon();
		}
	}
}
