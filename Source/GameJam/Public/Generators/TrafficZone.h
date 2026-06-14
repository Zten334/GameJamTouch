// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Generators/SpwanZone.h"
#include "TrafficZone.generated.h"

class USoundBase;
class ACharacterBase;
class USceneComponent;

UENUM(BlueprintType)
enum class ETrafficLight : uint8
{
	Green,
	Red
};

UCLASS()
class GAMEJAM_API ATrafficZone : public ASpwanZone
{
	GENERATED_BODY()

public:
	ATrafficZone();

protected:
	virtual void BeginPlay() override;
	virtual void SpawnItems() override;
	virtual void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	// ── 红绿灯 ──

	UPROPERTY(EditDefaultsOnly, Category = "Traffic")
	float GreenDuration = 6.f;

	UPROPERTY(EditDefaultsOnly, Category = "Traffic")
	float RedDuration = 6.f;

	/** 绿灯结束前几秒发警告 */
	UPROPERTY(EditDefaultsOnly, Category = "Traffic")
	float WarningAhead = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Traffic|Audio")
	TObjectPtr<USoundBase> WarningSound;

	ETrafficLight CurrentLight;

	void SwitchToGreen();
	void SwitchToRed();
	void OnWarningCheck();

	// ── 车辆控制 ──

	/** 已生成车辆 → 生成原点 */
	TMap<AActor*, FVector> VehicleOrigins;

	/** 绿灯：车按各自前方发射 */
	void LaunchVehicles();

	/** 红灯：停车 + 传送回原点 */
	void RecallVehicles();

	// ── 车辆撞到玩家 → 传送 ──

	UFUNCTION()
	void OnVehicleHitPlayer(AActor* HitPlayer);

	/** 被撞后传送回这里 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traffic|Components")
	TObjectPtr<USceneComponent> RestartPoint;

	// ── 玩家进入/离开区域 ──

	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool bPlayerInZone = false;

	// ── 定时器 ──

	FTimerHandle CycleTimer;
	FTimerHandle WarningTimer;
};
