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
	// ── 触发器：玩家踩入范围 ──
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn|Components")
	TObjectPtr<UBoxComponent> TriggerComp;

	// ── 标记点容器：在蓝图里往它下面加 Arrow/SceneComponent 即可 ──
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn|Components")
	TObjectPtr<USceneComponent> MarkersRoot;

	/** BeginPlay 时自动收集的标记点（只读） */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	TArray<TObjectPtr<USceneComponent>> CachedMarkers;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SpawnItems();

	/** 生成什么 Actor */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TSubclassOf<AActor> ItemClass;

	/** 是否只生成一次 */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	bool bSpawnOnce = true;

private:
	bool bHasSpawned = false;
};
