// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAssets.h"

#include "Fire.h"
#include "First/MySoueces/Iron/Iron.h"
#include "First/MySoueces/Plank/Plank.h"
#include "First/MySoueces/Weapon/Bow/My_Bow.h"
#include "First/MySoueces/Weapon/Sabre/Sabre.h"

// Sets default values
AMyAssets::AMyAssets()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyAssets::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyAssets::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AMyAssets::isPickedAsset()
{
	//虚函数空实现
	return true;
}

void AMyAssets::EnablePhysics()
{
	//虚函数空实现
}

void AMyAssets::DisablePhysics()
{
	//虚函数空实现
}

void AMyAssets::Throw(FVector StartLocation, FVector EndLocation, FQuat ActorRotation, bool HitOrNot)
{
	//虚函数空实现
}

EMySourceType AMyAssets::GetSourceTypeName()
{
	//虚函数空实现
	return EMySourceType::NoneSource;
}

void AMyAssets::SetHighLight()
{
	
}

void AMyAssets::DisableHighLight()
{
	
}


TSubclassOf<AMyAssets> AMyAssets::GetActorClassByType(EMySourceType ResourceType)
{
	//静态成员函数,根据资源名返回对应的子类类型指针
	switch (ResourceType)
	{
	case EMySourceType::Axe:
		return nullptr;
	case EMySourceType::Bow:
		return AMy_Bow::StaticClass();
	case EMySourceType::Arrow:
		return AMy_Arrow::StaticClass();
	case EMySourceType::Fire:
		return AFire::StaticClass();
	case EMySourceType::Sabre:
		return ASabre::StaticClass();
	case EMySourceType::Plank:
		return APlank::StaticClass();
	case EMySourceType::Iron:
		return AIron::StaticClass();
	default:
		return nullptr;
	}
}

int32 AMyAssets::GetResourceMaxStackNum(EMySourceType ResourceType)
{
	//静态成员函数,根据资源名返回对应的子类类型指针
	switch (ResourceType)
	{
	case EMySourceType::Fire:
		return AFire::GetMaxStackNum();
	case EMySourceType::Plank:
		return APlank::GetMaxStackNum();
	case EMySourceType::Iron:
		return AIron::GetMaxStackNum();
	case EMySourceType::Arrow:
		return AMy_Arrow::GetMaxStackNum();
	default:
		return 1;
	}
}