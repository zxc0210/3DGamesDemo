// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MyBTTaskNode.generated.h"

/**
 * 
 */
UCLASS()
class FIRST_API UMyBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()


	
public:
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	bool bHasExecutedOnce = false;
	FVector InitCharacterLocation;
	void UBTTask_RandomMove();
};
