// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_OrientToTargetActor.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "FunctionLibrary/FDFunctionLibrary.h"
#include "GAS/FDGameplayStatic.h"
#include "Kismet/KismetMathLibrary.h"


UBTService_OrientToTargetActor::UBTService_OrientToTargetActor()
{
	NodeName = TEXT("Native Orient Rotation To Target Actor");

	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	RotationInterpSpeed = 5.f;
	Interval = 0.f;
	RandomDeviation = 0.f;

	InTargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetActorKey), AActor::StaticClass());
}

void UBTService_OrientToTargetActor::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset =  GetBlackboardAsset())
	{
		InTargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UBTService_OrientToTargetActor::GetStaticDescription() const
{
	const FString KeyDescription = InTargetActorKey.SelectedKeyName.ToString();

	return FString::Printf(TEXT("Orient rotation to %s Key %s"), *KeyDescription, *GetStaticServiceDescription());
}

void UBTService_OrientToTargetActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetActorKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(ActorObject);

	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();
	
	if (OwningPawn)
	{
		if (UFDFunctionLibrary::NativeDoesActorHaveTag(OwningPawn, UFDGameplayStatic::StopOrientToRotation()) ||
			UFDFunctionLibrary::NativeDoesActorHaveTag(OwningPawn, UFDGameplayStatic::PlayerAttachParts()) ||
			UFDFunctionLibrary::NativeDoesActorHaveTag(OwningPawn, UFDGameplayStatic::MonsterGroggy()))
		{
			return;
		}
		
		if (!UFDFunctionLibrary::NativeDoesActorHaveTag(OwningPawn, UFDGameplayStatic::StopOrientToRotation()))
		{
			if (OwningPawn && TargetActor)
			{
				FVector PawnLocation = OwningPawn->GetActorLocation();
				FVector TargetLocation = TargetActor->GetActorLocation();

				TargetLocation.Z = PawnLocation.Z;

				const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(PawnLocation, TargetLocation);
				const FRotator TargetRot = FMath::RInterpTo(OwningPawn->GetActorRotation(), LookAtRot, DeltaSeconds, RotationInterpSpeed);

				FRotator NewRot = OwningPawn->GetActorRotation();
				NewRot.Yaw = TargetRot.Yaw;

				OwningPawn->SetActorRotation(NewRot);
			}
		}
	}
}
