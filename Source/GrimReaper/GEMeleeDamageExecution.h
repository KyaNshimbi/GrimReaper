// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEMeleeDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class GRIMREAPER_API UGEMeleeDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	// Constructor sets up the attribute captures needed to run the calculation
	UGEMeleeDamageExecution();

	// Execution implementation. The bulk of this operation is in this function
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};
