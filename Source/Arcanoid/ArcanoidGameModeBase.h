// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArcanoidGameModeBase.generated.h"

class APlayerStart;
class AArcanoidPlayerController;

UCLASS()
class ARCANOID_API AArcanoidGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArcanoidGameModeBase();

protected:
	UPROPERTY()
	AArcanoidPlayerController* Player1 = NULL;

	UPROPERTY()
	AArcanoidPlayerController* Player2 = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APlayerStart* Player1Start;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APlayerStart* Player2Start;

	UPROPERTY()
	int32 ScorePlayer1 = 0;
	UPROPERTY()
	int32 ScorePlayer2 = 0;

	UPROPERTY()
	int32 ReadyPlayers = 0;

public:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Tick(float DeltaSeconds) override;
	
	void PlayerScoreAdd(int32 PlayerID, int32 ScoreValue);

	UFUNCTION(Server, Reliable)
	void SetPlayerReady(bool ReadyState, int32 PlayerID);

private:
	bool StartGame();
	bool StopGame();
	
};
