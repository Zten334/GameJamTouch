// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerUIComponent.generated.h"

USTRUCT(BlueprintType)
struct FFloatingMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FText Text;

	float RemainingTime = 0.f;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEJAM_API UPlayerUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerUIComponent();

	/** 显示一条浮动消息，持续 Duration 秒 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMessage(FText Message, float Duration = 2.f);

	/** 更新计时器 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateTimer(float Seconds);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	/** Tick 里从 PlayerState 自动同步血条 */
	void SyncHealthFromPlayerState();

public:
	// ── UMG 直接 Bind 这些属性 ──

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	FText CurrentMessage;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	bool bHasMessage = false;

	/** 血条 0~1，Tick 自动从 PlayerState 同步 */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	float HealthPercent = 1.f;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	FText TimerText;

private:
	TArray<FFloatingMessage> MessageQueue;
};
