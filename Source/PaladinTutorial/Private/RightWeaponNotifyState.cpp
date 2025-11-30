// Fill out your copyright notice in the Description page of Project Settings.


#include "RightWeaponNotifyState.h"

void URightWeaponNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		PaladinCharacter = Cast<APaladinCharacter>(MeshComp->GetOwner());
		
		if (PaladinCharacter)
		{
			PaladinCharacter->ActivateRightWeapon();
		}
	}
}

void URightWeaponNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (PaladinCharacter)
		{
			PaladinCharacter->DeactivateRightWeapon();
		}
	}
}
