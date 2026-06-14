// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IteractableItem.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EInteracteType : uint8
{
	BANANA,        //
	BECYLE,          //
	UTILITY_POLE,   //电线杆
	PUDDLE,         //
	RUNNING_MAN,    //MAN！
	VEHICLE,
};

UCLASS()
class GAMEJAM_API AIteractableItem : public AActor
{
	GENERATED_BODY()

public:
	AIteractableItem();

	/** 蓝图里碰撞事件触发后调这个，传入碰撞的 Actor */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleInteraction(AActor* Interactor);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IteracteType")
	EInteracteType InteracteType;

	// ── 只负责显示 Mesh 和物理阻挡，不绑定事件 ──
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BlockComp;
};
