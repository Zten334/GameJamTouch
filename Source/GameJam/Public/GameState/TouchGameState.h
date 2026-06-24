// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TouchGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHit, FString, Msg, float, duration);

/**
 * 
 */
UCLASS()
class GAMEJAM_API ATouchGameState : public AGameStateBase
{
	GENERATED_BODY()
public:	
	UPROPERTY(BlueprintAssignable,BlueprintCallable,Category = "Delegates")
	FOnHit OnHit;
	
	
};
