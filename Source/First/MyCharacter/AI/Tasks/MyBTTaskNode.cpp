// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTaskNode.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "First/MyCharacter/NPCBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

EBTNodeResult::Type UMyBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 获取角色
	const AController* Controller = Cast<AController>(OwnerComp.GetOwner());
	ACharacter* Character = Controller ? Controller->GetCharacter() : nullptr;
	if (!Character)
	{
		return EBTNodeResult::Failed;
	}
	
	if (!bHasExecutedOnce)
	{
		// 获取角色的初始位置
		FVector CharacterLocation = Character->GetActorLocation();
		InitCharacterLocation = CharacterLocation;
		bHasExecutedOnce = true;
	}
	
	// 获取行为树的黑板
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		if (!BlackboardComp->GetValueAsBool("IsChase"))	// 检查黑板中的追逐状态
		{
			Character->GetCharacterMovement()->MaxWalkSpeed = 200.0f;	//设置随机移动最大速度
			// 获取可导航半径内的随机位置
			FNavLocation RandomLocation;
			UNavigationSystemV1* NavS = UNavigationSystemV1::GetCurrent(GetWorld());
			if (NavS && NavS->GetRandomPointInNavigableRadius(InitCharacterLocation, 500.0f, RandomLocation))
			{
				// 将更新的位置保存到黑板中
				BlackboardComp->SetValueAsVector("OwnerCurrentLocation", RandomLocation.Location);
			}
			
		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}
	return EBTNodeResult::Succeeded;
}

void UMyBTTaskNode::UBTTask_RandomMove()
{
	
}
