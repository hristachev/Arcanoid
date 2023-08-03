// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockRandomGenerator.generated.h"

class ABlockActor;
class UMaterialInstance;

UCLASS()
class ARCANOID_API ABlockRandomGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ABlockRandomGenerator();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TSubclassOf<ABlockActor> BlockClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Extras")
	UMaterialInstance* ExtraScoreMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Extras")
	UMaterialInstance* ExtraStopBallMaterial;

	UPROPERTY()
	ABlockActor* BlockActor;

	UPROPERTY(Replicated)
	bool bIsExtra = false;

	UPROPERTY(Replicated)
	bool bIsStopBallExtra = false;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Spawn Params")
	int32 Rows = 15;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Spawn Params")
	int32 Columns = 7;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
public:	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_GenerateBlockGrid();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ClearGenerate();

	UFUNCTION(Client, Unreliable)
	void Client_SetExtras(AActor* ExtrasActor);
};
