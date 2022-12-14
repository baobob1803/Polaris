// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharMovComp.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

UMyCharMovComp::UMyCharMovComp(const FObjectInitializer& objectInit)
	: Super(objectInit)
{

}

void UMyCharMovComp::BeginPlay()
{
	Super::BeginPlay();

	if (jumpCurve)
		jumpCurve->GetTimeRange(jumpMinTime, jumpMaxTime);

	defaultWalkSpeed = MaxWalkSpeed;

	MaxWalkSpeedCrouched = slideSpeed;

	GetWorld()->GetTimerManager().SetTimer(boltCoolDownHandler, this, &UMyCharMovComp::BoltCoolDown, 0.1f, true);
	GetWorld()->GetTimerManager().PauseTimer(boltCoolDownHandler);

	GetWorld()->GetTimerManager().SetTimer(fallFasterHandler, this, &UMyCharMovComp::CrouchFallFaster, 0.1f, true);
	GetWorld()->GetTimerManager().PauseTimer(fallFasterHandler);

	GetWorld()->GetTimerManager().SetTimer(slideHandler, this, &UMyCharMovComp::SlideTimer, 0.01f, true);
	GetWorld()->GetTimerManager().PauseTimer(slideHandler);

	GetWorld()->GetTimerManager().SetTimer(recoveryLandingHandler, this, &UMyCharMovComp::RecoverAfterLanding, 0.01f, true);
	GetWorld()->GetTimerManager().PauseTimer(recoveryLandingHandler);

	GetWorld()->GetTimerManager().SetTimer(boostJumpTimerHandler, this, &UMyCharMovComp::JumpBoostTimer, 0.01f, true);
	GetWorld()->GetTimerManager().PauseTimer(boostJumpTimerHandler);

	GetWorld()->GetTimerManager().SetTimer(slideCDHandler, this, &UMyCharMovComp::SlideCD, 0.01f, true);
	GetWorld()->GetTimerManager().PauseTimer(slideCDHandler);
}

void UMyCharMovComp::TickComponent(float delta_time, ELevelTick tick_type, FActorComponentTickFunction* tick_function)
{
	Super::TickComponent(delta_time, tick_type, tick_function);

	if (isJumping && jumpCurve)
	{
		jumpTime += delta_time;
		
		if (jumpTime <= jumpMaxTime) 
		{
			float jumpCurveValue = jumpCurve->GetFloatValue(jumpTime);
			float jumpCurveValuedelta = jumpCurveValue - prevJumpCurveValue;
			
			Velocity.Z = jumpCurveValuedelta / delta_time;

			FVector actorLocation = GetActorLocation();
			FVector targetLocation = actorLocation + FVector(0.0f, 0.0f, jumpCurveValuedelta);

			
			if (Velocity.Z > 0.0f)
			{
				FCollisionQueryParams roofCheckColParams;
				roofCheckColParams.AddIgnoredActor(CharacterOwner);
				FCollisionShape capsuleShape = FCollisionShape::MakeCapsule(CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(), CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

				FHitResult roofHitResult;
				bool isBlockingHit = GetWorld()->SweepSingleByProfile(roofHitResult, actorLocation, targetLocation, CharacterOwner->GetActorRotation().Quaternion(), CharacterOwner->GetCapsuleComponent()->GetCollisionProfileName(), capsuleShape, roofCheckColParams);

				//Check for roof collision
				if (isBlockingHit)
				{
					SetMovementMode(MOVE_Falling);

					isJumping = false;
					CharacterOwner->ResetJumpState();

					// Reset vertical velocity and let the gravity do the work
					Velocity.Z = 0.0f;

					targetLocation = actorLocation;
				}
			}

			FLatentActionInfo latent_info;
			latent_info.CallbackTarget = this;
			UKismetSystemLibrary::MoveComponentTo((USceneComponent*)CharacterOwner->GetCapsuleComponent(), targetLocation, CharacterOwner->GetActorRotation(), false, false, 0.0f, true, EMoveComponentAction::Type::Move, latent_info);

			if (Velocity.Z < 0.0f)
			{

			}

			prevJumpCurveValue = jumpCurveValue;
		}
		else
		{
			// Reached the end of the curve
			const FVector capsulLocation = UpdatedComponent->GetComponentLocation();
			FFindFloorResult floorResulst;
			FindFloor(capsulLocation, floorResulst, false);

			if (floorResulst.IsWalkableFloor() && IsValidLandingSpot(capsulLocation, floorResulst.HitResult))
				SetMovementMode(MOVE_Walking);
			else
				SetMovementMode(MOVE_Falling);

			isJumping = false;
			CharacterOwner->ResetJumpState();
		}
	}

	if (isBolting)
	{
		boltRemainTime -= delta_time;

		if (boltRemainTime <= 0)
		{
			Velocity = FVector(0,0,0);
			
			isBolting = false;
			boltCDRemainTime = boltCDDuration;
			SetMovementMode(MOVE_Falling);
			GetWorld()->GetTimerManager().UnPauseTimer(boltCoolDownHandler);
		}
	}
}

bool UMyCharMovComp::CanAttemptJump() const
{
	return IsJumpAllowed() &&
		bCanJump &&
		(IsMovingOnGround() || IsFalling()); // Falling included for double-jump and non-zero jump hold time, but validated by character.
}

bool UMyCharMovComp::DoJump(bool bReplayiingMoves)
{
	if (CharacterOwner && CharacterOwner->CanJump())
	{
		if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.0f)
		{
			if (jumpCurve)
			{
				if (!isJumping)
				{
					bWantsToCrouch = false;

					startJumpLoc = CharacterOwner->GetActorLocation();

					if (isSliding && !boostJumpAvailable)
					{
						GetWorld()->GetTimerManager().PauseTimer(slideHandler);
						isSliding = false;
						SetMovementMode(MOVE_Walking);
						Velocity = Velocity.GetSafeNormal() * defaultWalkSpeed;
					}	
					else if (boostJumpAvailable)
					{
						boostJumpAvailable = false;
						GetWorld()->GetTimerManager().PauseTimer(slideHandler);
						GetWorld()->GetTimerManager().PauseTimer(boostJumpTimerHandler);
						isSliding = false;
						Velocity = Velocity.GetSafeNormal() * MaxWalkSpeedCrouched;
					}

					SetMovementMode(MOVE_Falling);

					isJumping = true;
					jumpTime = jumpMinTime;
					prevJumpCurveValue = jumpCurve->GetFloatValue(jumpMinTime);

					bCanJump = false;

					return true;
				}
				
				return false;
			}
			else
				return Super::DoJump(bReplayiingMoves);
		}
	}

	return false;

	return Super::DoJump(bReplayiingMoves);
}

void UMyCharMovComp::JumpBoostTimer()
{
	boostJumpRemainTime -= 0.01f;

	if (boostJumpRemainTime <= 0)
	{
		boostJumpAvailable = false;
		MaxWalkSpeed = defaultWalkSpeed;
		GetWorld()->GetTimerManager().PauseTimer(boostJumpTimerHandler);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Boost no Available"));
	}
}

#pragma region Bolt

	void UMyCharMovComp::UseBolt(FVector direction)
	{
		if (PawnOwner && canBolt)
		{
			canBolt = false;
			isJumping = false;
			Velocity.Z = 0;
			isBolting = true;
			SetMovementMode(MOVE_Flying);
			AddImpulse(direction * boltStrength * 1000, false);
			boltRemainTime = boltTime;
		}
	}

	void UMyCharMovComp::BoltCoolDown()
	{
		boltCDRemainTime -= 0.1f;

		const FVector capsulLocation = UpdatedComponent->GetComponentLocation();
		FFindFloorResult floorResulst;
		FindFloor(capsulLocation, floorResulst, false);

		if (floorResulst.IsWalkableFloor() && IsValidLandingSpot(capsulLocation, floorResulst.HitResult))
			boltTouchFloorAfter = true;

		if (boltCDRemainTime <= 0 && boltTouchFloorAfter)
		{
			canBolt = true;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("Stop"));
			GetWorld()->GetTimerManager().PauseTimer(boltCoolDownHandler);
		}
	}

#pragma endregion 

#pragma region Crouch

	void UMyCharMovComp::UseCrouch()
	{
		if (MovementMode == MOVE_Falling && !isBolting)
		{
			isJumping = false;
			isFastFalling = true;
			GetWorld()->GetTimerManager().UnPauseTimer(fallFasterHandler);
		}
		else if (MovementMode == MOVE_Walking && Velocity.Equals(FVector3d(0, 0, 0)) == false && canSlide)
		{
			canSlide = false;
			slideRemainTime = slideDuration;
			isSliding = true;

			bWantsToCrouch = true;

			GetWorld()->GetTimerManager().UnPauseTimer(slideHandler);
		}
	}

	void UMyCharMovComp::CrouchFallFaster()
	{
		Velocity.Z = -1000;
	
		if(MovementMode == MOVE_Walking)
			GetWorld()->GetTimerManager().PauseTimer(fallFasterHandler);
	}
	
	void UMyCharMovComp::SlideCD()
	{
		slideCDRemaining -= 0.01f;

		if (slideCDRemaining <= 0.0f)
		{
			GetWorld()->GetTimerManager().PauseTimer(slideCDHandler);
			canSlide = true;
		}
	}

	//
	void UMyCharMovComp::SlideTimer()
	{
		slideRemainTime -= 0.01f;

		//Check if is at the end of the Slide
		if (slideRemainTime <= (slideDuration / 4) && !boostJumpAvailable)
		{
			boostJumpAvailable = true;

			//
			boostJumpRemainTime = slideRemainTime * 6;
			MaxWalkSpeed = defaultWalkSpeed * 1.5;
			GetWorld()->GetTimerManager().UnPauseTimer(boostJumpTimerHandler);
			

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Boost Available"));
		}

		if (slideRemainTime <= 0)
		{
			bWantsToCrouch = false;
			GetWorld()->GetTimerManager().PauseTimer(slideHandler);

			isSliding = false;
			slideCDRemaining = slideCooldown;

			GetWorld()->GetTimerManager().UnPauseTimer(slideCDHandler);
		}
	}

#pragma endregion 

#pragma region Landing

	// 
	void UMyCharMovComp::LandingBehviour()
	{
		// Check if he is in the Character Walk
		if (MovementMode = MOVE_Walking)
		{
			// Reset Walk speed
			MaxWalkSpeed = defaultWalkSpeed;
			canSlide = true;
			startJumpLoc = CharacterOwner->GetActorLocation() - startJumpLoc;

			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Orange, FString::Printf(TEXT("My Location is: %s"), *startJumpLoc.ToString()));

			//Check if the character fast falled before landing
			if (GetIsFastFalling())
			{
				isFastFalling = false;
				actualRecoveryTime = jumpRecoveryTime + fastFallRecoveryTime;
			}
			else
				actualRecoveryTime = jumpRecoveryTime;

			GetWorld()->GetTimerManager().UnPauseTimer(recoveryLandingHandler);
		}
	}

	bool UMyCharMovComp::GetIsFastFalling()
	{
		return isFastFalling;
	}

	// 
	void UMyCharMovComp::RecoverAfterLanding()
	{
		actualRecoveryTime -= 0.01f;

		if (actualRecoveryTime <= 0)
		{
			//MaxWalkSpeed = defaultWalkSpeed;
			bCanJump = true;
			GetWorld()->GetTimerManager().PauseTimer(recoveryLandingHandler);
		}
	}

#pragma endregion 

/*bool UMyCharMovComp::IsFalling() const
{
	if (isJumping)
	{
		return Super::IsFalling() || isJumping;
	}

	return Super::IsFalling();
}*/