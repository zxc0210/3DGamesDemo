// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWorldAssets.h"

#include "First/MySoueces/Fire/MyAssets.h"


// Sets default values
ABaseWorldAssets::ABaseWorldAssets()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseWorldAssets::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseWorldAssets::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseWorldAssets::UI_Show()
{
	
}

void ABaseWorldAssets::UI_Hide()
{
	
}

void ABaseWorldAssets::UI_Update(FActorResourceData* ActorResourceData)
{
	
}


TArray<EMySourceType> ABaseWorldAssets::GetDropResourceType()
{
	TArray<int32> GenerateResourceNum; //生成的资源数量
	
	//根据生成资源类型随机生成掉落总数量
	for (int i = 0; i < GenerateResourceType.Num(); ++i)
	{
		EMySourceType st = GenerateResourceType[i];
		int32 MaxNum = 2 * AMyAssets::GetResourceMaxStackNum(st); //每种的资源数量最多为最大堆叠数量的2倍
		int32 Count = FMath::RandRange(1, MaxNum);
		GenerateResourceNum.Add(Count);
	}
	//根据生成类型与数量计算实际分配的资源数组与数量数组
	for (int i = 0; i < GenerateResourceType.Num(); ++i)
	{
		EMySourceType st = GenerateResourceType[i];
		int32 MaxStackNum = AMyAssets::GetResourceMaxStackNum(st); 
		int32 RemainingCount = GenerateResourceNum[i]; // 当前资源的数量
		// 计算所需的格子数，并分配数量
		while (RemainingCount > 0)
		{
			int32 AllocatedCount = FMath::Min(MaxStackNum, RemainingCount); // 分配的数量不超过最大堆叠数量
			DropResourceTypeArray.Add(st);
			DropResourceNumArray.Add(AllocatedCount); 
			RemainingCount -= AllocatedCount; // 减少剩余的未分配数量
		}
	}

	return DropResourceTypeArray;
}

TArray<int32> ABaseWorldAssets::GetDropResourceNum()
{
	return DropResourceNumArray;
}

void ABaseWorldAssets::SetHighLight()
{
	
}

void ABaseWorldAssets::DisableHighLight()
{
	
}

