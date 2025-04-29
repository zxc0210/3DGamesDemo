// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ClimbMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class FIRST_API UClimbMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()


protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;


private:
	UPROPERTY(EditAnywhere)
	float maxMaxBrakingDeceleration = 2048.0f; //最大制动速度

	UPROPERTY(EditAnywhere)
	float MaxClimbSpeed = 100.0f;//最大攀爬速度
	
	bool bIsClimbing = false;

public:

	void StartClimbing(bool EnableClimb);
	void SetClimbState(bool EnableClimb);
	bool isClimbing();
	void PhysClimb(float deltaTime, int32 Iterations);

	UFUNCTION(BlueprintCallable, Category = "CharacterParam")
	float GetUnrotatedClimbVelocity();

	
};
