// Fill out your copyright notice in the Description page of Project Settings.


#include "GEMeleeDamageExecution.h"
#include "GrimReaperAttributeSet.h"
#include "AbilitySystemComponent.h"

// Helper struct for fetching the stats necessary for the melee calculation
struct FMeleeDamageStatics
{
    // Capturedef declarations for attributes
    DECLARE_ATTRIBUTE_CAPTUREDEF(WeaponDamage);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
    
    // Default constructor
    FMeleeDamageStatics()
    {
        // Weapon damage from the source of the gameplay effect running this calculation. Snapshotted at the moment the calculation is made
        DEFINE_ATTRIBUTE_CAPTUREDEF(UGrimReaperAttributeSet, WeaponDamage, Source, true);

        // Health from the target of the gameplay effect using this calculation
        DEFINE_ATTRIBUTE_CAPTUREDEF(UGrimReaperAttributeSet, Health, Target, false);
    }
};

// Static helper function to quickly fetch the melee damage capture attributes
static const FMeleeDamageStatics& MeleeDamageStatics()
{
    static FMeleeDamageStatics DmgStatics;
    return DmgStatics;
}

// Constructor for the melee damage execution. We capture only the attributes used to make the calculation, not the health attribute itself. We apply that at the end of the execution 
UGEMeleeDamageExecution::UGEMeleeDamageExecution()
{
    RelevantAttributesToCapture.Add(MeleeDamageStatics().WeaponDamageDef);
}

// Main implementation for the execution
void UGEMeleeDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const 
{
    // Step 1: Obtain references to the target and source ability system component. Each copy of a gameplay effect will have one of each
    UAbilitySystemComponent* TargetABSC = ExecutionParams.GetTargetAbilitySystemComponent();
    AActor* TargetActor = TargetABSC ? TargetABSC-> GetAvatarActor() : nullptr;

    UAbilitySystemComponent* SourceABSC = ExecutionParams.GetSourceAbilitySystemComponent();
    AActor* SourceActor = SourceABSC ? SourceABSC->GetAvatarActor() : nullptr;

    // Get the owning gameplay effect spec so that you can use its variables and tags
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    // Aggregator evaluates paramates used during the attribute capture
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    //Capturing the weapon damage
    float BaseDamage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeBonusMagnitude(MeleeDamageStatics().WeaponDamageDef, EvaluationParameters, BaseDamage);

    // Performing the actual damage calculation.
    float DamageDone = BaseDamage;

    // Minimum damage floor with be 1
    if(DamageDone <= 0.f)
    {
        DamageDone = 1.0f;
    }

    // Final execution output. We can add more than one AddOutputModifier to change multiple parameters at a time based on more complicated calculations. Here we apply -DamageDone to the Health. Health itself is clamped in the AttributeSet
    OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(MeleeDamageStatics().HealthProperty, EGameplayModOp::Additive, -DamageDone));
}