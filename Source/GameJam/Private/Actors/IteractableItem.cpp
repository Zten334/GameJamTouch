// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/IteractableItem.h"

#include "CharacterBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PlayerUIComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "State/State.h"

AIteractableItem::AIteractableItem()
{
	PrimaryActorTick.bCanEverTick = false;

	// ── 根：显示 + 阻挡，不模拟物理 ──
	BlockComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockComp"));
	SetRootComponent(BlockComp);
	BlockComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BlockComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BlockComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	// ── 移动：Item 自己控制飞行 ──
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->bAutoActivate = false;
	MovementComp->InitialSpeed = 0.f;
	MovementComp->MaxSpeed = 3000.f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bShouldBounce = false;
	MovementComp->ProjectileGravityScale = 0.f;

	CollisionType = ECollisionType::NONE;
}

void AIteractableItem::BeginPlay()
{
	Super::BeginPlay();

	if (InitialSpeed > 0.f)
	{
		MovementComp->Velocity = GetActorForwardVector() * InitialSpeed;
		MovementComp->Activate();
	}
}

void AIteractableItem::HandleCollisionInteraction(AActor* Interactor)
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
	AState* PlayerState = Cast<AState>(Player->GetPlayerState());
	if (!PlayerState)
	{
		return;
	}

	FText CollisionMsg;

	switch (CollisionType)
	{
	case ECollisionType::BANANA:
		PlayerState->Health -= CollisionDemage;
		CollisionMsg = FText::FromString(TEXT("踩到香蕉皮！滑倒！"));
		break;

	case ECollisionType::BECYLE:
		PlayerState->Health -= CollisionDemage;
		CollisionMsg = FText::FromString(TEXT("撞到自行车！"));
		break;

	case ECollisionType::UTILITY_POLE:
		PlayerState->Health -= CollisionDemage;
		CollisionMsg = FText::FromString(TEXT("撞到电线杆！"));
		break;

	case ECollisionType::PUDDLE:
		PlayerState->Health -= CollisionDemage;
		CollisionMsg = FText::FromString(TEXT("踩到水坑！溅起水花！"));
		break;

	case ECollisionType::RUNNING_MAN:
		PlayerState->Health -= CollisionDemage;
		CollisionMsg = FText::FromString(TEXT("WHAT CAN I SAY？MAN！"));
		break;

	case ECollisionType::VEHICLE:
		PlayerState->Health -= CollisionDemage;
		CollisionMsg = FText::FromString(TEXT("被车撞了！"));
		break;

	default:
		break;
	}

	if (Player->PlayerUIComponent)
	{
		Player->PlayerUIComponent->ShowMessage(CollisionMsg, 2.f);
	}

	if (CollisionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CollisionSound, GetActorLocation());
	}

	if (HitShakeClass)
	{
		if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
		{
			PC->ClientStartCameraShake(HitShakeClass);
		}
	}

	OnPlayerCollision.Broadcast(Interactor);
}

void AIteractableItem::HandleTouch(ACharacterBase* Character)
{
	if (!Character)
	{
		return;
	}

	if (TouchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), TouchSound, GetActorLocation());
	}

	AState* PlayerState = Cast<AState>(Character->GetPlayerState());
	if (!PlayerState)
	{
		return;
	}

	FText TouchMsg;

	switch (TouchType)
	{
	case EPlayerTouchType::KEY:
		PlayerState->Has_Key = PlayerState->Has_Key ? PlayerState->Has_Key : true;
		TouchMsg = FText::FromString(TEXT("获得钥匙！"));
		break;

	case EPlayerTouchType::WALLET:
		PlayerState->Has_Wallet = PlayerState->Has_Wallet ? PlayerState->Has_Wallet : true;
		TouchMsg = FText::FromString(TEXT("获得钱包！"));
		break;

	case EPlayerTouchType::WHITE_CANE:
		PlayerState->White_Cane = PlayerState->White_Cane ? PlayerState->White_Cane : true;
		TouchMsg = FText::FromString(TEXT("获得盲杖！"));
		break;

	case EPlayerTouchType::NORMAL:
	default:
		TouchMsg = FText::FromString(TEXT("该物品无特殊效果"));
		break;
	}

	if (Character->PlayerUIComponent)
	{
		Character->PlayerUIComponent->ShowMessage(TouchMsg, 2.f);
	}
}
