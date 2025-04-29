// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCBaseCharacter.h"

// Sets default values
ANPCBaseCharacter::ANPCBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANPCBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANPCBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANPCBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANPCBaseCharacter::ReduceNPCBlood(int32 ReduceNum)
{
	
}

TArray<EMySourceType> ANPCBaseCharacter::GetDropResourceType()
{
	return TArray<EMySourceType>();
}

TArray<int32> ANPCBaseCharacter::GetDropResourceNum()
{
	return TArray<int32>();
}

int32 ANPCBaseCharacter::GetGenerateGridNum()
{
	return 6;
}


