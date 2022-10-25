// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharMovComp.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

void UMyCharMovComp::BeginPlay()
{
	Super::BeginPlay();

	if (jumpCurve)
		jumpCurve->GetTimeRange(jumpMinTime, jumpMaxTime);
}

void UMyCharMovComp::TickComponent(float delta_time, ELevelTick tick_type, FActorComponentTickFunction* tick_function)
{
	Super::TickComponent(DELTA, tick_type, tick_function);

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

			FLatentActionInfo latent_info;
			latent_info.CallbackTarget = this;
			UKismetSystemLibrary::MoveComponentTo((USceneComponent*)CharacterOwner->GetCapsuleComponent(), targetLocation, CharacterOwner->GetActorRotation(), false, false, 0.0f, true, EMoveComponentAction::Type::Move, latent_info);

			prevJumpCurveValue = jumpCurveValue;
		}
		else
		{
			// Reached the end of the curve
			isJumping = false;
			SetMovementMode(MOVE_Walking);
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
				SetMovementMode(MOVE_Falling);

				isJumping = true;
				jumpTime = jumpMinTime;
				prevJumpCurveValue = jumpCurve->GetFloatValue(jumpMinTime);

				return true;
			}
			else
				return Super::DoJump(bReplayiingMoves);
		}
	}

	return false;

	return Super::DoJump(bReplayiingMoves);
}

bool UMyCharMovComp::IsFalling() const
{
	if (isJumping)
	{
		return Super::IsFalling() || isJumping;
	}

	return Super::IsFalling();
}