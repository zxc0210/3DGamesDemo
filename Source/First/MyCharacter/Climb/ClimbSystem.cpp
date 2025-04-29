// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbSystem.h"

#include "First/MyCharacter/EpicCharacter.h"

// Sets default values for this component's properties
UClimbSystem::UClimbSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true; // 默认禁用 Tick
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageObj(TEXT("/Game/_project/Character/Erika_Archer/Animation/Climb/ArcherCharacter_ToClimb_Montage"));
	if (MontageObj.Succeeded())
	{
		ToClimbMontage = MontageObj.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageObj2(TEXT("/Game/_project/Character/Erika_Archer/Animation/Climb/ArcherCharacter_ClimbDrop_Montage"));
	if (MontageObj2.Succeeded())
	{
		LeaveClimbMontage = MontageObj2.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageObj3(TEXT("/Game/_project/Character/Erika_Archer/Animation/Climb/Braced_Hang_Hop_Up_Montage"));
	if (MontageObj3.Succeeded())
	{
		ClimbUpMontage = MontageObj3.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageObj4(TEXT("/Game/_project/Character/Erika_Archer/Animation/Climb/ClimbToLadderTop_ArcherCharacter_Montage"));
	if (MontageObj4.Succeeded())
	{
		ClimbToLadderTopMontage = MontageObj4.Object;
	}
}

void UClimbSystem::SetOwningCharacter(AEpicCharacter* Character)
{
	owner = Character;
}

// Called when the game starts
void UClimbSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	this->SetComponentTickEnabled(false); 
}

// Called every frame
void UClimbSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (isMontagePlaying && ClimbMontageDuration > 0.0f)
	{
		//设置动画的目标变换
		TimeElapsed += DeltaTime;
		float Alpha = FMath::Clamp(TimeElapsed / ClimbMontageDuration, 0.0f, 1.0f);
		FVector NewLocation = FMath::Lerp(ClimbStartLocation, TargetClimbLocation, Alpha);
		
		owner->SetActorLocation(NewLocation);
		if (Alpha >= 1.0f) // 确保到达目标位置
		{
			isMontagePlaying = false;
		}
	}

	// if (isladder)
	// {
	// 	FString String = FString::Printf(TEXT("%f,%f,%f"), owner->GetActorLocation().X,owner->GetActorLocation().Y,owner->GetActorLocation().Z);
	// 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, String);
	// 	TimeElapsed += DeltaTime;
	// 	if (TimeElapsed>ClimbMontageDuration)
	// 	{
	// 		isladder = false;
	// 		owner->SetActorLocation(FVector(owner->GetActorLocation().X+100,owner->GetActorLocation().Y,owner->GetActorLocation().Z+100));
	// 	}
	// }
}

void UClimbSystem::ToClimbMontagePlay(const FVector& TargetLocation)
{
	 this->SetComponentTickEnabled(true); // 只在攀爬时启用 Tick
	
	TimeElapsed=0.0f;
	TargetClimbLocation = TargetLocation; // 存储目标位置
	ClimbStartLocation = owner->GetActorLocation(); // 记录初始位置
	
	ClimbMontageDuration = ToClimbMontage->GetPlayLength()/1.2f; // 获取蒙太奇时长
	
	//启用根运动可以防止在播放蒙太奇的时候移动角色
	owner->GetMesh()->GetAnimInstance()->Montage_Play(ToClimbMontage,1.2f);
	isMontagePlaying = true;

}

void UClimbSystem::LeaveClimbMontagePlay()
{
	this->SetComponentTickEnabled(false);
	owner->GetMesh()->GetAnimInstance()->Montage_Play(LeaveClimbMontage,1.0f);
}

void UClimbSystem::ClimbUpMontagePlay(const FVector& TargetLocation)
{
	if (isMontagePlaying)
	{
		return;
	}
	TimeElapsed=0.0f;
	TargetClimbLocation = TargetLocation; // 存储目标位置
	ClimbStartLocation = owner->GetActorLocation(); // 记录初始位置
	ClimbMontageDuration = ToClimbMontage->GetPlayLength()/1.5f + ClimbUpMontage->GetDefaultBlendInTime(); // 获取蒙太奇时长
	
	owner->GetMesh()->GetAnimInstance()->Montage_Play(ClimbUpMontage,1.5f);
	isMontagePlaying = true;
}

void UClimbSystem::ClimbToLadderTopMontagePlay()
{
	if (isMontagePlaying)
	{
		return;
	}
	TimeElapsed=0.0f;
	//TargetClimbLocation = FVector(owner->GetActorLocation().X ,owner->GetActorLocation().Y,owner->GetActorLocation().Z+230); 
	FVector ForwardDirection = owner->GetActorForwardVector();
	ForwardDirection.Normalize();
	TargetClimbLocation = owner->GetActorLocation() + FVector(0, 0, 220) + (ForwardDirection * 10);// 存储目标位置为角色前上方
	ClimbStartLocation = owner->GetActorLocation(); // 记录初始位置
	ClimbMontageDuration = ClimbToLadderTopMontage->GetPlayLength()/1.0f; // 获取蒙太奇时长
	
	owner->GetMesh()->GetAnimInstance()->Montage_Play(ClimbToLadderTopMontage,1.0f);
	isMontagePlaying = true;
}
