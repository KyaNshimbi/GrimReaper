// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GrimReaperAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class GRIMREAPER_API UGrimReaperAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	// Character Health Attribute
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	FGameplayAttributeData Health;

	// Health Getter (implemented through macro to save time)
	GAMEPLAYATTRIBUTE_VALUE_GETTER(Health);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(Health);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Health);
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UGrimReaperAttributeSet, Health);

	// Character Health Attribute
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	FGameplayAttributeData MaxHealth;
	GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxHealth);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(MaxHealth);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxHealth);
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UGrimReaperAttributeSet, MaxHealth);

	// Character Damage Attribute
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	FGameplayAttributeData WeaponDamage;
	// Base Damage Getter
	GAMEPLAYATTRIBUTE_VALUE_GETTER(WeaponDamage);
};
