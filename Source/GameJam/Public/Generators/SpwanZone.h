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

	/** 追踪所有已生成的 Actor，子类可直接用 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	TArray<TObjectPtr<AActor>> SpawnedActors;

protected:
	virtual void BeginPlay() override;

	// ── 组件 ──
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn|Components")
	TObjectPtr<UBoxComponent> TriggerComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn|Components")
	TObjectPtr<USceneComponent> MarkersRoot;

	/** BeginPlay 时自动收集的标记点 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	TArray<TObjectPtr<USceneComponent>> CachedMarkers;

	UFUNCTION()
	virtual void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** 生成 Actor，子类可 override */
	virtual void SpawnItems();

	/** 生成什么 Actor */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TSubclassOf<AActor> ItemClass;

	/** 是否只生成一次 */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	bool bSpawnOnce = true;

	bool bHasSpawned = false;
};
