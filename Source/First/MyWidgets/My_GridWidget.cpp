// Fill out your copyright notice in the Description page of Project Settings.


#include "My_GridWidget.h"

#include "My_BagSystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"

UTexture2D* UMy_GridWidget::DragGridImage = nullptr; // 静态变量初始化
int32 UMy_GridWidget::DragGridItemNum = 0; // 静态变量初始化
int32 UMy_GridWidget::DragGridId = 0; // 静态变量初始化
EMySourceType UMy_GridWidget::DragGridType = EMySourceType::NoneSource; // 静态变量初始化

void UMy_GridWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 为格子中的GridButton添加事件
	GridButton->OnClicked.AddDynamic(this, &UMy_GridWidget::OnButtonClicked);
}

void UMy_GridWidget::SetRelativeBag(UMy_BagSystem* Bag, int32 Index)
{
	RelativeBag = Bag;
	GridId = Index;
}

// 设置所属背包，同时设置在背包中的id，只有角色的背包才有id
void UMy_GridWidget::SetOwingBag(UMy_BagSystem* Bag, int32 Index)
{
	OwingBag = Bag;
	GridId = Index;
}


void UMy_GridWidget::HideImage()
{
	
	if (GridItemImage->Visibility == ESlateVisibility::Hidden)
	{
		GridItemImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		GridItemImage->SetVisibility(ESlateVisibility::Hidden);
	}	
	
	GridItemNum->SetText(FText::AsNumber(ItemNum));
}

void UMy_GridWidget::SetHighlightImage(bool bHighlight)
{
	if (bHighlight)
	{
		UTexture2D* HighlightImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/_project/Sources/Widgets/BagUI/Image/HighlightGridImage"));
		GridImage->SetBrushFromTexture(HighlightImage);
	}
	else
	{
		UTexture2D* NormalImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/_project/Sources/Widgets/BagUI/Image/GridImage"));
		GridImage->SetBrushFromTexture(NormalImage);
	}
}

void UMy_GridWidget::SetPreselectImage()
{
	UTexture2D* HighlightImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/_project/Sources/Widgets/BagUI/Image/PreselectGridImage"));
	GridImage->SetBrushFromTexture(HighlightImage);
}

void UMy_GridWidget::SetImage(EMySourceType st)
{
	UMySourcesType* GridSourceType = NewObject<UMySourcesType>(this, UMySourcesType::StaticClass());
	if (GridSourceType)
	{
		//根据类别获取图像并设置
		UTexture2D*	itemImage = GridSourceType->GetImageByType(st);
		GridItemImage->SetBrushFromTexture(itemImage);
	}
}

FReply UMy_GridWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	// 检查是否为左键点击
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	
	if (GridId > 99 && SourceTypeName != EMySourceType::NoneSource)	//只有非空的资源格子才能触发点击事件
	{
		if (RelativeBag->ClickResourcesToBag(GridId,SourceTypeName))	//更新资源表中的数据
		{
			if (--ItemNum == 0)
			{
				SourceTypeName = EMySourceType::NoneSource;
			}
			RefreshGrid();
		}
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NativeOnMouseButtonDown"));
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}


void UMy_GridWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (this->SourceTypeName != EMySourceType::NoneSource)//只有非空格子才让拖动,此函数仅做ui显示
	{
		Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
		
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NativeOnDragDetected"));
		DragGridId = GridId;	//设置静态变量值为当前格子的id
		if (GridId > 99)
		{
			RelativeBag->SetDragGrid(this);
		}
		
		OutOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
		// 保存当前格子的img与text
		DragGridImage = Cast<UTexture2D>(GridItemImage->GetBrush().GetResourceObject());
		DragGridItemNum = ItemNum;
		DragGridType = SourceTypeName;
	
		// 创建一个Widget作为拖拽ui，使用已经创建好的的BP_DragUI
		UUserWidget* DragUIWidget = CreateWidget<UUserWidget>(GetWorld(),
			LoadClass<UUserWidget>(nullptr, TEXT("/Game/_project/Sources/Widgets/BagUI/BP_DragUI.BP_DragUI_C")));

		// 设置拖拽Widget的img与text为当前格子的内容
		UImage* dragImage = Cast<UImage>(DragUIWidget->WidgetTree->FindWidget("GridItemImage"));
		UTextBlock* dragText = Cast<UTextBlock>(DragUIWidget->WidgetTree->FindWidget("GridItemNum"));
		dragImage->SetBrushFromTexture(DragGridImage);
		FString ItemNumText = FString::Printf(TEXT("%d"), DragGridItemNum);
		dragText->SetText(FText::FromString(ItemNumText));
		
		OutOperation->DefaultDragVisual = DragUIWidget;	//使用拖拽ui控件
		OutOperation->Payload = this;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("notallowDrag"));
	}
}

//调用的是放入的格子的drop方法
bool UMy_GridWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (GridId < 99 && OwingBag != nullptr) //从资源格子拖动到背包格子
	{
		if (DragGridId > 99)
		{
			int32 RemainNum = OwingBag->DragResourcesToBag(GridId, DragGridType, DragGridItemNum);
			if (RemainNum != DragGridItemNum)	//数量有变化
			{
				OwingBag->ReduceResourceDataMap(DragGridId, RemainNum - DragGridItemNum);
				OwingBag->UpdateDragGrid(RemainNum);
			}
		}
		else if (DragGridId < 99 && DragGridId != GridId)
		{
			OwingBag->SwapOrMergeResource(DragGridId,GridId);
		}
		return true;
	}
	if (GridId > 99 && RelativeBag != nullptr)	//接收的格子是资源格子,必须为空
	{
		if (SourceTypeName == EMySourceType::NoneSource)
		{
			//更新当前格子的ui和资源数量
			SourceTypeName = DragGridType;
			ItemNum = DragGridItemNum;
			RefreshGrid();
			RelativeBag->AddResourceDataMap(DragGridId,GridId,DragGridItemNum,DragGridType);
			if (DragGridId > 99 )
			{
				RelativeBag->UpdateDragGrid(0);
			}
			if (DragGridId < 99)
			{
				RelativeBag->ClearResourceOfDragGrid(DragGridId);
			}
		}
		
	}
	
	this->AddToViewport();
	FVector2D NewPosition = InDragDropEvent.GetScreenSpacePosition();
	SetPositionInViewport(NewPosition, false);
	return true;
}

void UMy_GridWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
    
	// 输出拖拽取消信息
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("NativeOnDragCancelled"));
	}
	if (DragGridId != -1 && DragGridId < 99)
	{
		OwingBag->ClearResourceOfDragGrid(DragGridId);
	}
	
	// 在此处添加你的自定义逻辑
}

void UMy_GridWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    
	// 只有背包当前使用的格子不能调用该方法
	if (OwingBag && OwingBag->GetCurrentGridIndex() == GridId)
	{
		return;
	}
	SetPreselectImage();
}

void UMy_GridWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
    
	if (OwingBag && OwingBag->GetCurrentGridIndex() == GridId)
	{
		return;
	}
	SetHighlightImage(false);
}

void UMy_GridWidget::SetSourceTypeName(EMySourceType st) 
{
	SourceTypeName = st;
}

EMySourceType UMy_GridWidget::GetSourceTypeName() 
{
	return SourceTypeName;
}

void UMy_GridWidget::ModifyGridItemNum(int32 num) 
{
	ItemNum += num;
}

int32 UMy_GridWidget::GetGridItemNum()
{
	return ItemNum;
}

void UMy_GridWidget::SetGridItemNum(int32 num) 
{
	ItemNum = num;
}

void UMy_GridWidget::OnButtonClicked()
{
	HideImage();
}

void UMy_GridWidget::RefreshGrid()
{
	SetImage(SourceTypeName);
	GridItemNum->SetText(FText::AsNumber(ItemNum));
}