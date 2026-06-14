// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/PlayerUIComponent.h"
#include "State/State.h"

UPlayerUIComponent::UPlayerUIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerUIComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerUIComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 消息队列倒计时
	for (int32 i = MessageQueue.Num() - 1; i >= 0; --i)
	{
		MessageQueue[i].RemainingTime -= DeltaTime;
		if (MessageQueue[i].RemainingTime <= 0.f)
		{
			MessageQueue.RemoveAt(i);
		}
	}

	if (MessageQueue.Num() > 0)
	{
		CurrentMessage = MessageQueue[0].Text;
		bHasMessage = true;
	}
	else
	{
		CurrentMessage = FText::GetEmpty();
		bHasMessage = false;
	}

	// 自动从 PlayerState 拉血条
	SyncHealthFromPlayerState();
}

void UPlayerUIComponent::SyncHealthFromPlayerState()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	AState* MyState = Cast<AState>(OwnerPawn->GetPlayerState());
	if (MyState)
	{
		HealthPercent = FMath::Clamp(MyState->Health / 100.f, 0.f, 1.f);
	}
}

void UPlayerUIComponent::ShowMessage(FText Message, float Duration)
{
	if (Duration <= 0.f) return;

	FFloatingMessage Entry;
	Entry.Text = Message;
	Entry.RemainingTime = Duration;
	MessageQueue.Add(Entry);
}

void UPlayerUIComponent::UpdateTimer(float Seconds)
{
	int32 Min = FMath::FloorToInt(Seconds / 60.f);
	int32 Sec = FMath::FloorToInt(FMath::Fmod(Seconds, 60.f));
	TimerText = FText::FromString(FString::Printf(TEXT("%02d:%02d"), Min, Sec));
}
