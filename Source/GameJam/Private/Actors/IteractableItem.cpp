// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/IteractableItem.h"

#include "CharacterBase.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

AIteractableItem::AIteractableItem()
{
	PrimaryActorTick.bCanEverTick = false;

	// ── BlockComp：根组件，负责显示 Mesh + 阻挡 Pawn，不绑定任何事件 ──
	BlockComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockComp"));
	SetRootComponent(BlockComp);
	BlockComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BlockComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BlockComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	InteracteType = EInteracteType::BANANA;
}

void AIteractableItem::HandleInteraction(AActor* Interactor)
{
	if (!Interactor)
	{
		return;
	}

	ACharacterBase* Player = Cast<ACharacterBase>(Interactor);
	if (!Player)
	{
		return;
	}

	switch (InteracteType)
	{
	case EInteracteType::BANANA:
		UE_LOG(LogTemp, Warning, TEXT("踩到香蕉皮！滑倒！"));
		break;

	case EInteracteType::BECYLE:
		UE_LOG(LogTemp, Warning, TEXT("撞到自行车！"));
		break;

	case EInteracteType::UTILITY_POLE:
		UE_LOG(LogTemp, Warning, TEXT("撞到电线杆！"));
		break;

	case EInteracteType::PUDDLE:
		UE_LOG(LogTemp, Warning, TEXT("踩到水坑！溅起水花！"));
		break;

	case EInteracteType::RUNNING_MAN:
		UE_LOG(LogTemp, Warning, TEXT("WHAT CAN I SAY？MAN！"));
		break;

	case EInteracteType::VEHICLE:
		UE_LOG(LogTemp, Warning, TEXT("被车撞了！"));
		break;

	default:
		break;
	}
}
