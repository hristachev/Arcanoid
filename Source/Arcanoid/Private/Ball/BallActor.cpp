// Fill out your copyright notice in the Description page of Project Settings.


#include "Arcanoid/Public/Ball/BallActor.h"

#include "Arcanoid/ArcanoidGameModeBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ABallActor::ABallActor()
{
	PrimaryActorTick.bCanEverTick = true;

	BodyCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Ball Collision"));
    SetRootComponent(BodyCollision);
	
    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball Mesh"));
    BodyMesh->SetupAttachment(RootComponent);
    BodyMesh->SetIsReplicated(true);
	
    SetReplicates(true);
    SetReplicateMovement(true);
}

void ABallActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABallActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetNetMode() != NM_Client)
	{
		Server_Move(DeltaTime);
	}

}

void ABallActor::StartMove()
{
	Server_StartMove();
}

void ABallActor::StopMove()
{
	Server_StopMove();
}

void ABallActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABallActor, bIsMoving);
	DOREPLIFETIME(ABallActor, PlayerID);
}

void ABallActor::SetPlayerID(int32 ID)
{
	Server_SetPlayerID(ID);
}

void ABallActor::AddScoreCheck(int32 ScoreValue)
{
	Server_AddScoreCheck(ScoreValue);
}

void ABallActor::Server_AddScoreCheck_Implementation(int32 ScoreValue)
{
	if(!GetWorld()) return;
	auto GameMode = Cast<AArcanoidGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode != nullptr)
	{
		GameMode->PlayerScoreAdd(PlayerID, ScoreValue);
	}
}

bool ABallActor::Server_AddScoreCheck_Validate(int32 ScoreValue)
{
	return true;
}

void ABallActor::Server_SetPlayerID_Implementation(int32 ID)
{
	PlayerID = ID;
}

bool ABallActor::Server_SetPlayerID_Validate(int32 ID)
{
	return ID != 0;
}

void ABallActor::Server_StopMove_Implementation()
{
	bIsMoving = false;
}

bool ABallActor::Server_StopMove_Validate()
{
	return true;
}

void ABallActor::Server_StartMove_Implementation()
{
	bIsMoving = true;
	GetWorldTimerManager().ClearTimer(StartMoveTimerHandle);
}

bool ABallActor::Server_StartMove_Validate()
{
	return true;
}

void ABallActor::Server_Move_Implementation(float DeltaTime)
{
	if(!bIsMoving) return;
	
	const auto CurrentLocation = GetActorLocation();
	const auto ForwardVector = GetActorForwardVector();
	FVector NewLocation = CurrentLocation + ForwardVector * MoveSpeed * DeltaTime;
	FHitResult HitResult;
	if (!SetActorLocation(NewLocation, true, &HitResult))
	{
		FVector MoveVector = ForwardVector - CurrentLocation;
		MoveVector.Normalize();

		float AimAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(MoveVector,HitResult.ImpactNormal)));
		MoveVector = MoveVector.RotateAngleAxis(AimAngle, FVector(0,0,0));
		
		FVector NewTargetMove = NewLocation + MoveVector * 1200;
		NewTargetMove.Z = CurrentLocation.Z;
		FVector NewBallPosition = CurrentLocation + MoveVector * DeltaTime * MoveSpeed;
		SetActorLocation(NewBallPosition);

		const auto CurrentRotation = GetActorRotation();
		FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, NewTargetMove);
		NewRotation = HitResult.ImpactNormal.Rotation();
		NewRotation.Yaw += FMath::RandRange(-45, 45);
		SetActorRotation(NewRotation);
	}
}

bool ABallActor::Server_Move_Validate(float DeltaTime)
{
	return true;
}

