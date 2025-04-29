// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "First/MySoueces/MySourcesType.h"
#include "GameFramework/Actor.h"
#include "BaseWorldAssets.generated.h"


UCLASS()
class FIRST_API ABaseWorldAssets : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWorldAssets();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	
	virtual void UI_Show();
	virtual void UI_Hide();
	virtual void UI_Update(FActorResourceData* ActorResourceData);
	virtual TArray<EMySourceType> GetDropResourceType();
	virtual TArray<int32> GetDropResourceNum();
	virtual void SetHighLight();
	virtual void DisableHighLight();
	
protected:
	TArray<EMySourceType> GenerateResourceType = {};		//掉落的资源类型
	TArray<int32> DropResourceNumArray = {};
	TArray<EMySourceType> DropResourceTypeArray = {};
};
