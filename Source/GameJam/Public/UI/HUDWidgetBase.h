// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidgetBase.generated.h"

class UPlayerUIComponent;

UCLASS()
class GAMEJAM_API UHUDWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 创建 HUD 后由 CharacterBase 调用一次，缓存组件引用 */
	void Init(UPlayerUIComponent* InUIComponent);

protected:
	/** 缓存好的组件引用，蓝图 Bind 时直接用 */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UPlayerUIComponent> UIComponent;
};
