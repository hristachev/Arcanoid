// Fill out your copyright notice in the Description page of Project Settings.


#include "Arcanoid/Public/Player/ArcanoidPlayerPawn.h"

#include "Ball/BallActor.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"

AArcanoidPlayerPawn::AArcanoidPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComponent->SetupAttachment(SpringArmComponent);

	SetReplicates(true);
}

void AArcanoidPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

}

void AArcanoidPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArcanoidPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}



