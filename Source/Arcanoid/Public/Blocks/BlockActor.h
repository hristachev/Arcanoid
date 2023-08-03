// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockActor.generated.h"

class UBoxComponent;

UCLASS()
class ARCANOID_API ABlockActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABlockActor();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UBoxComponent* OverlapCollisionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Destroy Actor")
	float DelayTime = 0.1f;

	UPROPERTY(Replicated)
	float ScoreValue = 1.0f;

	UPROPERTY(Replicated)
	bool bIsStopBall = false;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, 
					  AActor* OtherActor, 
					  UPrimitiveComponent* OtherComp, 
					  int32 OtherBodyIndex, 
					  bool bFromSweep, 
					  const FHitResult &SweepResult);

	UFUNCTION()
	void DestroyDelay();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	float GetScoreValue() const { return ScoreValue; };

	UFUNCTION()
	void SetScoreValue(float Value) { ScoreValue = Value; }

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SetExtraMaterial(UMaterialInstance* ExtraMaterial);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetIsStopBall(bool State);

private:
	FTimerHandle DelayTimerHandle;
	
	UFUNCTION()
	void SetExtraMaterial(UMaterialInstance* ExtraMaterial);
};
