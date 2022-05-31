// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "GrimReaperCharacter.generated.h"

UCLASS()
class GRIMREAPER_API AGrimReaperCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	// Camera boom positioning the camera behind the character
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Camera
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	// Sets default values for this character's properties
	AGrimReaperCharacter();

	// Base turn rate in deg/sec
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	// Base look up/down rate in deg/sec
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	// A function to bind to attribute changed delegate. Calls on health changed
	void OnHealthChangedInternal(const FOnAttributeChangeData & Data);

	// Blueprint implentable on health changed event used to send information to the UI about current health
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities|Attributes")
	void OnHealthChanged(float oldValue, float newValue);

	// Blueprint accessible getter for the current health value
	UFUNCTION(BlueprintPure, Category = "Abilities|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "Abilities|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "Abilities|Attributes")
	float GetBaseDamage() const;
	
	// Ability system component required to use gameplay attributes and gameplay abilities
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	class UAbilitySystemComponent* AbilitySystemComponent;

	// Ability System Interface getter
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	}

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level, int32 InputCode);

	// Activates an ability with a matching input code
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void ActivateAbility(int32 InputCode);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CancelAbility(const FGameplayTagContainer CancelWithTags);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Character Attribute Set on Character.h UPROPERTY macro required for reflection
	UPROPERTY()
	const class UGrimReaperAttributeSet* AttributeSet;

	// Called for forward/backward input
	void MoveForward(float Value);

	// Called for side to side input
	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
