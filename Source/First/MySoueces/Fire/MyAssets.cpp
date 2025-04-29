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
	//�麯����ʵ��
	return true;
}

void AMyAssets::EnablePhysics()
{
	//�麯����ʵ��
}

void AMyAssets::DisablePhysics()
{
	//�麯����ʵ��
}

void AMyAssets::Throw(FVector StartLocation, FVector EndLocation, FQuat ActorRotation, bool HitOrNot)
{
	//�麯����ʵ��
}

EMySourceType AMyAssets::GetSourceTypeName()
{
	//�麯����ʵ��
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
	//��̬��Ա����,������Դ�����ض�Ӧ����������ָ��
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
	//��̬��Ա����,������Դ�����ض�Ӧ����������ָ��
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