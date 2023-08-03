// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ArcanoidPlayerController.generated.h"

class ABallActor;
class AArcanoidPlayerPlatform;
class UScoreUserWidget;

UENUM()
enum class EPlayerWindowId : uint8
{
	WaitForAnotherPlayers = 0,
	Game = 1,
	WinLevel = 2
};

UCLASS()
class ARCANOID_API AArcanoidPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AArcanoidPlayerController();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TSubclassOf<AArcanoidPlayerPlatform> PlatformClass;

	UPROPERTY()
	AArcanoidPlayerPlatform* Platform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ABallActor* Ball;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UScoreUserWidget* ScoreWidget;

	UPROPERTY()
	FTransform StartTransform;

	UPROPERTY(Replicated)
	int32 PlayerID = 0;

public:

	UFUNCTION()
	void SetStartTransform(FTransform NewStratTransform) { StartTransform = NewStratTransform; }

	UFUNCTION(Server, Reliable, WithValidation)
	void Initialize(int32 NewPlayer, ABallActor* NewBall);

	UFUNCTION(Server, Reliable, WithValidation)
	void SpawnPlatform(TSubclassOf<AArcanoidPlayerPlatform> PlatformSpawnClass);

	virtual void SetupInputComponent() override;

	UFUNCTION()
	FORCEINLINE int32 GetPlayerID() const { return PlayerID; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_InitializeHUD();

	UFUNCTION(Client, Reliable)
	void Client_SetHUDWindow(EPlayerWindowId windowId);

	UFUNCTION(Client, Reliable)
	void UpdateWidgetPlayerScore(int32 Score);

	UFUNCTION(Client, Reliable)
	void UpdateWidgetEnemyScore(int32 Score);

	UFUNCTION(Client, Reliable)
	void Client_ReadyButtonPressed(bool IsReady);

protected:
	UFUNCTION()
	void HorizontalMovement(float AxisValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_HorizontalMovement(float AxisValue);

	UFUNCTION(Server, Reliable)
	void Server_ChangePlayerReadyStatus(bool IsReadyStatus);
};
