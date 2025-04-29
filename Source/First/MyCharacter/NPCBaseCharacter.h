// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "First/MySoueces/MySourcesType.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPCBaseCharacter.generated.h"

UCLASS()
class FIRST_API ANPCBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPCBaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	virtual void ReduceNPCBlood(int32 ReduceNum);
	virtual TArray<EMySourceType> GetDropResourceType();
	virtual TArray<int32> GetDropResourceNum();
	virtual int32 GetGenerateGridNum();
	
protected:
	int32 NPCBloodNUm;
	TArray<int32> DropResourceNumArray;
	TArray<EMySourceType> DropResourceTypeArray;
};
