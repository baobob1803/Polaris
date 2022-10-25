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
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float delta_time, ELevelTick tick_type, FActorComponentTickFunction* tick_function) override;
	virtual bool DoJump(bool bReplayingMoves) override;
	virtual bool IsFalling() const override;

	UPROPERTY(EDITAnywhere, BlueprintReadWrite, Category = "Custom Movement", meta = (DisplayName = "Jump Curve"))
	UCurveFloat * jumpCurve;

private:

	bool isJumping;
	float jumpTime;
	float prevJumpCurveValue;

	float jumpMinTime;
	float jumpMaxTime;

};