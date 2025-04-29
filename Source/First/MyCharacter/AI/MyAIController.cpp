// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"

#include "MyBlackboard.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AMyAIController::AMyAIController()
{
	// 初始化黑板和行为树
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	
	//MyBlackboardData = CreateDefaultSubobject<UMyBlackboard>(TEXT("MyBlackboardData"));
	
	// 加载行为树和黑板
	static ConstructorHelpers::FObjectFinder<UMyBlackboard> BlackboardFinder(TEXT("/Game/_project/Character/NPC/NPCDataAsset.NPCDataAsset"));
	if (BlackboardFinder.Succeeded())
	{
		MyBlackboardData = BlackboardFinder.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeFinder(TEXT("/Game/_project/Character/NPC/NPCBehaviorTree.NPCBehaviorTree"));
	if (BehaviorTreeFinder.Succeeded())
	{
		MyBehaviorTree = BehaviorTreeFinder.Object;
	}

}
void AMyAIController::BeginPlay()
{
	Super::BeginPlay();
	
}
void AMyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (MyBlackboardData)
	{
		BlackboardComp->InitializeBlackboard(*MyBlackboardData);
	}
	
	if (MyBehaviorTree)
	{
		BehaviorTreeComp->StartTree(*MyBehaviorTree);
	}
}

void AMyAIController::OnUnPossess()
{
	if (BehaviorTreeComp)
	{
		BehaviorTreeComp->StopTree();
	}
}