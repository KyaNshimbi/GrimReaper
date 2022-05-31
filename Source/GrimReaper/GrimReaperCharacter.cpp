// Fill out your copyright notice in the Description page of Project Settings.


#include "GrimReaperCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemComponent.h"
#include "GrimReaperAttributeSet.h"

// Sets default values
AGrimReaperCharacter::AGrimReaperCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// Set turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Just affect camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw =  false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	// Character moves in the direction of input
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//Rotate at this rate
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	// The camera follows at this distance behind the character
	CameraBoom->TargetArmLength = 300.0f;
	// Rotate the arm based on the controller
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Camera does not rotate relative to arm
	FollowCamera->bUsePawnControlRotation = false;

	/* Note: The skeletal mesh and anim blueprint references on the mesh component (inherited from character)
	   are set in the derived blueprint to avoid direct content reference in C++*/

	// Add the ability system component to the character
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Componet"));
}

void AGrimReaperCharacter::CancelAbility(const FGameplayTagContainer CancelWithTags)
{
	// The ability system component has multiple ways of cancelling and abilities
	// Using tages provides a generic means of categorizing different types of abilities with knowing what specific ability is active
	AbilitySystemComponent->CancelAbilities(&CancelWithTags);
}

// Called when the game starts or when spawned
void AGrimReaperCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Initializes the attibute set using a data table
	if (IsValid(AbilitySystemComponent))
	{
		AttributeSet = AbilitySystemComponent->GetSet<UGrimReaperAttributeSet>();

		// GetGameplayAttibuteValueChangedDelegate will enable you to bind delegates without programming them manually
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &AGrimReaperCharacter::OnHealthChangedInternal);
	}
	
}


void AGrimReaperCharacter::OnHealthChangedInternal(const FOnAttributeChangeData & Data)
{
	OnHealthChanged(Data.OldValue, Data.NewValue);
}

// Called every frame
void AGrimReaperCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AGrimReaperCharacter::GetHealth() const
{
	if(IsValid(AttributeSet))
	{
		return AttributeSet->GetHealth();
	}
	return -1.0f;
}

float AGrimReaperCharacter::GetMaxHealth() const
{
	if(IsValid(AttributeSet))
	{
		return AttributeSet->GetMaxHealth();
	}
	return 0.0f;
}


float AGrimReaperCharacter::GetBaseDamage() const
{
	if(IsValid(AttributeSet))
	{
		return AttributeSet->GetWeaponDamage();
	}
	return 0.0f;
}


// Create an instance of an ability and add it to the ability system component
void AGrimReaperCharacter::GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level, int32 InputCode)
{
	if(GetLocalRole() == ROLE_Authority && IsValid(AbilitySystemComponent) && IsValid(AbilityClass))
	{
		UGameplayAbility* Ability = AbilityClass->GetDefaultObject<UGameplayAbility>();

		if(IsValid(Ability))
		{
			// Create the new ability spec struct. Ability specs contain metadata about the ability, for example the leve they are set to and the reference to the ability 
			FGameplayAbilitySpec AbilitySpec(
				Ability,
				Level,
				InputCode
			);
			AbilitySystemComponent->GiveAbility(AbilitySpec);
		}
	}
}

void AGrimReaperCharacter::ActivateAbility(int32 InputCode)
{
	if(IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->AbilityLocalInputPressed(InputCode);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input

void AGrimReaperCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveFoward", this, &AGrimReaperCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGrimReaperCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGrimReaperCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGrimReaperCharacter::LookUpAtRate);
}

void AGrimReaperCharacter::TurnAtRate(float Rate)
{
	// Calculate delata for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGrimReaperCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AGrimReaperCharacter::MoveForward(float Value)
{
	if((Controller != nullptr) && (Value != 0.0f))
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction =  FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AGrimReaperCharacter::MoveRight(float Value)
{
	if((Controller != nullptr) && (Value != 0.0f))
	{
		// Find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0,Rotation.Yaw, 0);

		// Get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// Add movement in that direction
		AddMovementInput(Direction, Value);
	}
}