// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "MySourcesType.generated.h"

UENUM(BlueprintType)
enum class EMySourceType : uint8
{
	NoneSource   UMETA(DisplayName = "NoneSource"),
	Bow     UMETA(DisplayName = "Bow"),
	Arrow     UMETA(DisplayName = "Arrow"),
	Axe     UMETA(DisplayName = "Axe"),
	Fire     UMETA(DisplayName = "Fire"),
	Sabre     UMETA(DisplayName = "Sabre"),
	Plank     UMETA(DisplayName = "Plank"),
	Iron     UMETA(DisplayName = "Iron"),
};

USTRUCT(BlueprintType)
struct FActorResourceData		
{
	GENERATED_BODY()
	
	// 资源类型数组
	UPROPERTY(BlueprintReadWrite)
	TArray<EMySourceType> TypeArray;

	// 资源数量数组
	UPROPERTY(BlueprintReadWrite)
	TArray<int32> NumArray;

	// 添加资源
	void AddResource(EMySourceType ResourceType, int32 Quantity)
	{
		TypeArray.Add(ResourceType);
		NumArray.Add(Quantity);
	}

	// 减少资源
	void ReduceResource(int32 index, int32 Quantity)
	{
		NumArray[index] += Quantity;
		if (NumArray[index] == 0)
		{
			TypeArray[index] = EMySourceType::NoneSource;
			NumArray[index] = 0;
		}
	}
	void AddResource(int32 index, EMySourceType ResourceType, int32 Quantity)
	{
		if (TypeArray.Num() < index + 1)
		{
			while (TypeArray.Num() < index)	//长度不够,先扩充
			{
				TypeArray.Add(EMySourceType::NoneSource);
				NumArray.Add(0);
			}
			TypeArray.Add(ResourceType);
			NumArray.Add(Quantity);
		}
		else
		{
			TypeArray[index] = ResourceType;
			NumArray[index] = Quantity;
		}
	}
};

// 合成配方结构
USTRUCT(BlueprintType)
struct FRecipe:public FTableRowBase  // 用 DataTable 存储配方
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMySourceType Target;	// 合成目标
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EMySourceType, int32> Material; // 所需材料类型与数量
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity;	//一次合成生成的数量
	
};


UCLASS()
class FIRST_API UMySourcesType : public UObject
{
	GENERATED_BODY()

	
public:
	UMySourcesType();
	static UMySourcesType* GetInstance();	// 全局单例获取函数

	void InitializeSources();
	
	UTexture2D* GetImageByType(EMySourceType E_SourceType);
	
private:
	// 图片资源
	UTexture2D* NullGridImage;
	UTexture2D* BowImage;
	UTexture2D* ArrowImage;
	UTexture2D* AxeImage;
	UTexture2D* FireImage;
	UTexture2D* SabreImage;
	UTexture2D* PlankImage;
	UTexture2D* IronImage;

	// 保存输出图像与类型
	UTexture2D* SourceImage;  
};
