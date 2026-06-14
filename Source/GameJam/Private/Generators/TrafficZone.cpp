// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/TrafficZone.h"

#include "CharacterBase.h"
#include "Actors/IteractableItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ATrafficZone::ATrafficZone()
{
	PrimaryActorTick.bCanEverTick = false;

	RestartPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RestartPoint"));
	RestartPoint->SetupAttachment(TriggerComp);
}

void ATrafficZone::BeginPlay()
{
	Super::BeginPlay();

	// 绑定玩家离开区域
	TriggerComp->OnComponentEndOverlap.AddDynamic(this, &ATrafficZone::OnTriggerEndOverlap);

	// 开局就生成车辆（不受 bSpawnOnce 限制）
	SpawnItems();
	bHasSpawned = true;

	// 从绿灯开始
	SwitchToGreen();
}

// ── 覆盖：不靠玩家踩入触发生成，只记录进出 ──
void ATrafficZone::OnZoneBeginOverlap(UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (Cast<ACharacterBase>(OtherActor))
	{
		bPlayerInZone = true;
	}
}

void ATrafficZone::OnTriggerEndOverlap(UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32)
{
	if (Cast<ACharacterBase>(OtherActor))
	{
		bPlayerInZone = false;
	}
}

// ── 生成 + 绑车辆撞人事件 ──
void ATrafficZone::SpawnItems()
{
	if (!ItemClass || !GetWorld()) return;

	for (USceneComponent* Marker : CachedMarkers)
	{
		if (!Marker) continue;

		const FVector SpawnLocation = Marker->GetComponentLocation();
		const FRotator SpawnRotation = Marker->GetComponentRotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
			ItemClass, SpawnLocation, SpawnRotation, Params);

		SpawnedActors.Add(SpawnedActor);
		VehicleOrigins.Add(SpawnedActor, SpawnLocation);

		// 绑定 Item 的碰撞委托，TrafficZone 监听撞人事件
		if (AIteractableItem* Item = Cast<AIteractableItem>(SpawnedActor))
		{
			Item->OnPlayerCollision.AddDynamic(this, &ATrafficZone::OnVehicleHitPlayer);
		}
	}
}

// ── 车辆撞到玩家 → 传送 ──
void ATrafficZone::OnVehicleHitPlayer(AActor* HitPlayer)
{
	ACharacterBase* Player = Cast<ACharacterBase>(HitPlayer);
	if (!Player || !RestartPoint) return;

	Player->SetActorLocation(RestartPoint->GetComponentLocation());
}

// ── 红绿灯切换 ──
void ATrafficZone::SwitchToGreen()
{
	CurrentLight = ETrafficLight::Green;
	LaunchVehicles();

	GetWorldTimerManager().SetTimer(CycleTimer, this, &ATrafficZone::SwitchToRed, GreenDuration, false);

	// 绿灯快结束时检查
	float WarnTime = GreenDuration - WarningAhead;
	if (WarnTime > 0.f)
	{
		GetWorldTimerManager().SetTimer(WarningTimer, this, &ATrafficZone::OnWarningCheck, WarnTime, false);
	}
}

void ATrafficZone::SwitchToRed()
{
	CurrentLight = ETrafficLight::Red;
	RecallVehicles();

	GetWorldTimerManager().SetTimer(CycleTimer, this, &ATrafficZone::SwitchToGreen, RedDuration, false);
}

void ATrafficZone::OnWarningCheck()
{
	if (bPlayerInZone && CurrentLight == ETrafficLight::Green)
	{
		if (WarningSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), WarningSound, GetActorLocation());
		}
	}
}

// ── 车辆发射 / 回收 ──
void ATrafficZone::LaunchVehicles()
{
	for (const auto& Pair : VehicleOrigins)
	{
		AActor* Vehicle = Pair.Key;
		if (!Vehicle) continue;

		if (AIteractableItem* Item = Cast<AIteractableItem>(Vehicle))
		{
			if (Item->InitialSpeed > 0.f)
			{
				Item->MovementComp->Velocity = Vehicle->GetActorForwardVector() * Item->InitialSpeed;
				Item->MovementComp->Activate();
			}
		}
	}
}

void ATrafficZone::RecallVehicles()
{
	for (const auto& Pair : VehicleOrigins)
	{
		AActor* Vehicle = Pair.Key;
		const FVector& Origin = Pair.Value;
		if (!Vehicle) continue;

		// 停止移动
		if (AIteractableItem* Item = Cast<AIteractableItem>(Vehicle))
		{
			Item->MovementComp->Velocity = FVector::ZeroVector;
			Item->MovementComp->Deactivate();
		}

		// 传送回原点
		Vehicle->SetActorLocation(Origin);
	}
}
