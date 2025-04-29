// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClimbSystem.generated.h"


class AEpicCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIRST_API UClimbSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClimbSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	
	void SetOwningCharacter(AEpicCharacter* Character);
	void ToClimbMontagePlay(const FVector& TargetLocation);
	void LeaveClimbMontagePlay();
	void ClimbUpMontagePlay(const FVector& TargetLocation);
	void ClimbToLadderTopMontagePlay();


private:
	AEpicCharacter* owner;
	bool isMontagePlaying = false;
	float TimeElapsed;
	FVector TargetClimbLocation;
	FVector ClimbStartLocation;
	float ClimbMontageDuration;

	bool isladder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ToClimbMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LeaveClimbMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ClimbUpMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ClimbToLadderTopMontage;
};
