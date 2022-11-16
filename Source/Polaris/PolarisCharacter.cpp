// Copyright Epic Games, Inc. All Rights Reserved.

#include "PolarisCharacter.h"
#include "PolarisProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"

#include "MyCharMovComp.h"

//////////////////////////////////////////////////////////////////////////
// APolarisCharacter

APolarisCharacter::APolarisCharacter(const FObjectInitializer& objectInit)
	: Super(objectInit.SetDefaultSubobjectClass<UMyCharMovComp>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	GetCapsuleComponent()->SetSimulatePhysics(true);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

}

void APolarisCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	CharMovComp = Cast<UMyCharMovComp>(GetCharacterMovement());
}

//////////////////////////////////////////////////////////////////////////// Input

void APolarisCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &APolarisCharacter::OnPrimaryAction);

	// Bolt event
	PlayerInputComponent->BindAction("Bolt", IE_Pressed, this, &APolarisCharacter::Bolt);

	// Bolt event
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APolarisCharacter::Crouch);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APolarisCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &APolarisCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &APolarisCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &APolarisCharacter::LookUpAtRate);
}

void APolarisCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void APolarisCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnPrimaryAction();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void APolarisCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void APolarisCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	switch (PrevMovementMode)
	{
	case MOVE_None:
		break;
	case MOVE_Walking:
		break;
	case MOVE_NavWalking:
		break;
	case MOVE_Falling:
		CharMovComp->LandingBehviour();
		break;
	case MOVE_Swimming:
		break;
	case MOVE_Flying:
		break;
	case MOVE_Custom:
		break;
	case MOVE_MAX:
		break;
	default:
		break;
	}
}

#pragma region Locomotion

	void APolarisCharacter::MoveForward(float Value)
	{
		forwardAxesValue = Value;

		if (Value != 0.0f)
		{
			// add movement in that direction
			AddMovementInput(GetActorForwardVector(), Value);
		}
	}

	void APolarisCharacter::MoveRight(float Value)
	{
		rightAxesValue = Value;

		if (Value != 0.0f)
		{
			// add movement in that direction
			AddMovementInput(GetActorRightVector(), Value);
		}
	}

	void APolarisCharacter::TurnAtRate(float Rate)
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
	}

	void APolarisCharacter::LookUpAtRate(float Rate)
	{
		// calculate delta for this frame from the rate information
		AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
	}

	void APolarisCharacter::Bolt()
	{
		// Calculate the direction
		FVector rightValue = GetActorRightVector() * rightAxesValue;
		FVector forwardValue = GetActorForwardVector() * forwardAxesValue;

		FVector boltDirection = rightValue + forwardValue;

		// Normalize the Vector 
		FVector boltDirectionNormalized = boltDirection.GetSafeNormal();

		if (CharMovComp)
		{
			if (boltDirection.Length() != 0)
				CharMovComp->UseBolt(boltDirectionNormalized);
			else
				CharMovComp->UseBolt(GetActorForwardVector());
		}
	}

	void APolarisCharacter::Crouch()
	{
		CharMovComp->UseCrouch();
	}

#pragma endregion Locomotion

bool APolarisCharacter::CanJumpInternal_Implementation() const
{
	return JumpIsAllowedInternal();
}

bool APolarisCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APolarisCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &APolarisCharacter::EndTouch);

		return true;
	}
	
	return false;
}