#include "BlindEchoRevealComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

UBlindEchoRevealComponent::UBlindEchoRevealComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UBlindEchoRevealComponent::BeginPlay()
{
    Super::BeginPlay();

    InitializeEchoMaterial();
}

void UBlindEchoRevealComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ClearAllEchoes();
    Super::EndPlay(EndPlayReason);
}

void UBlindEchoRevealComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!EchoMaterialInstance)
    {
        return;
    }

    const UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const float Now = World->GetTimeSeconds();
    EchoMaterialInstance->SetScalarParameterValue(TEXT("EchoNow"), Now);

    for (int32 Index = ActiveEchoes.Num() - 1; Index >= 0; --Index)
    {
        const FBlindEchoRuntimeEntry& Echo = ActiveEchoes[Index];
        if (Now - Echo.StartTime >= Echo.LifeTime)
        {
            RemoveEchoAtIndex(Index);
        }
    }
}

bool UBlindEchoRevealComponent::InitializeEchoMaterial()
{
    if (!EnsureMaterialInstance())
    {
        return false;
    }

    MaxEchoSlots = FMath::Clamp(MaxEchoSlots, 1, 32);
    NextEchoSlot = 0;

    if (!TargetCamera)
    {
        if (AActor* Owner = GetOwner())
        {
            TargetCamera = Owner->FindComponentByClass<UCameraComponent>();
        }
    }

    if (TargetCamera)
    {
        TargetCamera->PostProcessSettings.AddBlendable(EchoMaterialInstance, PostProcessBlendWeight);
        TargetCamera->PostProcessBlendWeight = FMath::Max(TargetCamera->PostProcessBlendWeight, PostProcessBlendWeight);
    }

    EchoMaterialInstance->SetScalarParameterValue(TEXT("EchoNow"), GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f);
    EchoMaterialInstance->SetScalarParameterValue(TEXT("EchoSlotCount"), static_cast<float>(MaxEchoSlots));

    for (int32 Slot = 0; Slot < MaxEchoSlots; ++Slot)
    {
        DisableEchoSlot(Slot);
    }

    return true;
}

void UBlindEchoRevealComponent::SetEchoPostProcessMaterial(UMaterialInterface* InMaterial)
{
    EchoPostProcessMaterial = InMaterial;
    EchoMaterialInstance = nullptr;
    InitializeEchoMaterial();
}

void UBlindEchoRevealComponent::SetTargetCamera(UCameraComponent* InCamera)
{
    TargetCamera = InCamera;
    InitializeEchoMaterial();
}

bool UBlindEchoRevealComponent::RevealByLineTrace(const FVector& Start, const FVector& End)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(BlindEchoLineTrace), bTraceComplex);
    if (AActor* Owner = GetOwner())
    {
        QueryParams.AddIgnoredActor(Owner);
    }

    FHitResult Hit;
    const bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, QueryParams);

    if (bDrawDebugTrace)
    {
        DrawDebugLine(World, Start, End, bHit ? FColor::Green : FColor::Red, false, DebugDrawTime, 0, 1.0f);
        if (bHit)
        {
            DrawDebugSphere(World, Hit.ImpactPoint, EchoRadius, 16, FColor::Cyan, false, DebugDrawTime);
        }
    }

    return bHit && RevealFromHit(Hit);
}

int32 UBlindEchoRevealComponent::RevealBySphereSweep(const FVector& Start, const FVector& End, float SweepRadius)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return 0;
    }

    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(BlindEchoSphereSweep), bTraceComplex);
    if (AActor* Owner = GetOwner())
    {
        QueryParams.AddIgnoredActor(Owner);
    }

    TArray<FHitResult> Hits;
    const FCollisionShape Shape = FCollisionShape::MakeSphere(FMath::Max(0.1f, SweepRadius));
    const bool bHit = World->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, TraceChannel, Shape, QueryParams);

    if (bDrawDebugTrace)
    {
        DrawDebugLine(World, Start, End, bHit ? FColor::Green : FColor::Red, false, DebugDrawTime, 0, 1.0f);
        DrawDebugSphere(World, End, SweepRadius, 16, bHit ? FColor::Cyan : FColor::Red, false, DebugDrawTime);
    }

    int32 RevealedCount = 0;
    TSet<TWeakObjectPtr<UPrimitiveComponent>> RevealedComponents;

    for (const FHitResult& Hit : Hits)
    {
        UPrimitiveComponent* HitComponent = Hit.GetComponent();
        const TWeakObjectPtr<UPrimitiveComponent> WeakHitComponent(HitComponent);
        if (!HitComponent || RevealedComponents.Contains(WeakHitComponent))
        {
            continue;
        }

        if (RevealFromHit(Hit))
        {
            RevealedComponents.Add(WeakHitComponent);
            ++RevealedCount;
        }
    }

    return RevealedCount;
}

bool UBlindEchoRevealComponent::RevealFromSceneComponent(USceneComponent* TraceOrigin, float Distance, bool bSweep)
{
    if (!TraceOrigin)
    {
        return false;
    }

    const float ActualDistance = Distance > 0.0f ? Distance : DefaultTraceDistance;
    const FVector Start = TraceOrigin->GetComponentLocation();
    const FVector End = Start + TraceOrigin->GetForwardVector() * ActualDistance;

    if (bSweep)
    {
        return RevealBySphereSweep(Start, End, DefaultSweepRadius) > 0;
    }

    return RevealByLineTrace(Start, End);
}

bool UBlindEchoRevealComponent::RevealFromHit(const FHitResult& Hit)
{
    if (!EnsureMaterialInstance())
    {
        return false;
    }

    UPrimitiveComponent* HitComponent = Hit.GetComponent();
    if (!HitComponent)
    {
        return false;
    }

    const UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    const int32 Slot = AllocateEchoSlot();
    const float Now = World->GetTimeSeconds();
    const FVector ImpactPoint = Hit.ImpactPoint.IsNearlyZero() ? Hit.Location : Hit.ImpactPoint;

    MarkComponentForReveal(HitComponent);
    WriteEchoSlot(Slot, ImpactPoint, EchoRadius, Now, EchoLifeTime, 1.0f);

    FBlindEchoRuntimeEntry Entry;
    Entry.Component = HitComponent;
    Entry.ImpactPoint = ImpactPoint;
    Entry.Radius = EchoRadius;
    Entry.StartTime = Now;
    Entry.LifeTime = EchoLifeTime;
    Entry.Slot = Slot;
    ActiveEchoes.Add(Entry);

    return true;
}

void UBlindEchoRevealComponent::ClearAllEchoes()
{
    for (int32 Index = ActiveEchoes.Num() - 1; Index >= 0; --Index)
    {
        RemoveEchoAtIndex(Index);
    }

    if (EchoMaterialInstance)
    {
        for (int32 Slot = 0; Slot < MaxEchoSlots; ++Slot)
        {
            DisableEchoSlot(Slot);
        }
    }
}

bool UBlindEchoRevealComponent::EnsureMaterialInstance()
{
    if (EchoMaterialInstance)
    {
        return true;
    }

    if (!EchoPostProcessMaterial)
    {
        return false;
    }

    EchoMaterialInstance = UMaterialInstanceDynamic::Create(EchoPostProcessMaterial, this);
    return EchoMaterialInstance != nullptr;
}

int32 UBlindEchoRevealComponent::AllocateEchoSlot()
{
    MaxEchoSlots = FMath::Clamp(MaxEchoSlots, 1, 32);

    const int32 Slot = NextEchoSlot;
    NextEchoSlot = (NextEchoSlot + 1) % MaxEchoSlots;

    RemoveEchoInSlot(Slot);
    return Slot;
}

void UBlindEchoRevealComponent::RemoveEchoAtIndex(int32 EchoIndex)
{
    if (!ActiveEchoes.IsValidIndex(EchoIndex))
    {
        return;
    }

    const FBlindEchoRuntimeEntry Echo = ActiveEchoes[EchoIndex];
    if (Echo.Slot != INDEX_NONE)
    {
        DisableEchoSlot(Echo.Slot);
    }

    if (Echo.Component.IsValid())
    {
        ReleaseComponentReveal(Echo.Component.Get());
    }

    ActiveEchoes.RemoveAtSwap(EchoIndex);
}

void UBlindEchoRevealComponent::RemoveEchoInSlot(int32 Slot)
{
    for (int32 Index = ActiveEchoes.Num() - 1; Index >= 0; --Index)
    {
        if (ActiveEchoes[Index].Slot == Slot)
        {
            RemoveEchoAtIndex(Index);
            return;
        }
    }

    DisableEchoSlot(Slot);
}

void UBlindEchoRevealComponent::MarkComponentForReveal(UPrimitiveComponent* Component)
{
    if (!Component)
    {
        return;
    }

    FBlindEchoCustomDepthState& State = CustomDepthStates.FindOrAdd(Component);
    if (State.RefCount == 0)
    {
        State.bWasRenderingCustomDepth = Component->bRenderCustomDepth;
        State.PreviousStencilValue = Component->CustomDepthStencilValue;
    }

    ++State.RefCount;

    Component->SetRenderCustomDepth(true);
    Component->SetCustomDepthStencilValue(FMath::Clamp(CustomDepthStencilValue, 0, 255));
}

void UBlindEchoRevealComponent::ReleaseComponentReveal(UPrimitiveComponent* Component)
{
    if (!Component)
    {
        return;
    }

    FBlindEchoCustomDepthState* State = CustomDepthStates.Find(Component);
    if (!State)
    {
        return;
    }

    State->RefCount = FMath::Max(0, State->RefCount - 1);
    if (State->RefCount > 0)
    {
        return;
    }

    if (bRestoreCustomDepthOnExpire)
    {
        Component->SetCustomDepthStencilValue(State->PreviousStencilValue);
        Component->SetRenderCustomDepth(State->bWasRenderingCustomDepth);
    }

    CustomDepthStates.Remove(Component);
}

void UBlindEchoRevealComponent::WriteEchoSlot(int32 Slot, const FVector& ImpactPoint, float Radius, float StartTime, float LifeTime, float Strength)
{
    if (!EchoMaterialInstance)
    {
        return;
    }

    EchoMaterialInstance->SetVectorParameterValue(
        MakeSlotVectorName(TEXT("EchoPosRadius"), Slot),
        FLinearColor(ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z, Radius));

    EchoMaterialInstance->SetVectorParameterValue(
        MakeSlotVectorName(TEXT("EchoTime"), Slot),
        FLinearColor(StartTime, LifeTime, Strength, 0.0f));
}

void UBlindEchoRevealComponent::DisableEchoSlot(int32 Slot)
{
    if (!EchoMaterialInstance)
    {
        return;
    }

    EchoMaterialInstance->SetVectorParameterValue(
        MakeSlotVectorName(TEXT("EchoPosRadius"), Slot),
        FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));

    EchoMaterialInstance->SetVectorParameterValue(
        MakeSlotVectorName(TEXT("EchoTime"), Slot),
        FLinearColor(-100000.0f, 1.0f, 0.0f, 0.0f));
}

FName UBlindEchoRevealComponent::MakeSlotVectorName(const TCHAR* Prefix, int32 Slot) const
{
    return FName(*FString::Printf(TEXT("%s_%d"), Prefix, Slot));
}
