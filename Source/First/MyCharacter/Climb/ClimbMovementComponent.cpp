// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"

void UClimbMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType, ThisTickFunction);

}


void UClimbMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	//当移动状态切换时会自动调用该函数
	if (PreviousMovementMode == MOVE_Custom)	//退出
	{
		StopMovementImmediately();
	}
	
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UClimbMovementComponent::PhysClimb(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	RestorePreAdditiveRootMotionVelocity();

	if( !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{
		//摩檫力为0
		CalcVelocity(deltaTime, 0.1f, true, maxMaxBrakingDeceleration);
		
		// 限制最大攀爬速度
		if (Velocity.Size() > MaxClimbSpeed)
		{
			Velocity = Velocity.GetSafeNormal() * MaxClimbSpeed;
		}
	}

	ApplyRootMotionToVelocity(deltaTime);

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.Time < 1.f)
	{
		//adjust and try again
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	if(!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}
}



void UClimbMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if(isClimbing())
	{
		PhysClimb(deltaTime,Iterations);
	}

	Super::PhysCustom(deltaTime, Iterations);
}

void UClimbMovementComponent::StartClimbing(bool EnableClimb)
{
	if (EnableClimb)
	{
		SetMovementMode(MOVE_Custom);
	}
	else
	{
		SetMovementMode(MOVE_Falling);
	}
	SetClimbState(EnableClimb);
}

bool UClimbMovementComponent::isClimbing()
{
	return bIsClimbing;
}

void UClimbMovementComponent::SetClimbState(bool EnableClimb)
{
	bIsClimbing = EnableClimb;
}


// FVector UClimbMovementComponent::GetUnrotatedClimbVelocity()
// {
// 	return UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(),Velocity);
//
// }

float UClimbMovementComponent::GetUnrotatedClimbVelocity()
{
	FVector UnrotatedVelocity = UKismetMathLibrary::Quat_UnrotateVector(
		UpdatedComponent->GetComponentQuat(),
		Velocity
	);
	
	return UnrotatedVelocity.Z;
}