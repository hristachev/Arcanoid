// Fill out your copyright notice in the Description page of Project Settings.


#include "Arcanoid/Public/Player/Platform/ArcanoidPlayerPlatform.h"
#include "Components/BoxComponent.h"

AArcanoidPlayerPlatform::AArcanoidPlayerPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	PlatformCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	SetRootComponent(PlatformCollision);
	
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>("Platform");
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetIsReplicated(true);


	SetReplicates(true);
	SetReplicateMovement(true);
}

void AArcanoidPlayerPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArcanoidPlayerPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArcanoidPlayerPlatform::Server_HorizontalMovement_Implementation(float AxisValue)
{
	const auto CurrentLocation = GetActorLocation();
	const auto MoveDirection = GetActorRightVector();
	const auto NextLocation = CurrentLocation + MoveDirection * MoveSpeed * AxisValue;
	if(!SetActorLocation(NextLocation, true))
	{
		SetActorLocation(CurrentLocation);
	}
}

bool AArcanoidPlayerPlatform::Server_HorizontalMovement_Validate(float AxisValue)
{
	return true;
}

