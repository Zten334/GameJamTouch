// Fill out your copyright notice in the Description page of Project Settings.

#include "GameJam/Public/Components/RayCastComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

URayCastComponent::URayCastComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 创建可视化箭头，编辑器里能看到射线方向和长度
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->ArrowColor = FColor::Cyan;
	ArrowComponent->ArrowSize = 0.5f;
	ArrowComponent->bIsScreenSizeScaled = true;
	ArrowComponent->bHiddenInGame = true;
	
	ArrowComponent->SetupAttachment(this);
	
}

void URayCastComponent::BeginPlay()
{
	Super::BeginPlay();
	UpdateArrow();
}

void URayCastComponent::UpdateArrow()
{
	if (ArrowComponent)
	{
		ArrowComponent->ArrowLength = TraceLength;
		ArrowComponent->SetRelativeLocation(FVector::ZeroVector);
		ArrowComponent->SetRelativeRotation(FRotator::ZeroRotator);
	}
}

#if WITH_EDITOR
void URayCastComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(URayCastComponent, TraceLength))
	{
		UpdateArrow();
	}
}
#endif

FString URayCastComponent::PerformTrace()
{
	UE_LOG(LogTemp, Warning, TEXT("射线检测"));
	
	if (!GetWorld())
	{
		return TEXT("");
	}

	const FVector Start = GetComponentLocation();
	const FVector End = Start + GetForwardVector() * TraceLength;
	
	UE_LOG(LogTemp, Warning, TEXT("%s"),*Start.ToString());
	UE_LOG(LogTemp, Warning, TEXT("%s"),*End.ToString());

	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bTraceComplex = false;

	FHitResult Hit;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		TraceChannel,
		QueryParams
	);

	if (bDrawDebug)
	{
		DrawDebugLine(GetWorld(), Start, End, DebugColor, false, 5.f, 0, 5.0f);

		if (bHit)
		{
			DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.f, FColor::Green, false, 0.1f);
		}
	}

	return bHit ? Hit.GetActor()->GetName() : TEXT("");
}
