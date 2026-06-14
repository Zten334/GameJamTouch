// Fill out your copyright notice in the Description page of Project Settings.


#include "GameJam/Public/CharacterBase.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameJam/Public/Components/RayCastComponent.h"
#include "GameJam/Public/DataAsset/InputDataAsset.h"
#include "Actors/IteractableItem.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
    //暂时放开，方便蓝图后续使用
	PrimaryActorTick.bCanEverTick = true;
	
	GetMesh()->bReceivesDecals = false;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement() -> bOrientRotationToMovement = false;
	GetCharacterMovement() -> RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement() -> JumpZVelocity = 500.f;
	GetCharacterMovement() -> AirControl = 0.35f;
	GetCharacterMovement() -> MaxWalkSpeed = 500.f;
	GetCharacterMovement() -> MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement() -> BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement() -> BrakingDecelerationFalling = 1500.f;
	
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom -> SetupAttachment(GetRootComponent());
	CameraBoom -> TargetArmLength = 600.0f;
	CameraBoom -> bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;	
	
	//创建射线检测组件
	RayCastComponent = CreateDefaultSubobject<URayCastComponent>("RayCastComponent");
	RayCastComponent -> SetupAttachment(GetRootComponent());
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// ── 防护 ──
	if (!InputData) { return; }

	// ── 1. 获取 EnhancedInputComponent ──
	UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	// ── 2. 将 MappingContext 添加到 EnhancedInputLocalPlayerSubsystem ──
	if (const APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (InputData->InputMappingContext)
			{
				// priority 0 = 默认优先级
				Subsystem->AddMappingContext(InputData->InputMappingContext, 0);
			}
		}
	}
	// ── 3. 绑定 InputAction 到回调函数 ──
	if (InputData->MoveAction)
	{
		// ETriggerEvent::Triggered 每帧触发（用于持续输入如摇杆/WASD）
		EnhancedInput->BindAction(InputData->MoveAction, ETriggerEvent::Triggered,this, &ACharacterBase::DoMove);
	}

	if (InputData->RotateAction)
	{
		EnhancedInput->BindAction(InputData->RotateAction, ETriggerEvent::Triggered,this, &ACharacterBase::DoLook);
	}	
	
	if (InputData->RaycastAction)
	{
		EnhancedInput->BindAction(InputData->RaycastAction, ETriggerEvent::Triggered, this, &ACharacterBase::DoRaycast);
	}

	if (InputData->MouseClickAction)
	{
		EnhancedInput->BindAction(InputData->MouseClickAction, ETriggerEvent::Started, this, &ACharacterBase::DoMouseClick);
	}
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;
	//开始时开放鼠标光标
	PC->SetShowMouseCursor(true);
}

void ACharacterBase::DoMove(const FInputActionValue& InputActionValue)
{
	const FVector2D Movement = InputActionValue.Get<FVector2D>();

	if (!Controller || Movement.IsNearlyZero())
	{
		return;
	}

	const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);

	if (Movement.Y != 0.f)
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardDirection, Movement.Y);
	}
	if (Movement.X != 0.f)
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(RightDirection, Movement.X);
	}
}

void ACharacterBase::DoLook(const FInputActionValue& InputActionValue)
{
	const float InputValue = InputActionValue.Get<float>();
	if (!Controller || InputValue == 0.f)
	{
		return;
	}
	AddControllerYawInput(InputValue * RotateSpeed * GetWorld()->GetDeltaSeconds() );
}



void ACharacterBase::DoRaycast()
{
	AActor* HitActor = RayCastComponent->PerformTrace(
		RayCastComponent->GetComponentLocation(),
		RayCastComponent->GetForwardVector()
	);

	if (AIteractableItem* Item = Cast<AIteractableItem>(HitActor))
	{
		Item->HandleTouch(this);
	}
}

void ACharacterBase::DoMouseClick()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	float MouseX, MouseY;
	if (!PC->GetMousePosition(MouseX, MouseY)) return;

	FVector WorldLocation, WorldDirection;
	if (!PC->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection)) return;

	AActor* HitActor = RayCastComponent->PerformTrace(WorldLocation, WorldDirection);

	if (AIteractableItem* Item = Cast<AIteractableItem>(HitActor))
	{
		Item->HandleTouch(this);
	}
}




