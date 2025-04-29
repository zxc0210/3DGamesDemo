// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkTable.h"

#include "First/MyWidgets/WorkTable/WorkTableWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Uobject/ConstructorHelpers.h"

// Sets default values
AWorkTable::AWorkTable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//网格体
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sm_Table(
		TEXT("/Game/_project/Sources/WorkTable/SM_Table.SM_Table"));
	SM_WorkTable = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("smc_Table"));
	SM_WorkTable->SetStaticMesh(sm_Table.Object);
	SetRootComponent(SM_WorkTable);
	SM_WorkTable->SetWorldScale3D(FVector(1.3f, 1.3f, 1.3f));
	SM_WorkTable->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SM_WorkTable->SetCollisionResponseToAllChannels(ECR_Ignore);
	SM_WorkTable->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 保留与地面的碰撞
	SM_WorkTable->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block); // 忽略角色碰撞
	
	static ConstructorHelpers::FClassFinder<UWorkTableWidget> WidgetClass(TEXT("/Game/_project/Sources/Widgets/WorkTable/BP_WorkTableWidget"));
	if (WidgetClass.Succeeded())
	{
		WorkTableClass = WidgetClass.Class;
	}
}

// Called when the game starts or when spawned
void AWorkTable::BeginPlay()
{
	Super::BeginPlay();
	
	WorkTableWidget = CreateWidget<UWorkTableWidget>(GetWorld(), WorkTableClass);	//创建新的控件添加到布局,注意此时不会调用NativeConstruct，而是在添加到视图时才调用
	WorkTableWidget ->Init(); //必须在这调用，否则会有属性未初始化
}

// Called every frame
void AWorkTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWorkTable::UI_Update(FActorResourceData* ActorResourceData)
{
	WorkTableWidget->Update(ActorResourceData);
}

void AWorkTable::UI_Show()
{
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
	
	WorkTableWidget->AddToViewport();
	
}

void AWorkTable::UI_Hide()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();	// 获取玩家控制器
	PlayerController->SetInputMode(FInputModeGameOnly());	//恢复输入模式
	PlayerController->bShowMouseCursor = false;	// 隐藏鼠标光标
	WorkTableWidget->RemoveFromParent();
}

void AWorkTable::SetHighLight()
{
	SM_WorkTable->SetRenderCustomDepth(true);
}

void AWorkTable::DisableHighLight()
{
	SM_WorkTable->SetRenderCustomDepth(false);
}
