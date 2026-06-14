// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/SpwanZone.h"

#include "CharacterBase.h"
#include "Actors/IteractableItem.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"

ASpwanZone::ASpwanZone()
{
	PrimaryActorTick.bCanEverTick = false;

	// ── 触发器：玩家踩入范围 ──
	TriggerComp = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComp"));
	SetRootComponent(TriggerComp);
	TriggerComp->SetBoxExtent(FVector(300.f, 300.f, 150.f));
	TriggerComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerComp->OnComponentBeginOverlap.AddDynamic(this, &ASpwanZone::OnZoneBeginOverlap);

#if WITH_EDITOR
	TriggerComp->ShapeColor = FColor::Green;
#endif

	// ── 创建标记点（编辑器里可拖拽箭头）──
	for (int32 i = 0; i < MarkerCount; ++i)
	{
		USceneComponent* Marker = CreateDefaultSubobject<USceneComponent>(
			*FString::Printf(TEXT("SpawnMarker_%d"), i));
		Marker->SetupAttachment(TriggerComp);
		Marker->SetRelativeLocation(FVector(i * 200.f, 0.f, 100.f));
		Marker->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
		SpawnMarkers.Add(Marker);
	}
}

void ASpwanZone::BeginPlay()
{
	Super::BeginPlay();
}

void ASpwanZone::OnZoneBeginOverlap(UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (bSpawnOnce && bHasSpawned) return;

	ACharacterBase* Player = Cast<ACharacterBase>(OtherActor);
	if (!Player) return;

	bHasSpawned = true;
	SpawnItems();
}

void ASpwanZone::SpawnItems()
{
	if (!ItemClass || !GetWorld()) return;

	for (USceneComponent* Marker : SpawnMarkers)
	{
		if (!Marker) continue;

		const FVector SpawnLocation = Marker->GetComponentLocation();
		const FRotator SpawnRotation = Marker->GetComponentRotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
			ItemClass, SpawnLocation, SpawnRotation, Params);

		// Item 的 BeginPlay 里自己的 MovementComp 按 InitialSpeed 发射，
		// 方向 = 生成时的朝向（Marker 的 ForwardVector）
	}
}
