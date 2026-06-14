// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IteractableItem.generated.h"

class ACharacterBase;
class UStaticMeshComponent;
class UCameraShakeBase;
class USoundBase;
class UProjectileMovementComponent;

UENUM(BlueprintType)
enum class ECollisionType : uint8
{
	NONE,            //什么也不是的类型，不会触发CollitionType
	BANANA,          // 香蕉
	BECYLE,          // 自行车
	UTILITY_POLE,    // 电线杆
	PUDDLE,          // 水坑
	RUNNING_MAN,     // MAN！
	VEHICLE,         // 载具
};

UENUM(BlueprintType)
enum class EPlayerTouchType : uint8
{
	NORMAL,                // 一般物体
	KEY,                   // 钥匙
	WALLET,               // 钱包
	WHITE_CANE,           // 盲杖

};

UCLASS()
class GAMEJAM_API AIteractableItem : public AActor
{
	GENERATED_BODY()

public:
	AIteractableItem();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleCollisionInteraction(AActor* Interactor);

	void HandleTouch(ACharacterBase* Character);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IteracteType")
	ECollisionType CollisionType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TouchType")
	EPlayerTouchType TouchType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> CollisionSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> TouchSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> HitShakeClass;

	// ── 显示 + 物理阻挡 ──
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BlockComp;

	// ── 移动：Item 自己控制飞行 ──
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> MovementComp;

	/** 初始移动速度，BeginPlay 时按自身前方推一下 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float InitialSpeed = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float CollisionDemage = 0.f;
};
