// Fill out your copyright notice in the Description page of Project Settings.


#include "GrimReaperAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

void UGrimReaperAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    
    if(Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
    }
}

