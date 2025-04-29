// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "First/MySoueces/MySourcesType.h"
#include "GameFramework/Actor.h"
#include "MyAssets.generated.h"

//使用前向声明，避免循环依赖
class AFire;

UCLASS()
class FIRST_API AMyAssets : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyAssets();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool isPickedAsset();
	virtual void EnablePhysics();
	virtual void DisablePhysics();
	virtual void Throw(FVector StartLocation, FVector EndLocation, FQuat ActorRotation, bool HitOrNot);
	virtual EMySourceType GetSourceTypeName();
	virtual void SetHighLight();
	virtual void DisableHighLight();
	
	static TSubclassOf<AMyAssets> GetActorClassByType(EMySourceType ResourceType); //静态函数不会被子类继承
	static int32 GetResourceMaxStackNum(EMySourceType ResourceType); 
};
