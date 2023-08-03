// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArcanoidPlayerPlatform.generated.h"

class UBoxComponent;

UCLASS()
class ARCANOID_API AArcanoidPlayerPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	AArcanoidPlayerPlatform();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UBoxComponent* PlatformCollision;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BodyMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 100.0f;
public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Client, Reliable, WithValidation)
	void Server_HorizontalMovement(float AxisValue);
};
