// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IteractableItem.generated.h"

class ACharacterBase;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UCameraShakeBase;

UENUM(BlueprintType)
enum class ECollisionType : uint8
{
	BANANA,          // 香蕉
	BECYLE,          // 自行车
	UTILITY_POLE,    // 电线杆
	PUDDLE,          // 水坑
	RUNNING_MAN,     // MAN！
	VEHICLE,         // 载具
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> HitShakeClass;

	// ── 显示 + 物理阻挡 ──
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BlockComp;

	// ── 移动组件：Item 自己控制运动 ──
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> MovementComp;

	/** 初始移动速度，BeginPlay 时自动按自身前方发射 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float InitialSpeed = 500.f;
};
