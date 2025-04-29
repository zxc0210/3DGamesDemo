// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "First/MySoueces/BaseWorldAssets/BaseWorldAssets.h"
#include "GameFramework/Actor.h"
#include "WorkTable.generated.h"

class UWorkTableWidget;

UCLASS()
class FIRST_API AWorkTable : public ABaseWorldAssets
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorkTable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	void UI_Update(FActorResourceData* ActorResourceData) override;
	void UI_Show() override;
	void UI_Hide() override;
	void SetHighLight() override;
	void DisableHighLight() override;
	
protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SM_WorkTable;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UWorkTableWidget> WorkTableClass;
	UPROPERTY()
	UWorkTableWidget* WorkTableWidget;
	
};
