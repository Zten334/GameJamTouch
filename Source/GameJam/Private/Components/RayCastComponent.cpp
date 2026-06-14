// Fill out your copyright notice in the Description page of Project Settings.

#include "GameJam/Public/Components/RayCastComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

URayCastComponent::URayCastComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URayCastComponent::OnRegister()
{
	Super::OnRegister();

#ifdef WITH_EDITOR
	if (!ArrowComponent)
	{
		ArrowComponent = NewObject<UArrowComponent>(this, TEXT("Arrow"));
		ArrowComponent->ArrowColor = FColor::Cyan;
		ArrowComponent->ArrowSize = 0.5f;
		ArrowComponent->bIsScreenSizeScaled = true;
		ArrowComponent->bHiddenInGame = true;
		ArrowComponent->SetupAttachment(this);
		ArrowComponent->RegisterComponent();
	}
	UpdateArrow();
#endif
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

FString URayCastComponent::PerformTrace(FVector Start, FVector Direction)
{
	if (!GetWorld())
	{
		return TEXT("");
	}

	Direction.Normalize();
	const FVector End = Start + Direction * TraceLength;

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
			UE_LOG(LogTemp, Warning, TEXT("%s hit!"), *Hit.GetActor()->GetName());

			DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.f, FColor::Green, false, 5.f);
		}
	}

	return bHit ? Hit.GetActor()->GetName() : TEXT("");
}
