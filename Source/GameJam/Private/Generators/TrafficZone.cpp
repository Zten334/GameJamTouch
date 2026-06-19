// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/TrafficZone.h"

#include "CharacterBase.h"
#include "Actors/InteractableItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Components/PlayerUIComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "State/State.h"

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

	// 开局就生成车辆
	SpawnItems();
	bHasSpawned = true;

	// 从绿灯开始
	SwitchToGreen();
}

// ── 玩家进入：绿灯只记录，红灯直接弹走 ──
void ATrafficZone::OnZoneBeginOverlap(UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	ACharacterBase* Player = Cast<ACharacterBase>(OtherActor);
	if (!Player) return;

	if (CurrentLight == ETrafficLight::Red)
	{
		PunishAndTeleport(Player);
		return;
	}

	bPlayerInZone = true;
}

void ATrafficZone::OnTriggerEndOverlap(UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32)
{
	if (Cast<ACharacterBase>(OtherActor))
	{
		bPlayerInZone = false;
	}
}

// ── 红灯踩入：音效 + 晃镜 + 传送 ──
void ATrafficZone::PunishAndTeleport(ACharacterBase* Player)
{
	if (!Player || !RestartPoint) return;

	if (PunishSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PunishSound, Player->GetActorLocation());
	}

	if (PunishShakeClass)
	{
		if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
		{
			PC->ClientStartCameraShake(PunishShakeClass);
		}
	}
	//出现提示
	if (Player->PlayerUIComponent)
	{
		Player->PlayerUIComponent->ShowMessage(FText::FromString(TEXT("不要闯红灯！")),2.f);
	}
	//扣血
	AState* State = Cast<AState>(Player->GetPlayerState());
	if (State)
	{
		State->Health -= Damage;
	}
	
	Player->SetActorLocation(RestartPoint->GetComponentLocation());
}

// ── 生成车辆，不做额外绑定 ──
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
	}
}

// ── 红绿灯切换 ──
void ATrafficZone::SwitchToGreen()
{
	CurrentLight = ETrafficLight::Green;

	// 停掉红灯轮询
	GetWorldTimerManager().ClearTimer(RedCheckTimer);

	// 绿灯 = 安全，车子停
	RecallVehicles();

	if (GreenLightSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GreenLightSound, GetActorLocation());
	}

	GetWorldTimerManager().SetTimer(CycleTimer, this, &ATrafficZone::SwitchToRed, GreenDuration, false);

	float WarnTime = GreenDuration - WarningAhead;
	if (WarnTime > 0.f)
	{
		GetWorldTimerManager().SetTimer(WarningTimer, this, &ATrafficZone::OnWarningCheck, WarnTime, false);
	}
}

void ATrafficZone::SwitchToRed()
{
	CurrentLight = ETrafficLight::Red;

	// 红灯 = 危险，车子出动
	LaunchVehicles();

	if (RedLightSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), RedLightSound, GetActorLocation());
	}

	// 先清理一次当前在区域内的玩家
	if (bPlayerInZone)
	{
		if (ACharacterBase* Player = Cast<ACharacterBase>(
			GetWorld()->GetFirstPlayerController()->GetPawn()))
		{
			PunishAndTeleport(Player);
		}
		bPlayerInZone = false;
	}

	// 启动红灯轮询：每 0.2s 检查有没有玩家踩进来
	GetWorldTimerManager().SetTimer(RedCheckTimer, this,
		&ATrafficZone::RedTickCheck, 0.2f, true);

	GetWorldTimerManager().SetTimer(CycleTimer, this, &ATrafficZone::SwitchToGreen, RedDuration, false);
}

void ATrafficZone::RedTickCheck()
{
	if (CurrentLight != ETrafficLight::Red) return;

	TArray<AActor*> Overlapping;
	TriggerComp->GetOverlappingActors(Overlapping, ACharacterBase::StaticClass());

	for (AActor* Actor : Overlapping)
	{
		if (ACharacterBase* Player = Cast<ACharacterBase>(Actor))
		{
			PunishAndTeleport(Player);
		}
	}
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

		if (AInteractableItem* Item = Cast<AInteractableItem>(Vehicle))
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

		if (AInteractableItem* Item = Cast<AInteractableItem>(Vehicle))
		{
			Item->MovementComp->Velocity = FVector::ZeroVector;
			Item->MovementComp->Deactivate();
		}

		Vehicle->SetActorLocation(Origin);
	}
}
