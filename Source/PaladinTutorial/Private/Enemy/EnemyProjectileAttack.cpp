// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyProjectileAttack.h"
#include "Enemy/Enemy.h"

void UEnemyProjectileAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		AEnemy* const Enemy = Cast<AEnemy>(MeshComp->GetOwner());
		if (Enemy)
		{
			Enemy->SpawnProjectile();
		}
	}
}
