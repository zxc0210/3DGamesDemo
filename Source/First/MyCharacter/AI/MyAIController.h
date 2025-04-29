// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

/**
 * 
 */
UCLASS()
class FIRST_API AMyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMyAIController();
	virtual void BeginPlay() override;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
	UPROPERTY()
	UBlackboardComponent* BlackboardComp;
	UPROPERTY()
	class UBehaviorTreeComponent* BehaviorTreeComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UMyBlackboard* MyBlackboardData;
	// UPROPERTY(EditDefaultsOnly, Category = "AI")
	// class UMyBehaviorTree* MyBehaviorTree;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* MyBehaviorTree;
};
