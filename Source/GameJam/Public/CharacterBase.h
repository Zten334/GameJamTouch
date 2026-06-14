// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

class URayCastComponent;
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
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "SpringArm", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Raycast", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URayCastComponent> RayCastComponent;
	
	
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Input")
	TObjectPtr<UInputDataAsset> InputData;

#pragma region InputResponse
	void DoMove(const FInputActionValue& InputActionValue);
	void DoLook(const FInputActionValue& InputActionValue);
	void DoRaycast();
	void DoMouseClick();

#pragma endregion
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="LookPara|TurnSpeed")
	float RotateSpeed = 90.f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="LookPara|MinYaw")
	float MinYaw = 45.f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="LookPara|MaxYaw")
	float MaxYaw = 135.f;
};
