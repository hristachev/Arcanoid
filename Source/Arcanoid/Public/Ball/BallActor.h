// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallActor.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class APlayerStart;

UCLASS()
class ARCANOID_API ABallActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABallActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USphereComponent* BodyCollision;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APlayerStart* PlayerStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball Params")
	float MoveSpeed = 100.0f;

	UPROPERTY(Replicated)
	bool bIsMoving = false;

	UPROPERTY(Replicated)
	int32 PlayerID = 0;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void StartMove();

	UFUNCTION(BlueprintCallable)
	void StopMove();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	APlayerStart* GetPlayerStart() const { return PlayerStart; }

	void SetPlayerID(int32 ID);

	UFUNCTION()
	void AddScoreCheck(int32 ScoreValue);

	FTimerHandle GetStartMoveTimerHandle() const { return StartMoveTimerHandle; }

protected:

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Move(float DeltaTime);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartMove();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StopMove();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetPlayerID(int32 ID);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddScoreCheck(int32 ScoreValue);

private:
	FTimerHandle StartMoveTimerHandle;
	
};
