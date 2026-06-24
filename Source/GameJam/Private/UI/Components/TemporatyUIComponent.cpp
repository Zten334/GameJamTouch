// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/TemporatyUIComponent.h"


void UTemporatyUIComponent::Init(const float duration)
{
	Duration = duration;
	if (Duration > 0.f)
	{
		//设置定时器之后按时销毁自己
		GetGameInstance()->GetTimerManager().SetTimer(TimerHandle,[this]()->void{ RemoveFromParent(); } ,Duration,false);
	}
}
