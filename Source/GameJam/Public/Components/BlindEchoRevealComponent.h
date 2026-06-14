#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "BlindEchoRevealComponent.generated.h"

class UCameraComponent;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UPrimitiveComponent;
class USceneComponent;

USTRUCT()
struct FBlindEchoRuntimeEntry
{
    GENERATED_BODY()

    TWeakObjectPtr<UPrimitiveComponent> Component;
    FVector ImpactPoint = FVector::ZeroVector;
    float Radius = 120.0f;
    float StartTime = 0.0f;
    float LifeTime = 3.5f;
    int32 Slot = INDEX_NONE;
};

USTRUCT()
struct FBlindEchoCustomDepthState
{
    GENERATED_BODY()

    bool bWasRenderingCustomDepth = false;
    int32 PreviousStencilValue = 0;
    int32 RefCount = 0;
};

UCLASS(ClassGroup=(BlindDemo), meta=(BlueprintSpawnableComponent))
class GAMEJAM_API UBlindEchoRevealComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBlindEchoRevealComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Blind Echo")
    bool InitializeEchoMaterial();

    UFUNCTION(BlueprintCallable, Category="Blind Echo")
    void SetEchoPostProcessMaterial(UMaterialInterface* InMaterial);

    UFUNCTION(BlueprintCallable, Category="Blind Echo")
    void SetTargetCamera(UCameraComponent* InCamera);

    UFUNCTION(BlueprintCallable, Category="Blind Echo")
    bool RevealByLineTrace(const FVector& Start, const FVector& End);

    UFUNCTION(BlueprintCallable, Category="Blind Echo")
    int32 RevealBySphereSweep(const FVector& Start, const FVector& End, float SweepRadius);

    UFUNCTION(BlueprintCallable, Category="Blind Echo")
    bool RevealFromSceneComponent(USceneComponent* TraceOrigin, float Distance, bool bSweep);

    UFUNCTION(BlueprintCallable, Category="Blind Echo")
    bool RevealFromHit(const FHitResult& Hit);

    UFUNCTION(BlueprintCallable, Category="Blind Echo")
    void ClearAllEchoes();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blind Echo|Post Process")
    TObjectPtr<UMaterialInterface> EchoPostProcessMaterial = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blind Echo|Post Process")
    TObjectPtr<UCameraComponent> TargetCamera = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blind Echo|Post Process", meta=(ClampMin="0.0", ClampMax="1.0"))
    float PostProcessBlendWeight = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blind Echo|Trace")
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blind Echo|Trace")
    bool bTraceComplex = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blind Echo|Trace", meta=(ClampMin="1.0"))
    float DefaultTraceDistance = 140.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blind Echo|Trace", meta=(ClampMin="0.0"))
    float DefaultSweepRadius = 18.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blind Echo|Reveal", meta=(ClampMin="1.0"))
    float EchoRadius = 140.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blind Echo|Reveal", meta=(ClampMin="0.05"))
    float EchoLifeTime = 3.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blind Echo|Reveal", meta=(ClampMin="1", ClampMax="32"))
    int32 MaxEchoSlots = 16;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blind Echo|Reveal", meta=(ClampMin="0", ClampMax="255"))
    int32 CustomDepthStencilValue = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blind Echo|Reveal")
    bool bRestoreCustomDepthOnExpire = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blind Echo|Debug")
    bool bDrawDebugTrace = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blind Echo|Debug")
    float DebugDrawTime = 0.75f;

private:
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> EchoMaterialInstance = nullptr;

    TArray<FBlindEchoRuntimeEntry> ActiveEchoes;
    TMap<TWeakObjectPtr<UPrimitiveComponent>, FBlindEchoCustomDepthState> CustomDepthStates;
    int32 NextEchoSlot = 0;

    bool EnsureMaterialInstance();
    int32 AllocateEchoSlot();
    void RemoveEchoAtIndex(int32 EchoIndex);
    void RemoveEchoInSlot(int32 Slot);
    void MarkComponentForReveal(UPrimitiveComponent* Component);
    void ReleaseComponentReveal(UPrimitiveComponent* Component);
    void WriteEchoSlot(int32 Slot, const FVector& ImpactPoint, float Radius, float StartTime, float LifeTime, float Strength);
    void DisableEchoSlot(int32 Slot);
    FName MakeSlotVectorName(const TCHAR* Prefix, int32 Slot) const;
};
