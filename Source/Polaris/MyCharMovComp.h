// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyCharMovComp.generated.h"

/**
 * 
 */
UCLASS()
class POLARIS_API UMyCharMovComp : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float delta_time, ELevelTick tick_type, FActorComponentTickFunction* tick_function) override;
	virtual bool DoJump(bool bReplayingMoves) override;
	//virtual bool IsFalling() const override;

	virtual bool CanAttemptJump() const override;

	UFUNCTION()
	void BoltCoolDown();

	UFUNCTION()
	void CrouchFallFaster();

	void SlideTimer();

	void RecoverAfterLanding();

	void JumpBoostTimer();

	void SlideCD();

public: 

	void UseBolt(FVector direction);

	void UseCrouch();

	bool GetIsFastFalling();

	void LandingBehviour();

protected: 
	UPROPERTY(EDITAnywhere, BlueprintReadWrite, Category = "Custom Jump", meta = (DisplayName = "Jump Curve"))
	UCurveFloat * jumpCurve;

	UPROPERTY(EDITAnywhere, BlueprintReadWrite, Category = "Custom Jump", meta = (DisplayName = "Jump Recovery Time"))
	float jumpRecoveryTime = 0.15f;

	UPROPERTY(EDITAnywhere, BlueprintReadWrite, Category = "Fast Fall", meta = (DisplayName = "Fast Fall Recovery Time"))
	float fastFallSpeed = 1000.0f;

	UPROPERTY(EDITAnywhere, BlueprintReadWrite, Category = "Fast Fall", meta = (DisplayName = "Fast Fall Recovery Time"))
	float fastFallRecoveryTime = 0.1f;

	UPROPERTY(EDITAnywhere, BlueprintReadWrite, Category = "Bolt", meta = (DisplayName = "Bolt Strength", ToolTip = "Have effect on the speed and distance"))
	float boltStrength = 300.0f;

	UPROPERTY(EDITAnywhere, BlueprintReadWrite, Category = "Bolt", meta = (DisplayName = "Bolt Time"))
	float boltTime = 0.5f;

	UPROPERTY(EDITAnywhere, BlueprintReadWrite, Category = "Bolt", meta = (DisplayName = "Bolt Cooldown"))
	float boltCDDuration = 3.0f;

	UPROPERTY(EDITAnywhere, BlueprintReadWrite, Category = "Slide", meta = (DisplayName = "Slide Time"))
	float slideDuration = 0.5f;

	UPROPERTY(EDITAnywhere, BlueprintReadWrite, Category = "Slide", meta = (DisplayName = "Slide Speed"))
	float slideSpeed = 0.5f;

	UPROPERTY(EDITAnywhere, BlueprintReadWrite, Category = "Slide", meta = (DisplayName = "Slide Cooldown"))
	float slideCooldown = 0.15f;

private:

	bool isJumping;
	bool bCanJump = true;
	float jumpTime;
	float prevJumpCurveValue;

	float jumpMinTime;
	float jumpMaxTime;

	bool canBolt= true;
	bool isBolting;
	float boltRemainTime;

	float boltCDRemainTime;
	FTimerHandle boltCoolDownHandler;
	bool boltTouchFloorAfter;

	float slideRemainTime;
	bool isSliding;
	bool canSlide = true;
	float slideCDRemaining;

	bool isFastFalling = false;

	FTimerHandle fallFasterHandler;
	FTimerHandle slideHandler;
	FTimerHandle slideCDHandler;

	float actualRecoveryTime;
	FTimerHandle recoveryLandingHandler;

	FTimerHandle boostJumpTimerHandler;
	bool boostJumpAvailable = false;
	float boostJumpRemainTime;

	float defaultWalkSpeed;

	FVector startJumpLoc;
};