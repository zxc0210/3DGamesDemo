// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkTableWidget.h"

#include "TableItemWidget.h"
#include "First/MyCharacter/EpicCharacter.h"

void UWorkTableWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

UWorkTableWidget* UWorkTableWidget::GetInstance() //单例
{
	static UWorkTableWidget* Instance = NewObject<UWorkTableWidget>();
	return Instance;
}

void UWorkTableWidget::Init()
{
	if (bHasExecuted) return;
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();	// 获取玩家控制器
	ACharacter* ControlledCharacter = PlayerController ? Cast<ACharacter>(PlayerController->GetPawn()) : nullptr;
	AEpicCharacter * CurrentCharacter = Cast<AEpicCharacter>(ControlledCharacter);
	//Item类
	UClass* ItemWidgetClass = LoadClass<UTableItemWidget>(nullptr, TEXT("/Game/_project/Sources/Widgets/WorkTable/BP_TableItemsWidget.BP_TableItemsWidget_C"));
	ItemClass = ItemWidgetClass;
	for (int i = 0; i < UTableItemWidget::GetCount(); ++i)	
	{
		UTableItemWidget* ItemWidget = CreateWidget<UTableItemWidget>(GetWorld(), ItemClass);
		ItemContainer->AddChildToUniformGrid(ItemWidget, i / 4, i % 4);
		ItemWidget->SetIndex(i,CurrentCharacter->BagSystem,this);
	}
	
	//格子类
	UClass* GridWidgetClass = LoadClass<UMy_GridWidget>(nullptr, TEXT("/Game/_project/Sources/Widgets/BagUI/BP_GridWidget.BP_GridWidget_C"));
	GridClass = GridWidgetClass;
	for (int i = 0; i < 12; ++i)	
	{
		UMy_GridWidget* GridWidget = CreateWidget<UMy_GridWidget>(GetWorld(), GridClass);	//创建新的格子
		GridContainer->AddChildToUniformGrid(GridWidget, i / 6, i % 6);	// 每行最多4个格子
		GridWidget->SetRelativeBag(CurrentCharacter->BagSystem, i + 100);
		GridWidgetsArray.Add(GridWidget);
	}
	bHasExecuted = true;
}


void UWorkTableWidget::Update(FActorResourceData* ActorResourceData)
{
	TArray<EMySourceType> ResourceTypeArray;
	TArray<int32> ResourceNumArray;
	for (int32 i = 0; i < ActorResourceData->TypeArray.Num(); ++i)
	{
		ResourceTypeArray.Add(ActorResourceData->TypeArray[i]);
		ResourceNumArray.Add(ActorResourceData->NumArray[i]);
	}
	for (int i = 0; i < 12; ++i)	
	{
		GridWidgetsArray[i]->SetHighlightImage(false);
		if (i < ResourceTypeArray.Num())	//防止越界
		{
			GridWidgetsArray[i]->SetSourceTypeName(ResourceTypeArray[i]);	//初始化格子中的资源与数量，做 ui显示
			GridWidgetsArray[i]->SetGridItemNum(ResourceNumArray[i]);
			GridWidgetsArray[i]->RefreshGrid();
		}
	}
	
}
