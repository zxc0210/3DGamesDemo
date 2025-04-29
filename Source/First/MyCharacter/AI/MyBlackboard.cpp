// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlackboard.h"

#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UMyBlackboard::UMyBlackboard()
{
	// 初始化黑板键
	IsChase = CreateDefaultSubobject<UBlackboardKeyType_Bool>(TEXT("IsChase"));
	IsIdle = CreateDefaultSubobject<UBlackboardKeyType_Bool>(TEXT("IsIdle"));
	EnemyLocation = CreateDefaultSubobject<UBlackboardKeyType_Vector>(TEXT("EnemyLocation"));
	OwnerCurrentLocation = CreateDefaultSubobject<UBlackboardKeyType_Vector>(TEXT("OwnerCurrentLocation"));

	// 创建黑板条目并添加到 Keys 数组中
	FBlackboardEntry IsChaseEntry;
	IsChaseEntry.EntryName = FName(TEXT("IsChase"));
	IsChaseEntry.KeyType = IsChase;
	IsChaseEntry.bInstanceSynced = false;

	FBlackboardEntry IsIdleEntry;
	IsIdleEntry.EntryName = FName(TEXT("IsIdle"));
	IsIdleEntry.KeyType = IsIdle;
	IsIdleEntry.bInstanceSynced = false;

	FBlackboardEntry EnemyLocationEntry;
	EnemyLocationEntry.EntryName = FName(TEXT("EnemyLocation"));
	EnemyLocationEntry.KeyType = EnemyLocation;
	EnemyLocationEntry.bInstanceSynced = false;
	
	FBlackboardEntry OwnerCurrentLocationEntry;
	OwnerCurrentLocationEntry.EntryName = FName(TEXT("OwnerCurrentLocation"));
	OwnerCurrentLocationEntry.KeyType = OwnerCurrentLocation;
	OwnerCurrentLocationEntry.bInstanceSynced = false;
	
	Keys.Add(IsChaseEntry);
	Keys.Add(IsIdleEntry);
	Keys.Add(EnemyLocationEntry);
	Keys.Add(OwnerCurrentLocationEntry);
}
