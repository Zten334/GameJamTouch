// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/IteractableItem.h"

#include "CharacterBase.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "State/State.h"

AIteractableItem::AIteractableItem()
{
	PrimaryActorTick.bCanEverTick = false;

	// ── 根：物理阻挡 ──
	BlockComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockComp"));
	SetRootComponent(BlockComp);
	BlockComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BlockComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BlockComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	// ── 移动组件：Item 自己管自己飞 ──
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->bAutoActivate = false;          // 不由组件自己激活，BeginPlay 里手动控制
	MovementComp->InitialSpeed = 0.f;             // 不靠组件自带的 InitialSpeed
	MovementComp->MaxSpeed = 3000.f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bShouldBounce = false;
	MovementComp->ProjectileGravityScale = 0.5f;  // 有一点重力，落地比较自然

	CollisionType = ECollisionType::BANANA;
}

void AIteractableItem::BeginPlay()
{
	Super::BeginPlay();
	// 仅针对部分类型
	// 如果有初始速度，激活移动组件
	if (InitialSpeed > 0.f)
	{
		MovementComp->Velocity = GetActorForwardVector() * InitialSpeed;
		MovementComp->Activate();
	}

	// 如果速度 > 0 并且有 Mesh，开物理模拟也行
	// 这里统一用 ProjectileMovement 管运动，BlockComp 不开 SimulatePhysics
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
	switch (CollisionType)
	{
	case ECollisionType::BANANA:
		PlayerState->Health -= 10.f;
		UE_LOG(LogTemp, Warning, TEXT("踩到香蕉皮！滑倒！，扣除生命值，剩余生命值是%f"),PlayerState->Health);
		break;

	case ECollisionType::BECYLE:
		UE_LOG(LogTemp, Warning, TEXT("撞到自行车！"));
		break;

	case ECollisionType::UTILITY_POLE:
		UE_LOG(LogTemp, Warning, TEXT("撞到电线杆！"));
		break;

	case ECollisionType::PUDDLE:
		UE_LOG(LogTemp, Warning, TEXT("踩到水坑！溅起水花！"));
		break;

	case ECollisionType::RUNNING_MAN:
		UE_LOG(LogTemp, Warning, TEXT("WHAT CAN I SAY？MAN！"));
		break;

	case ECollisionType::VEHICLE:
		UE_LOG(LogTemp, Warning, TEXT("被车撞了！"));
		break;

	default:
		break;
	}

	if (HitShakeClass)
	{
		if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
		{
			PC->ClientStartCameraShake(HitShakeClass);
		}
	}
}

void AIteractableItem::HandleTouch(ACharacterBase* Character)
{
	UE_LOG(LogTemp, Warning, TEXT("已被选中！"));
	if (!Character)
	{
		return;
	}
	AState* PlayerState = Cast<AState>(Character->GetPlayerState());
	if (!PlayerState)
	{
		return;
	}
	switch (CollisionType)
	{
	case ECollisionType::BANANA:
		PlayerState->Health -= 10.f;
		UE_LOG(LogTemp, Warning, TEXT("踩到香蕉皮！滑倒！，扣除生命值，剩余生命值是%f"),PlayerState->Health);
		break;

	case ECollisionType::BECYLE:
		UE_LOG(LogTemp, Warning, TEXT("撞到自行车！"));
		break;

	case ECollisionType::UTILITY_POLE:
		UE_LOG(LogTemp, Warning, TEXT("撞到电线杆！"));
		break;

	case ECollisionType::PUDDLE:
		UE_LOG(LogTemp, Warning, TEXT("踩到水坑！溅起水花！"));
		break;

	case ECollisionType::RUNNING_MAN:
		UE_LOG(LogTemp, Warning, TEXT("WHAT CAN I SAY？MAN！"));
		break;

	case ECollisionType::VEHICLE:
		UE_LOG(LogTemp, Warning, TEXT("被车撞了！"));
		break;

	default:
		break;
	}
}
