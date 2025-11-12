// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GC/Player/LightningBeamCue.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"


bool ALightningBeamCue::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	Super::OnExecute_Implementation(MyTarget, Parameters);
	
	if (BeamEffect)
	{
		UNiagaraComponent* Beam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BeamEffect,
			Parameters.Location,
			FRotator::ZeroRotator
		);

		if (Beam)
		{
			Beam->SetVectorParameter("BeamStart", Parameters.Location);
			Beam->SetVectorParameter("BeamEnd", Parameters.Normal);
			UE_LOG(LogTemp, Warning, TEXT("Spawn Beam Effect"));
			return true;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Not Beam Effect"));
	return false;
}
