// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreUserWidget.generated.h"

class UButton;
class UTextBlock;
class UWidgetSwitcher;

UCLASS()
class ARCANOID_API UScoreUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerScore(int newValue);

	UFUNCTION(BlueprintCallable)
	void UpdateEnemyScore(int newValue);

	UFUNCTION(BlueprintCallable)
	void SwitchWidget(int32 id);

	UFUNCTION(BlueprintCallable)
	void OnReadyButtonCLick();

	UFUNCTION()
	UTextBlock* GetReadyText() const { return ReadyText; }

	UFUNCTION()
	void SetFlipFlopReadyClick(bool bValue) { FlipFlopReadyClick = bValue; }
	 
protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* PlayerScore;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* OpponentScore;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	UButton* ReadyButton;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ReadyText;


private:
	bool FlipFlopReadyClick = true;
};
