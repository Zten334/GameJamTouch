// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TemporatyUIComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM_API UTemporatyUIComponent : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable)
	void Init(const float Duration);
	
protected:
	FTimerHandle TimerHandle;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "UI")
	float Duration;
	
};
