// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

class URayCastComponent;
class UPlayerUIComponent;
struct FInputActionValue;
class UInputDataAsset;
class USpringArmComponent;
class UCameraComponent;
class UBlindEchoRevealComponent;
class UUserWidget;

UCLASS()
class GAMEJAM_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();
	//暴露给接触到的物品使用
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerUIComponent> PlayerUIComponent;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "SpringArm", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Raycast", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URayCastComponent> RayCastComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlindEcho", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBlindEchoRevealComponent> BlindEchoRevealComponent;

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Input")
	TObjectPtr<UInputDataAsset> InputData;

	/** HUD Widget 类，蓝图里选 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

#pragma region InputResponse
	void DoMove(const FInputActionValue& InputActionValue);
	void DoLook(const FInputActionValue& InputActionValue);
	void DoRaycast();
	UFUNCTION()
	void HandleRayCastHit(const FHitResult& HitResult);
	void DoMouseClick();

#pragma endregion

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="LookPara|TurnSpeed")
	float RotateSpeed = 90.f;

};
