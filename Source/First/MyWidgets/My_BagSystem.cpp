// Fill out your copyright notice in the Description page of Project Settings.


#include "My_BagSystem.h"

#include "First/MyCharacter/EpicCharacter.h"
#include "First/MyCharacter/NPCBaseCharacter.h"
#include "First/MySoueces/BaseWorldAssets/BaseWorldAssets.h"
#include "First/MySoueces/Fire/MyAssets.h"

// Sets default values for this component's properties
UMy_BagSystem::UMy_BagSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> BagWidgetClass(TEXT("/Game/_project/Sources/Widgets/BagUI/BP_BagWidget"));
	if (BagWidgetClass.Succeeded())
	{
		BagClass = BagWidgetClass.Class;
	}
	
	static ConstructorHelpers::FClassFinder<UUserWidget> WorldStaticResourceWidgetClass(TEXT("/Game/_project/Sources/Widgets/BagUI/BP_OpenResourceWidget"));
	if (BagWidgetClass.Succeeded())
	{
		WorldStaticResourceClass = WorldStaticResourceWidgetClass.Class;
	}
	
	// BP_GridWidget的父类需设置为UMy_GridWidget
	static ConstructorHelpers::FClassFinder<UMy_GridWidget> GridWidgetClass(TEXT("/Game/_project/Sources/Widgets/BagUI/BP_GridWidget"));
	if (GridWidgetClass.Succeeded())
	{
		GridClass = GridWidgetClass.Class;
	}
	
}

void UMy_BagSystem::SetOwningCharacter(AEpicCharacter* Character)
{
	OwningCharacter = Character;
}

// Called when the game starts
void UMy_BagSystem::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

// Called every frame
void UMy_BagSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UMy_BagSystem::Init()
{
	// 实例化背包控件
	BagWidget = CreateWidget<UUserWidget>(GetWorld(), BagClass);
	GridContainer = Cast<UUniformGridPanel>(BagWidget->WidgetTree->FindWidget("GridContainer"));

	// 实例化世界资源箱控件
	WorldStaticResourceWidget = CreateWidget<UUserWidget>(GetWorld(), WorldStaticResourceClass);
	GridContainer2 = Cast<UUniformGridPanel>(WorldStaticResourceWidget->WidgetTree->FindWidget("GridContainer2"));
	
	// 默认创建8个格子
	CreateGridWidgets();
}

void UMy_BagSystem::CreateGridWidgets()		//创建 8个格子控件
{
	for (int i = 0; i < 8; ++i)
	{
		UMy_GridWidget* GridWidget = CreateWidget<UMy_GridWidget>(GetWorld(), GridClass);	//创建新的格子添加到布局,注意此时不会调用NativeConstruct，而是在添加到视图时才调用
		GridWidget->SetOwingBag(this, i);
		GridWidgetsArray.Add(GridWidget);
		GridResourceNameArray.Add(EMySourceType::NoneSource);
		GridContainer->AddChildToUniformGrid(GridWidget, i / 10, i % 10);	// 每行最多10个格子
	}

}

void UMy_BagSystem::BagUI_Show()
{
	// 添加到视口
	BagWidget->AddToViewport();
}

void UMy_BagSystem::BagUI_Hide()
{
	// 从视口移除
	BagWidget->RemoveFromParent();
}

bool UMy_BagSystem::AddResourceToBag(EMySourceType st, bool HandIsEmpty)
{
	if (st == EMySourceType::NoneSource)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Add Resource Error"));
		return false;
	}
	
	int32 ResourceStackNum = AMyAssets::GetResourceMaxStackNum(st);
	//添加资源的整体思路，先找正在使用的格子，如果它不为空或者相同资源，那就找背包里有没有格子存放该类型资源并且还有剩余容量，如果都没有那就找一个空格子存放
	if (CurrentGridIndex != -1)  //判断当前是否有正在使用的格子
	{
		if (GridWidgetsArray[CurrentGridIndex]->GetSourceTypeName() == EMySourceType::NoneSource)
		{
			//做为新资源添加，三个数组都要修改
			GridWidgetsArray[CurrentGridIndex]->SetSourceTypeName(st);
			GridWidgetsArray[CurrentGridIndex]->ModifyGridItemNum(1);
			GridWidgetsArray[CurrentGridIndex]->RefreshGrid();
			GridResourceNameArray[CurrentGridIndex] = st;
			ExistResourceGridIndexArray.Add(CurrentGridIndex);		//将该格子的索引添加到资源索引数组中
			return true;
		}
		if (GridWidgetsArray[CurrentGridIndex]->GetSourceTypeName() == st && GridWidgetsArray[CurrentGridIndex]->GetGridItemNum() < ResourceStackNum)
		{
			//作为同类资源添加，注意必须要有多余数量时才能添加资源，这里只需要修改格子数组内容
			GridWidgetsArray[CurrentGridIndex]->ModifyGridItemNum(1);
			GridWidgetsArray[CurrentGridIndex]->RefreshGrid();
			return true;
		}
	}
	
	// 遍历 ExistResourceGridIndexArray 判断是否作为同类资源添加
	for (int32 i = 0; i < ExistResourceGridIndexArray.Num(); ++i)
	{
		int32 index = ExistResourceGridIndexArray[i];	//只遍历已存在资源的格子，减少查找次数
		if (GridResourceNameArray[index] == st && GridWidgetsArray[index]->GetGridItemNum() < ResourceStackNum) 
		{
			GridWidgetsArray[index]->ModifyGridItemNum(1);
			GridWidgetsArray[index]->RefreshGrid();
			if (HandIsEmpty)
			{
				CurrentGridIndex = index; //记录当前存放资源的格子索引
				GridWidgetsArray[index]->SetHighlightImage(true);
				OwningCharacter->UpdateHandActorFromNull(st);
			}
			return true;
		}
	}

	// 否则做为新资源添加，这里从后往前找
	for (int32 i = GridResourceNameArray.Num() - 1; i >= 0; --i)
	{
		if (GridResourceNameArray[i] == EMySourceType::NoneSource)	//找到第一个 NoneSource 的位置
		{
			// 设置资源
			GridResourceNameArray[i] = st;
			GridWidgetsArray[i]->SetSourceTypeName(st);
			GridWidgetsArray[i]->ModifyGridItemNum(1);
			GridWidgetsArray[i]->RefreshGrid();
			
			ExistResourceGridIndexArray.Add(i);		//将该格子的索引添加到资源索引数组中
			if (HandIsEmpty)
			{
				CurrentGridIndex = i; //记录当前存放资源的格子索引
				GridWidgetsArray[i]->SetHighlightImage(true);
				OwningCharacter->UpdateHandActorFromNull(st);
			}
			return true;
		}
	}

	// 如果背包已满
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("背包已满"));
	}
	return false;
}

//按下 F丢弃当前资源
int32 UMy_BagSystem::ReduceResourceFromBag()
{
	if (CurrentGridIndex != -1)
	{
		GridWidgetsArray[CurrentGridIndex]->ModifyGridItemNum(-1);  //减少数量
		if (GridWidgetsArray[CurrentGridIndex]->GetGridItemNum() == 0)
		{
			//设置格子和资源数组该位置为空
			GridWidgetsArray[CurrentGridIndex]->SetSourceTypeName(EMySourceType::NoneSource);	
			GridResourceNameArray[CurrentGridIndex] = EMySourceType::NoneSource;
			ExistResourceGridIndexArray.Remove(CurrentGridIndex); //remove方法移除指定值的元素
		}
		GridWidgetsArray[CurrentGridIndex]->RefreshGrid();
		return GridWidgetsArray[CurrentGridIndex]->GetGridItemNum();	//返回该格子剩余的资源数量
	}
	return 0;
}

// 减少指定类型的资源（比如箭，子弹等）
bool UMy_BagSystem::ReduceSpecialResource(EMySourceType st)
{
	// 遍历所有存在资源的格子索引
	for (int32 i = 0; i < ExistResourceGridIndexArray.Num(); i++)
	{
		int32 GridIndex = ExistResourceGridIndexArray[i];
		if (GridResourceNameArray[GridIndex] == st)
		{
			GridWidgetsArray[GridIndex]->ModifyGridItemNum(-1);
			if (GridWidgetsArray[GridIndex]->GetGridItemNum() == 0)
			{
				GridWidgetsArray[GridIndex]->SetSourceTypeName(EMySourceType::NoneSource);
				GridResourceNameArray[GridIndex] = EMySourceType::NoneSource;
				ExistResourceGridIndexArray.RemoveAt(i); 
			}
			GridWidgetsArray[GridIndex]->RefreshGrid();
			return true;
		}
	}
	return false; 
}

EMySourceType UMy_BagSystem::WheelUp(int32 value)
{
	if (CurrentGridIndex != -1)
	{
		GridWidgetsArray[CurrentGridIndex]->SetHighlightImage(false);
	}
	
	CurrentGridIndex = (CurrentGridIndex - value + GridWidgetsArray.Num()) % GridWidgetsArray.Num();		//修改索引值
	GridWidgetsArray[CurrentGridIndex]->SetHighlightImage(true);
	
	return GridWidgetsArray[CurrentGridIndex]->GetSourceTypeName();
}

void UMy_BagSystem::SwapOrMergeResource(int32 DragIndex, int32 DropIndex)
{
	if (GridWidgetsArray[DragIndex]->GetSourceTypeName() == GridWidgetsArray[DropIndex]->GetSourceTypeName()) //如果是相同类型的资源那就合并
	{
		//思路是判断两个格子的资源数量相加有没有超过最大数量，如果没有那就合并为一个格子，如果超过了那就依然保留两个格子，只是修改数量而已
		int32 NumOfAll = GridWidgetsArray[DragIndex]->GetGridItemNum() + GridWidgetsArray[DropIndex]->GetGridItemNum();
		int32 MaxStatckNum = AMyAssets::GetResourceMaxStackNum(GridWidgetsArray[DropIndex]->GetSourceTypeName());
		if ( NumOfAll > MaxStatckNum )
		{
			GridWidgetsArray[DropIndex]->SetGridItemNum(MaxStatckNum);
			GridWidgetsArray[DragIndex]->SetGridItemNum(NumOfAll - MaxStatckNum);
		}
		else
		{
			GridWidgetsArray[DropIndex]->SetGridItemNum(NumOfAll);
			ExistResourceGridIndexArray.Remove(DragIndex);
			GridWidgetsArray[DragIndex]->SetGridItemNum(0);
			GridWidgetsArray[DragIndex]->SetSourceTypeName(EMySourceType::NoneSource);
			GridResourceNameArray[DragIndex] = EMySourceType::NoneSource;
		}
	}
	else
	{
		//如果要放入的格子是空的话,那就修改保存已存在资源的数组，这里已经确保drag格子不为空
		if (GridWidgetsArray[DropIndex]->GetSourceTypeName() == EMySourceType::NoneSource)		
		{	
			ExistResourceGridIndexArray.Remove(DragIndex);
			ExistResourceGridIndexArray.Add(DropIndex);
			if (CurrentGridIndex == DragIndex)	// 如果是将当前使用资源放到空位置，那就设置当前使用的格子到drop位置
			{
				GridWidgetsArray[CurrentGridIndex]->SetHighlightImage(false);
				CurrentGridIndex = DropIndex;
				GridWidgetsArray[CurrentGridIndex]->SetHighlightImage(true);
			}
			if (CurrentGridIndex == DropIndex)	// 如果是将其他资源放到当前格子，那就更新角色手中的物品
			{
				//让角色手中显示
				OwningCharacter->UpdateHandActorByDrag(GridWidgetsArray[DragIndex]->GetSourceTypeName());
			}
		}
		// 然后交换两个格子的内容
		EMySourceType tempType = GridWidgetsArray[DragIndex]->GetSourceTypeName();
		int32 tempNum = GridWidgetsArray[DragIndex]->GetGridItemNum();
		GridWidgetsArray[DragIndex]->SetSourceTypeName(GridWidgetsArray[DropIndex]->GetSourceTypeName());
		GridWidgetsArray[DragIndex]->SetGridItemNum(GridWidgetsArray[DropIndex]->GetGridItemNum());
		GridWidgetsArray[DropIndex]->SetSourceTypeName(tempType);
		GridWidgetsArray[DropIndex]->SetGridItemNum(tempNum);
		
		GridResourceNameArray.Swap(DragIndex, DropIndex);	//交换的是索引
	}
	
	GridWidgetsArray[DragIndex]->RefreshGrid();
	GridWidgetsArray[DropIndex]->RefreshGrid();
	
}


void UMy_BagSystem::ClearResourceOfDragGrid(int32 DragIndex)
{
	//清空当前格子的资源
	GridWidgetsArray[DragIndex]->SetSourceTypeName(EMySourceType::NoneSource);
	GridWidgetsArray[DragIndex]->SetGridItemNum(0);
	GridResourceNameArray[DragIndex] = EMySourceType::NoneSource;
	ExistResourceGridIndexArray.Remove(DragIndex);
	GridWidgetsArray[DragIndex]->RefreshGrid();

	if (CurrentGridIndex == DragIndex)
	{
		OwningCharacter->UpdateHandActorByDrag(EMySourceType::NoneSource);
	}
	//todo 把所有的物体扔出来，在世界中创建他们的实例
}


int32 UMy_BagSystem::GetCurrentGridIndex()
{
	return CurrentGridIndex;
}






























/************
	  世界资源部分NPC
 *************/
void UMy_BagSystem::ResourceGridInit(ANPCBaseCharacter* DropNPC)
{
	TArray<EMySourceType> ResourceTypeArray = DropNPC->GetDropResourceType();
	TArray<int32> ResourceNumArray = DropNPC->GetDropResourceNum();
	FActorResourceData SingleActorData;
	for (int i = 0; i < ResourceTypeArray.Num(); ++i)
	{
		SingleActorData.AddResource(ResourceTypeArray[i], ResourceNumArray[i]);  //实际保存
	}
	//保存，使用实例id唯一标识
	AllResourceDataMap.Add(DropNPC->GetUniqueID(), SingleActorData);
}

void UMy_BagSystem::OpenResourceBag(ANPCBaseCharacter* DropNPC, bool HandIsEmpty)	//创建资源箱中的格子控件
{
	NPCUniqueID = DropNPC->GetUniqueID();
	IsHandEmptyWhenOpen = HandIsEmpty;
	TArray<EMySourceType> ResourceTypeArray;
	TArray<int32> ResourceNumArray;
	
	// 检查全局资源列表中是否包含该 Actor的资源
	if (!AllResourceDataMap.Contains(NPCUniqueID))
	{
		ResourceGridInit(DropNPC);	//如果没有就先初始化然后再获取资源
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("no id"));
	}

	// 获取对应的资源数据
	FActorResourceData* ActorResourceData = AllResourceDataMap.Find(NPCUniqueID);
	for (int32 i = 0; i < ActorResourceData->TypeArray.Num(); ++i)
	{
		ResourceTypeArray.Add(ActorResourceData->TypeArray[i]);
		ResourceNumArray.Add(ActorResourceData->NumArray[i]);
	}
	
	for (int i = 0; i < DropNPC->GetGenerateGridNum(); ++i)	
	{
		UMy_GridWidget* GridWidget = CreateWidget<UMy_GridWidget>(GetWorld(), GridClass);	//创建新的格子
		GridContainer2->AddChildToUniformGrid(GridWidget, i / 10, i % 10);	// 每行最多10个格子
		GridWidget->SetRelativeBag(this, i + 100);
		if (i < ResourceTypeArray.Num())	//防止越界
		{
			GridWidget->SetSourceTypeName(ResourceTypeArray[i]);	//初始化格子中的资源与数量，做 ui显示
			GridWidget->SetGridItemNum(ResourceNumArray[i]);
			GridWidget->RefreshGrid();
		}
	}
	
	//ui显示
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();	// 获取玩家控制器
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
	PlayerController->bShowMouseCursor = true;	// 显示鼠标光标
	PlayerController->SetMouseLocation(ViewportSizeX / 2, ViewportSizeY / 2);	// 将鼠标位置设置到屏幕中心
	// 设置鼠标输入只与ui交互
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 锁定鼠标到视口
	PlayerController->SetInputMode(InputMode);
	WorldStaticResourceWidget->AddToViewport();
}


void UMy_BagSystem::RemoveResourceBagGrid()
{
	// 遍历并移除所有的子组件
	for (int32 i = GridContainer2->GetChildrenCount() - 1; i >= 0; --i)
	{
		UWidget* ChildWidget = GridContainer2->GetChildAt(i);
		if (ChildWidget != nullptr)
		{
			GridContainer2->RemoveChildAt(i);
		}
	}
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();	// 获取玩家控制器
	PlayerController->SetInputMode(FInputModeGameOnly());	//恢复输入模式
	PlayerController->bShowMouseCursor = false;	// 隐藏鼠标光标
	WorldStaticResourceWidget->RemoveFromParent();
	NPCUniqueID = -1;
}

int32 UMy_BagSystem::ClickResourcesToBag(int32 ClickIndex, EMySourceType st)
{
	bool flag = AddResourceToBag(st, IsHandEmptyWhenOpen);	//添加到背包
	if (flag)
	{
		ReduceResourceDataMap(ClickIndex, -1);
		IsHandEmptyWhenOpen = false;	//执行完成之后就不为空了
	}
	return flag;
}

int32 UMy_BagSystem::DragResourcesToBag(int32 DropIndex, EMySourceType st, int32 ResourceNum)
{
	//将世界中的资源拖动到背包格子中，思路是判断背包格子是不是空，如果是则直接添加（已经控制资源格子不会超过最大堆叠数量），否则的话判断是否是相同资源，如果不是不予执行
	if (GridResourceNameArray[DropIndex] == EMySourceType::NoneSource)
	{
		//做为新资源添加，三个数组都要修改
		GridWidgetsArray[DropIndex]->SetSourceTypeName(st);
		GridWidgetsArray[DropIndex]->SetGridItemNum(ResourceNum);
		GridWidgetsArray[DropIndex]->RefreshGrid();
		GridResourceNameArray[DropIndex] = st;
		ExistResourceGridIndexArray.Add(DropIndex);		//将该格子的索引添加到资源索引数组中
		if (CurrentGridIndex == DropIndex)
		{
			OwningCharacter->UpdateHandActorFromNull(st);
		}
		return 0;
	}
	if (GridWidgetsArray[DropIndex]->GetSourceTypeName() == st)	//是同类型资源
	{
		int32 NumOfAll = ResourceNum + GridWidgetsArray[DropIndex]->GetGridItemNum();
		int32 MaxStackNum = AMyAssets::GetResourceMaxStackNum(st);
		//如果总数量超过了最大数量就拿满，否则全拿
		if ( NumOfAll > MaxStackNum )
		{
			GridWidgetsArray[DropIndex]->SetGridItemNum(MaxStackNum);
			GridWidgetsArray[DropIndex]->RefreshGrid();
			return NumOfAll - MaxStackNum;
		}
		GridWidgetsArray[DropIndex]->SetGridItemNum(NumOfAll);
		GridWidgetsArray[DropIndex]->RefreshGrid();
		return 0;
	}
	//否则不予执行
	return ResourceNum;
}

bool UMy_BagSystem::ReduceResourceDataMap(int32 GridId, int32 UpdateNum)
{
	FActorResourceData* ActorResourceData = AllResourceDataMap.Find(NPCUniqueID);
	ActorResourceData->ReduceResource(GridId - 100,UpdateNum );
	IsHandEmptyWhenOpen = false;	//执行完成之后就不为空了
	return true;
}

void UMy_BagSystem::AddResourceDataMap(int32 DragGridId, int32 DropGridId, int32 UpdateNum, EMySourceType st)
{
	FActorResourceData* ActorResourceData = AllResourceDataMap.Find(NPCUniqueID);
	if (DragGridId > 99)
	{
		ActorResourceData->ReduceResource(DragGridId -100,-1 * UpdateNum);
		ActorResourceData->AddResource(DropGridId - 100, st,UpdateNum);
	}
	else
	{
		ActorResourceData->AddResource(DropGridId - 100, st,UpdateNum);
	}
}

void UMy_BagSystem::SetDragGrid(UMy_GridWidget* Drag)
{
	DragWidget = Drag;
}

void UMy_BagSystem::UpdateDragGrid(int32 RemainNum)
{
	//更新格子
	if (RemainNum != 0)
	{
		DragWidget->SetGridItemNum(RemainNum);
	}
	else
	{
		DragWidget->SetGridItemNum(0);
		DragWidget->SetSourceTypeName(EMySourceType::NoneSource);
	}
	DragWidget->RefreshGrid();
}














/************
	  世界资源部分资源箱
 *************/


void UMy_BagSystem::ClearActorID()
{
	NPCUniqueID = -1;
}

void UMy_BagSystem::ResourceGridInit(ABaseWorldAssets* DropNPC)
{
	TArray<EMySourceType> ResourceTypeArray = DropNPC->GetDropResourceType();
	TArray<int32> ResourceNumArray = DropNPC->GetDropResourceNum();
	FActorResourceData SingleActorData;
	for (int i = 0; i < ResourceTypeArray.Num(); ++i)
	{
		SingleActorData.AddResource(ResourceTypeArray[i], ResourceNumArray[i]);  //实际保存
	}
	//保存，使用实例id唯一标识
	AllResourceDataMap.Add(DropNPC->GetUniqueID(), SingleActorData);
}



FActorResourceData* UMy_BagSystem::OpenWorldAssetBag(ABaseWorldAssets* DropNPC, bool HandIsEmpty)
{
	NPCUniqueID = DropNPC->GetUniqueID();
	IsHandEmptyWhenOpen = HandIsEmpty;
	
	// 检查全局资源列表中是否包含该 Actor的资源
	if (!AllResourceDataMap.Contains(NPCUniqueID))
	{
		ResourceGridInit(DropNPC);	//如果没有就先初始化然后再获取资源
	}

	// 获取对应的资源数据
	FActorResourceData* ActorResourceData = AllResourceDataMap.Find(NPCUniqueID);
	return ActorResourceData;
	
}

//在合成的时候检测Actor是否有足够多的资源
bool UMy_BagSystem::CheckIfEnoughResource(EMySourceType st, int32 num)
{
	FActorResourceData* ActorResourceData = AllResourceDataMap.Find(NPCUniqueID);

	// 统计该资源类型的总数量
	int32 totalAmount = 0;
	for (int32 i = 0; i < ActorResourceData->TypeArray.Num(); ++i)
	{
		if (ActorResourceData->TypeArray[i] == st)
		{
			totalAmount += ActorResourceData->NumArray[i];
		}
	}

	return totalAmount >= num;
}

//有足够的资源用于合成，要减少相应的数量
void UMy_BagSystem::ReduceResourceByWork(EMySourceType st, int32 num)
{
	FActorResourceData* ActorResourceData = AllResourceDataMap.Find(NPCUniqueID);
	
	// 资源足够，开始逐个格子减少
	int32 remainingToReduce = num;
	for (int32 i = 0; i < ActorResourceData->TypeArray.Num() && remainingToReduce > 0; ++i)
	{
		if (ActorResourceData->TypeArray[i] == st)
		{
			int32 availableInSlot = ActorResourceData->NumArray[i];
			int32 reduceAmount = FMath::Min(availableInSlot, remainingToReduce);
            
			ActorResourceData->NumArray[i] -= reduceAmount;
			remainingToReduce -= reduceAmount;

			// 如果该格子资源耗尽，设置为NoneSource
			if (ActorResourceData->NumArray[i] == 0)
			{
				ActorResourceData->TypeArray[i] = EMySourceType::NoneSource;
			}
		}
	}
}

// 合成之后添加相应的资源数量到工作台
FActorResourceData* UMy_BagSystem::AddResourceByWork(EMySourceType st, int32 num)
{
	FActorResourceData* ActorResourceData = AllResourceDataMap.Find(NPCUniqueID);
	const int32 MaxStack = AMyAssets::GetResourceMaxStackNum(st);
	int32 remainingToAdd = num;

	// 尝试填充已有同类型资源的格子
	for (int32 i = 0; i < ActorResourceData->TypeArray.Num() && remainingToAdd > 0; ++i)
	{
		if (ActorResourceData->TypeArray[i] == st)
		{
			int32 availableSpace = MaxStack - ActorResourceData->NumArray[i];
			int32 addAmount = FMath::Min(availableSpace, remainingToAdd);	//实际添加的数量
            
			ActorResourceData->NumArray[i] += addAmount;
			remainingToAdd -= addAmount;
		}
	}

	// 如果还有剩余，填充NoneSource的空槽
	if (remainingToAdd > 0)
	{
		for (int32 i = 0; i < ActorResourceData->TypeArray.Num() && remainingToAdd > 0; ++i)
		{
			if (ActorResourceData->TypeArray[i] == EMySourceType::NoneSource)
			{
				int32 addAmount = FMath::Min(MaxStack, remainingToAdd);
                
				ActorResourceData->TypeArray[i] = st;
				ActorResourceData->NumArray[i] = addAmount;
				remainingToAdd -= addAmount;
			}
		}
	}

	// 如果还有剩余且数组未满，扩展数组添加新格子
	while (remainingToAdd > 0)
	{
		int32 addAmount = FMath::Min(MaxStack, remainingToAdd);
        
		ActorResourceData->TypeArray.Add(st);
		ActorResourceData->NumArray.Add(addAmount);
		remainingToAdd -= addAmount;
	}
	
	return ActorResourceData;
}

//点火的时候检查当前有多少特定的资源
int32 UMy_BagSystem::GetSpecialResourcesNum(EMySourceType st)
{
	const FActorResourceData* ActorResourceData = AllResourceDataMap.Find(NPCUniqueID);

	int32 TotalCount = 0;
	for (int32 i = 0; i < ActorResourceData->TypeArray.Num(); ++i)
	{
		if (ActorResourceData->TypeArray[i] == st)
		{
			TotalCount += ActorResourceData->NumArray[i];
		}
	}
	return TotalCount;
}

FActorResourceData* UMy_BagSystem::ReduceResourceByFire(EMySourceType st)
{
	FActorResourceData* ActorResourceData = AllResourceDataMap.Find(NPCUniqueID);
	for (int32 i = 0; i < ActorResourceData->TypeArray.Num(); ++i)
	{
		if (ActorResourceData->TypeArray[i] == st)
		{
			ActorResourceData->NumArray[i] = 0;
			ActorResourceData->TypeArray[i] = EMySourceType::NoneSource;
		}
	}
	return ActorResourceData;
}