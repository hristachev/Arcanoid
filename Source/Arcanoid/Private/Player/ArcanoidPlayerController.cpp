// Fill out your copyright notice in the Description page of Project Settings.


#include "Arcanoid/Public/Player/ArcanoidPlayerController.h"

#include "Arcanoid/ArcanoidGameModeBase.h"
#include "Arcanoid/Public/Player/Platform/ArcanoidPlayerPlatform.h"
#include "Ball/BallActor.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Net/UnrealNetwork.h"
#include "UI/ScoreUserWidget.h"

AArcanoidPlayerController::AArcanoidPlayerController()
{
	SetReplicates(true);
}

void AArcanoidPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	InputComponent->BindAxis("HorizontalMove", this, &AArcanoidPlayerController::HorizontalMovement);
}

void AArcanoidPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArcanoidPlayerController, PlayerID);
}

void AArcanoidPlayerController::Client_SetHUDWindow_Implementation(EPlayerWindowId windowId)
{
	if (ScoreWidget)
	{
		ScoreWidget->SwitchWidget(static_cast<int32>(windowId));
		if(windowId == EPlayerWindowId::WinLevel)
		{
			SetShowMouseCursor(true);
		}
		else
		{
			SetShowMouseCursor(false);
			ScoreWidget->GetReadyText()->SetOpacity(0.0f);
			ScoreWidget->SetFlipFlopReadyClick(true);
		}
	}
}

void AArcanoidPlayerController::UpdateWidgetPlayerScore_Implementation(int32 Score)
{
	ScoreWidget->UpdatePlayerScore(Score);
}

void AArcanoidPlayerController::Client_InitializeHUD_Implementation()
{
	if (!ScoreWidget)
	{
		ScoreWidget = CreateWidget<UScoreUserWidget>(this, WidgetClass);
		if (ScoreWidget)
		{
			ScoreWidget->UpdatePlayerScore(0);
			ScoreWidget->UpdateEnemyScore(0);
			ScoreWidget->AddToViewport();
		}
	}
}

bool AArcanoidPlayerController::Client_InitializeHUD_Validate()
{
	return (WidgetClass != nullptr);
}

void AArcanoidPlayerController::UpdateWidgetEnemyScore_Implementation(int32 Score)
{
	ScoreWidget->UpdateEnemyScore(Score);
}

void AArcanoidPlayerController::Client_ReadyButtonPressed_Implementation(bool IsReady)
{
	Server_ChangePlayerReadyStatus(IsReady);
}

void AArcanoidPlayerController::HorizontalMovement(float AxisValue)
{
	Server_HorizontalMovement(AxisValue);
}

void AArcanoidPlayerController::Server_ChangePlayerReadyStatus_Implementation(bool IsReadyStatus)
{
	if(!GetWorld()) return;
	const auto GM = Cast<AArcanoidGameModeBase>(GetWorld()->GetAuthGameMode());

	if (GM)
	{
		GM->SetPlayerReady(IsReadyStatus, PlayerID);
	}
}


void AArcanoidPlayerController::Server_HorizontalMovement_Implementation(float AxisValue)
{
	if(!Platform) return;

	Platform->Server_HorizontalMovement(AxisValue);
}

bool AArcanoidPlayerController::Server_HorizontalMovement_Validate(float AxisValue)
{
	return true;
}

void AArcanoidPlayerController::SpawnPlatform_Implementation(TSubclassOf<AArcanoidPlayerPlatform> PlatformSpawnClass)
{
	Platform = (AArcanoidPlayerPlatform*)GetWorld()->SpawnActor<AArcanoidPlayerPlatform>(PlatformClass);
	if (Platform)
	{
		Platform->SetActorLocation(StartTransform.GetLocation());
		Platform->SetActorRotation(StartTransform.GetRotation());
	}
}

bool AArcanoidPlayerController::SpawnPlatform_Validate(TSubclassOf<AArcanoidPlayerPlatform> PlatformSpawnClass)
{
	return PlatformClass != NULL;
}

void AArcanoidPlayerController::Initialize_Implementation(int32 NewPlayer, ABallActor* NewBall)
{
	if(Platform)
		Platform->Destroy();

	SpawnPlatform(PlatformClass);

	PlayerID = NewPlayer;
	Ball = NewBall;
	Ball->SetPlayerID(PlayerID);
}

bool AArcanoidPlayerController::Initialize_Validate(int32 NewPlayer, ABallActor* NewBall)
{
	return (NewPlayer != 0 && NewBall != nullptr);
}
