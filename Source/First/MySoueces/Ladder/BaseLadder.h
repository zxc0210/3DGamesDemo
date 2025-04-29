// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseLadder.generated.h"

class UBoxComponent;

UCLASS()
class FIRST_API ABaseLadder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseLadder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	virtual void OnConstruction(const FTransform& Transform) override;
	
	FVector GetNormal() const;
	void SetLadderRungsNum(int32 num);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder", meta = (ClampMin = "1"))
	int32 RungsCount = 1;	// 梯子节数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder", meta = (ClampMin = "10"))
	float SupportDistance = 100.0f;		// 左右梯子距离
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder", meta = (ClampMin = "10"))
	float SupportHeight = 60.0f;	// 每节梯子的高度
	float LadderHeight = 60.0f;		// 梯子总高度
	const FVector SupportScale = FVector(0.1f, 0.1f, 0.0f);	// 尺寸
	const FVector RungScale = FVector(0.1f, 0.1f, 0.0f);

	FVector GetClosestRungsLocation(FVector CharacterLocation ,bool isNext);
	
private:
	// 网格
	UPROPERTY()
	UStaticMesh* CubeMesh;
	UPROPERTY()
	UStaticMesh* CylinderMesh;
	UPROPERTY()
	class USceneComponent* RootSceneComponent;
	
	UPROPERTY()
	TArray<UStaticMeshComponent*> LeftSupports;
	UPROPERTY()
	TArray<UStaticMeshComponent*> RightSupports;
	UPROPERTY()
	TArray<UStaticMeshComponent*> Rungs;
	
	//碰撞体
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* LadderCollision;
	
	void CreateLadderComponents();
	void ClearLadderComponents();
	void SetLadderCollision();
};

