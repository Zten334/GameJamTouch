// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpwanZone.generated.h"

class UBoxComponent;

UCLASS()
class GAMEJAM_API ASpwanZone : public AActor
{
	GENERATED_BODY()

public:
	ASpwanZone();

protected:
	virtual void BeginPlay() override;

	// ── 触发器 ──
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn|Components")
	TObjectPtr<UBoxComponent> TriggerComp;

	UFUNCTION()
	void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// ── 生成 ──
	void SpawnItems();

	/** 生成什么 Actor */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TSubclassOf<AActor> ItemClass;

	/** 标记点：位置 = 生成点, 朝向 = 物体前方（ProjectileMovement 发方向） */
	UPROPERTY(VisibleAnywhere, Category = "Spawn")
	TArray<TObjectPtr<USceneComponent>> SpawnMarkers;

	/** 标记点数量（构造里批量创建） */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	int32 MarkerCount = 3;

	/** 是否只生成一次 */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	bool bSpawnOnce = true;

private:
	bool bHasSpawned = false;
};
