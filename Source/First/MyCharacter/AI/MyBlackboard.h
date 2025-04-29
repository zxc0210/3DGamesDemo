// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardData.h"
#include "MyBlackboard.generated.h"

/**
 * 
 */
UCLASS()
class FIRST_API UMyBlackboard : public UBlackboardData
{
	GENERATED_BODY()
	
public:
	UMyBlackboard();

protected:
	class UBlackboardKeyType_Bool* IsChase;
	class UBlackboardKeyType_Bool* IsIdle;
	class UBlackboardKeyType_Vector* EnemyLocation;
	class UBlackboardKeyType_Vector* OwnerCurrentLocation;
};
