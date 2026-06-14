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
	float CurrentTime = 0.f;
	float Health = 100.f;
	bool Has_Key = false;
	bool Has_Wallet = false;
	bool White_Cane = false;
	
	
};
