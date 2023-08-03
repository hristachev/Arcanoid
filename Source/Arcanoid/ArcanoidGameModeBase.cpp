// Copyright Epic Games, Inc. All Rights Reserved.


#include "ArcanoidGameModeBase.h"
#include "Ball/BallActor.h"
#include "Blocks/BlockRandomGenerator.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Public/utils.h"
#include "Public/Player/ArcanoidPlayerController.h"
#include "Public/Player/ArcanoidPlayerPawn.h"

DEFINE_LOG_CATEGORY_STATIC(LogArcanoidGM, All, All);

AArcanoidGameModeBase::AArcanoidGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	DefaultPawnClass = AArcanoidPlayerPawn::StaticClass();
	PlayerControllerClass = AArcanoidPlayerController::StaticClass();
}

void AArcanoidGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void AArcanoidGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UWorld* World = GetWorld();
	if (World && (!Player1Start || !Player2Start))
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundActors);
		if(FoundActors.Num() > 0)
			Player1Start = (APlayerStart*)FoundActors[0];
		if(FoundActors.Num() > 1)
			Player2Start = (APlayerStart*)FoundActors[1];
	}

	AArcanoidPlayerController* CurrentPlayer = NULL;
	APlayerStart* StartPos = NULL;
	int32 PlayerID;

	if(Player1 == NULL)
	{
		Player1 = Cast<AArcanoidPlayerController>(NewPlayer);
		CurrentPlayer = Player1;
		StartPos = Player1Start;
		PlayerID = 1;
		
		UE_LOG(LogArcanoidGM, Display, TEXT("GM: Init Player 1"));
	}
	else if (Player2 == NULL)
	{
		Player2 = Cast<AArcanoidPlayerController>(NewPlayer);
		CurrentPlayer = Player2;
		StartPos = Player2Start;
		PlayerID = 2;
		
		UE_LOG(LogArcanoidGM, Display, TEXT("GM: Init Player 2"));
	}
	else
	{
		UE_LOG(LogArcanoidGM, Warning, TEXT("GM: Game Is Full Now!! Try to connect later!"));
		return;
	}

	AArcanoidPlayerPawn* NewPawn = Cast<AArcanoidPlayerPawn>(NewPlayer->GetPawn());
	if(!NewPawn)
	{
		NewPawn = World->SpawnActor<AArcanoidPlayerPawn>(DefaultPawnClass);
		
	}

	TArray<ABallActor*> FoundBallActors;
	utils::FindAllActors<ABallActor>(World, FoundBallActors);
	auto FindBallByPlayerStart = [](const TArray<ABallActor*>& Array, const APlayerStart* SearchObject)
	{
		auto Result = Array.FindByPredicate([&](const ABallActor *Ball)
		{
			return Ball->GetPlayerStart() == SearchObject;
		});
		return Result != nullptr ? *Result : nullptr;
	};
	

	if (StartPos && NewPawn)
	{
		auto PlayerBall { FindBallByPlayerStart(FoundBallActors, StartPos)};
		NewPawn->SetActorLocation(StartPos->GetActorLocation());
		NewPawn->SetActorRotation(StartPos->GetActorRotation());
		NewPlayer->SetPawn(NewPawn);

		CurrentPlayer->SetStartTransform(StartPos->GetActorTransform());
		CurrentPlayer->Client_InitializeHUD();
		CurrentPlayer->Initialize(PlayerID, PlayerBall);

		if(Player1 != nullptr && Player2 != nullptr)
		{
			Player1->Client_SetHUDWindow(EPlayerWindowId::Game);
			Player2->Client_SetHUDWindow(EPlayerWindowId::Game);
			StartGame();
		}
	}
	else
	{
		UE_LOG(LogArcanoidGM, Error, TEXT("Start Position is not setted in GameMode!!!"));
	}
	
}

void AArcanoidGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ScorePlayer1 >= 35 || ScorePlayer2 >= 35)
	{
		StopGame();
		Player1->Client_SetHUDWindow(EPlayerWindowId::WinLevel);
		Player2->Client_SetHUDWindow(EPlayerWindowId::WinLevel);
	}
}

void AArcanoidGameModeBase::PlayerScoreAdd(int32 PlayerID, int32 ScoreValue)
{
	auto CurrentPlayerScore = 0;
	if (PlayerID == 1)
	{
		CurrentPlayerScore = ScorePlayer1 + ScoreValue;
		ScorePlayer1 = CurrentPlayerScore;
		UE_LOG(LogArcanoidGM, Display, TEXT(" Score: %i"), CurrentPlayerScore);
		
	}
	else if (PlayerID == 2)
	{
		CurrentPlayerScore = ScorePlayer2 + ScoreValue;
		ScorePlayer2 = CurrentPlayerScore;
	}

	for ( auto It = GetWorld()->GetPlayerControllerIterator(); It ; ++It)
	{
		auto PlayerController = Cast<AArcanoidPlayerController>(*It);
		if (PlayerController != nullptr)
		{
			if (PlayerController->GetPlayerID() == PlayerID)
			{
				PlayerController->UpdateWidgetPlayerScore(CurrentPlayerScore);
			}
			else
			{
				PlayerController->UpdateWidgetEnemyScore(CurrentPlayerScore);
			}
		}
	}
}

void AArcanoidGameModeBase::SetPlayerReady_Implementation(bool ReadyState, int32 PlayerID)
{
	if(!GetWorld()) return;
	
	if (ReadyPlayers < 0)
	{
		ReadyPlayers = 0;
	}
	
	if (ReadyState)
	{
		++ReadyPlayers;
		if (ReadyPlayers == 2)
		{
			TArray<ABlockRandomGenerator*> FoundABlockRandomGenerators;
			utils::FindAllActors<ABlockRandomGenerator>(GetWorld(), FoundABlockRandomGenerators);

			TArray<ABallActor*> FoundBallActors;
			utils::FindAllActors<ABallActor>(GetWorld(), FoundBallActors);

			for (auto FoundBallActor : FoundBallActors)
			{
				FoundBallActor->Reset();
			}

			for (auto FoundABlockRandomGenerator : FoundABlockRandomGenerators)
			{
				const auto Generator = Cast<ABlockRandomGenerator>(FoundABlockRandomGenerator);
				if(!Generator) return;

				Generator->Server_ClearGenerate();
			}
			
			for (const auto FoundABlockRandomGenerator : FoundABlockRandomGenerators)
			{
				const auto Generator = Cast<ABlockRandomGenerator>(FoundABlockRandomGenerator);
				if(!Generator) return;
				
				Generator->Server_GenerateBlockGrid();
			}
			
			ReadyPlayers = 0;
			const auto CurrentScore = 0.0f;
			ScorePlayer1 = CurrentScore;
			ScorePlayer2 = CurrentScore;
			
			for ( auto It = GetWorld()->GetPlayerControllerIterator(); It ; ++It)
			{
				auto PlayerController = Cast<AArcanoidPlayerController>(*It);
				if (PlayerController != nullptr)
				{
					if (PlayerController->GetPlayerID() == PlayerID)
					{
						PlayerController->UpdateWidgetPlayerScore(CurrentScore);
					}
					else
					{
						PlayerController->UpdateWidgetEnemyScore(CurrentScore);
					}
				}
			}
			
			Player1->Client_SetHUDWindow(EPlayerWindowId::Game);
			Player2->Client_SetHUDWindow(EPlayerWindowId::Game);
			StartGame();
		}
	}
	else
	{
		if(ReadyPlayers == 0)
		{
			ReadyPlayers = 0;
		}
		else
		{
			--ReadyPlayers;	
		}
	}
}

bool AArcanoidGameModeBase::StartGame()
{
	TArray<ABallActor*> FoundActors;
	utils::FindAllActors<ABallActor>(GetWorld(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		for (auto FoundActor : FoundActors)
		{
			FoundActor->StartMove();
		}
		return true;
	}
	return false;
}

bool AArcanoidGameModeBase::StopGame()
{
	TArray<ABallActor*> FoundActors;
	utils::FindAllActors<ABallActor>(GetWorld(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		for (auto FoundActor : FoundActors)
		{
			FoundActor->StopMove();
		}
		return true;
	}
	return false;
}

