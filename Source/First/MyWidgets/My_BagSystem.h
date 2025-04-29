// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "My_GridWidget.h"

#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blueprint/WidgetTree.h"
#include "My_BagSystem.generated.h"

class ABaseWorldAssets;
//使用角色类的前向声明，避免循环依赖
class AEpicCharacter;
class ANPCBaseCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIRST_API UMy_BagSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMy_BagSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
/************
	控件UI，包括背包控件与格子控件
 *************/	
	UPROPERTY(EditAnywhere, Category = "BagSystem")
	TSubclassOf<UUserWidget> BagClass;
	UPROPERTY()
	UUserWidget* BagWidget;				//整个背包控件
	// 可以使用BindWidget绑定蓝图控件，注意变量名和蓝图中控件的名字必须匹配
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* GridContainer;
	
	// 格子
	UPROPERTY(EditAnywhere, Category = "BagSystem")
	TSubclassOf<UMy_GridWidget> GridClass;

/************
	 游戏中的物资箱类
 *************/
	UPROPERTY(EditAnywhere, Category = "BagSystem")
	TSubclassOf<UUserWidget> WorldStaticResourceClass;	
	UPROPERTY()
	UUserWidget* WorldStaticResourceWidget;	
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* GridContainer2;
	
/************
	  成员变量与初始化函数
 *************/
	AEpicCharacter* OwningCharacter;		// 所属角色的指针
	
	UMy_GridWidget* DragWidget;
		
	TArray<UMy_GridWidget*> GridWidgetsArray;  //在Array中保存所有的格子
	TArray<EMySourceType> GridResourceNameArray;	//资源名称数组，与格子数组一一对应
	TArray<int32> ExistResourceGridIndexArray;	//存在资源的格子索引数组
	int32 CurrentGridIndex = -1;	//当前使用的资源所在格子的索引
	
	int32 NPCUniqueID;
	bool IsHandEmptyWhenOpen;
	TMap<int32, FActorResourceData> AllResourceDataMap;		//全局资源掉落列表,存放已经打开过的所有Actor的资源
	
	void Init();
	void CreateGridWidgets();
	

public:
/************
	  公有函数
 *************/
	void SetOwningCharacter(AEpicCharacter* Character);
	int32 GetCurrentGridIndex();
	void BagUI_Show();
	void BagUI_Hide();
	
	bool AddResourceToBag(EMySourceType st, bool HandIsEmpty);
	int32 ReduceResourceFromBag();
	bool ReduceSpecialResource(EMySourceType st);

	EMySourceType WheelUp(int32 value);
	
	void SwapOrMergeResource(int32 DragIndex, int32 DropIndex);
	void ClearResourceOfDragGrid(int32 DragIndex);
	
	
	void OpenResourceBag(ANPCBaseCharacter* DropNPC, bool HandIsEmpty);
	void ResourceGridInit(ANPCBaseCharacter* ResourceDropNPC);
	void RemoveResourceBagGrid();
	int32 ClickResourcesToBag(int32 ClickIndex, EMySourceType st);
	int32 DragResourcesToBag(int32 DropIndex, EMySourceType st, int32 ResourceNum);
	bool ReduceResourceDataMap(int32 GridId,int32 UpdateNum);
	void AddResourceDataMap(int32 DragGridId, int32 DropGridId, int32 UpdateNum, EMySourceType st);
	void SetDragGrid(UMy_GridWidget* Drag);
	void UpdateDragGrid(int32 RemainNum);

	//世界资源箱部分
	void ClearActorID();
	void ResourceGridInit(ABaseWorldAssets* DropNPC);
	FActorResourceData* OpenWorldAssetBag(ABaseWorldAssets* DropNPC, bool HandIsEmpty);
	bool CheckIfEnoughResource(EMySourceType st, int32 num);
	void ReduceResourceByWork(EMySourceType st, int32 num);
	FActorResourceData* AddResourceByWork(EMySourceType st, int32 num);
	int32 GetSpecialResourcesNum(EMySourceType st);
	FActorResourceData* ReduceResourceByFire(EMySourceType st);
};

