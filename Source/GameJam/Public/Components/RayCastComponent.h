// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RayCastComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEJAM_API URayCastComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	URayCastComponent();

	// 执行射线检测，返回命中 Actor 的名字。未命中返回空字符串。
	UFUNCTION(BlueprintCallable, Category = "RayCast")
	FString PerformTrace();

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	// ── 可编辑属性 ──

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RayCast")
	float TraceLength = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RayCast")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

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
