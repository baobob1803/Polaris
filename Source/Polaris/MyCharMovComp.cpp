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

	GetWorld()->GetTimerManager().SetTimer(boltCoolDownHandler, this, &UMyCharMovComp::BoltCoolDown, 0.1f, true);
	GetWorld()->GetTimerManager().PauseTimer(boltCoolDownHandler);
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
			{
				SetMovementMode(MOVE_Walking);
			}
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
					SetMovementMode(MOVE_Falling);

					isJumping = true;
					jumpTime = jumpMinTime;
					prevJumpCurveValue = jumpCurve->GetFloatValue(jumpMinTime);

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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Dash!"));
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
	{
		boltTouchFloorAfter = true;
	}

	if (boltCDRemainTime <= 0 && boltTouchFloorAfter)
	{
		canBolt = true;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("Stop"));
		GetWorld()->GetTimerManager().PauseTimer(boltCoolDownHandler);
	}
}

/*bool UMyCharMovComp::IsFalling() const
{
	if (isJumping)
	{
		return Super::IsFalling() || isJumping;
	}

	return Super::IsFalling();
}*/