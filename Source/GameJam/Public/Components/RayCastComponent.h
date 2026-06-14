// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RayCastComponent.generated.h"

// 命中结果广播委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRayCastHit, const FHitResult&, HitResult);


//直接把检测完之后的逻辑也写在这里了
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEJAM_API URayCastComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	URayCastComponent();

	// 从 Start 沿 Direction 发射射线，返回命中的 Actor。未命中返回 nullptr。
	UFUNCTION(BlueprintCallable, Category = "RayCast")
	AActor* PerformTrace(FVector Start, FVector Direction);

	// 命中时广播，外部组件（如 BlindEchoRevealComponent）订阅即可解耦
	UPROPERTY(BlueprintAssignable, Category = "RayCast")
	FOnRayCastHit OnRayCastHit;

protected:
	virtual void BeginPlay() override;
	virtual void OnRegister() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	// ── 可编辑属性 ──

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RayCast")
	float TraceLength = 500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RayCast")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RayCast|Debug")
	bool bDrawDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RayCast|Debug")
	FColor DebugColor = FColor::Red;

private:
	// 可视化箭头
	UPROPERTY()
	TObjectPtr<class UArrowComponent> ArrowComponent;

	void UpdateArrow();
};
