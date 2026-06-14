// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/SpwanZone.h"

#include "CharacterBase.h"
#include "Actors/IteractableItem.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"

ASpwanZone::ASpwanZone()
{
	PrimaryActorTick.bCanEverTick = false;

	// ── 触发器 ──
	TriggerComp = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComp"));
	SetRootComponent(TriggerComp);
	TriggerComp->SetBoxExtent(FVector(300.f, 300.f, 150.f));
	TriggerComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerComp->SetGenerateOverlapEvents(true);
	TriggerComp->OnComponentBeginOverlap.AddDynamic(this, &ASpwanZone::OnZoneBeginOverlap);

	// ── 标记点容器：蓝图里手动往它下面加 Arrow Component ──
	MarkersRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MarkersRoot"));
	MarkersRoot->SetupAttachment(TriggerComp);

#if WITH_EDITOR
	TriggerComp->ShapeColor = FColor::Green;
#endif
}

void ASpwanZone::BeginPlay()
{
	Super::BeginPlay();

	// 收集 MarkersRoot 下所有子组件，缓存起来（先用原始指针接收，再转存）
	TArray<USceneComponent*> RawMarkers;
	MarkersRoot->GetChildrenComponents(false, RawMarkers);
	CachedMarkers = TArray<TObjectPtr<USceneComponent>>(RawMarkers);
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
	}
}
