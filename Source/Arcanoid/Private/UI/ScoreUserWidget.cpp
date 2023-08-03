// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ScoreUserWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Player/ArcanoidPlayerController.h"

void UScoreUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ReadyButton->OnClicked.AddDynamic(this, &UScoreUserWidget::OnReadyButtonCLick);

	ReadyText->SetOpacity(0.0f);
}

void UScoreUserWidget::UpdatePlayerScore(int newValue)
{
	if (PlayerScore)
	{
		TArray<FStringFormatArg> Args;
		Args.Add(FStringFormatArg(newValue));
		auto String { FString::Format(TEXT("{0}") , Args) };
		PlayerScore->SetText(FText::FromString(String));
	}
}

void UScoreUserWidget::UpdateEnemyScore(int newValue)
{
	if (OpponentScore)
	{
		TArray<FStringFormatArg> Args;
		Args.Add(FStringFormatArg(newValue));
		auto String { FString::Format(TEXT("{0}") , Args) };
		OpponentScore->SetText(FText::FromString(String));
	}
}

void UScoreUserWidget::SwitchWidget(int32 id)
{
	WidgetSwitcher->SetActiveWidgetIndex(id);
}

void UScoreUserWidget::OnReadyButtonCLick()
{
	const auto Player = GetOwningPlayer();
	if(!Player) return;

	const auto LocalPlayer = Cast<AArcanoidPlayerController>(Player);
	
	if (FlipFlopReadyClick)
	{
		LocalPlayer->Client_ReadyButtonPressed(true);
		ReadyText->SetOpacity(1.0f);
	}
	else
	{
		LocalPlayer->Client_ReadyButtonPressed(false);
		ReadyText->SetOpacity(0.0f);
	}

	FlipFlopReadyClick = !FlipFlopReadyClick;
}

