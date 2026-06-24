// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "State.generated.h"

UCLASS()
class GAMEJAM_API AState : public APlayerState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float CurrentTime = 0.f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float Health = 100.f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	bool Has_Key = false;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	bool Has_Wallet = false;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	bool White_Cane = false;
	
	
};
