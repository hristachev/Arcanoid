// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ArcanoidPlayerPawn.generated.h"

class ABallActor;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ARCANOID_API AArcanoidPlayerPawn : public ACharacter
{
	GENERATED_BODY()

public:
	AArcanoidPlayerPawn();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;
	
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};



