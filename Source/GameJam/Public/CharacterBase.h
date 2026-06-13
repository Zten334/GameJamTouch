// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

struct FInputActionValue;
class UInputDataAsset;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GAMEJAM_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Input")
	TObjectPtr<UInputDataAsset> InputData;

#pragma region InputResponse
	void DoMove(const FInputActionValue& InputActionValue);
	void DoLook(const FInputActionValue& InputActionValue);
	
#pragma endregion
	

};
