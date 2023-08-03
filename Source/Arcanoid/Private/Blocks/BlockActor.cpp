// Fill out your copyright notice in the Description page of Project Settings.


#include "Arcanoid/Public/Blocks/BlockActor.h"

#include "Ball/BallActor.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

ABlockActor::ABlockActor()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Block Mesh");
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetIsReplicated(true);

	OverlapCollisionComponent = CreateDefaultSubobject<UBoxComponent>("Overlap Collision");
	OverlapCollisionComponent->SetupAttachment(StaticMeshComponent);

	SetReplicates(true);
}

void ABlockActor::BeginPlay()
{
	Super::BeginPlay();

	OverlapCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABlockActor::BeginOverlap);
	
}

void ABlockActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	GetWorldTimerManager().ClearTimer(DelayTimerHandle);
}

void ABlockActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const auto InstigatorActor = Cast<ABallActor>(OtherActor);
	if (InstigatorActor)
	{
		InstigatorActor->AddScoreCheck(ScoreValue);

		GetWorldTimerManager().SetTimer(DelayTimerHandle, this, &ABlockActor::DestroyDelay, DelayTime);

		if (bIsStopBall)
		{
			InstigatorActor->StopMove();
			FTimerHandle StartMoveTimerHandle = InstigatorActor->GetStartMoveTimerHandle();
			const auto StopDelay = 2.0f;
			GetWorldTimerManager().SetTimer(StartMoveTimerHandle, InstigatorActor, &ABallActor::StartMove, StopDelay);
		}
	}
}

void ABlockActor::DestroyDelay()
{
	StaticMeshComponent->SetVisibility(false);
	OverlapCollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABlockActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlockActor, ScoreValue);
	DOREPLIFETIME_CONDITION(ABlockActor, bIsStopBall, COND_SimulatedOnly);
}

void ABlockActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlockActor::Multicast_SetExtraMaterial_Implementation(UMaterialInstance* ExtraMaterial)
{
	SetExtraMaterial(ExtraMaterial);
}

void ABlockActor::SetIsStopBall_Implementation(bool State)
{
	bIsStopBall = State;
}

bool ABlockActor::SetIsStopBall_Validate(bool State)
{
	return true;
}

void ABlockActor::SetExtraMaterial(UMaterialInstance* ExtraMaterial)
{
	if(!ExtraMaterial) return;
	
	StaticMeshComponent->SetMaterial(0, ExtraMaterial);
}


