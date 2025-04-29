// Fill out your copyright notice in the Description page of Project Settings.


#include "Boiler.h"

#include "BoilerWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Uobject/ConstructorHelpers.h"

// Sets default values
ABoiler::ABoiler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//网格体
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticM(
		TEXT("/Game/_project/Sources/Boiler/SM_OilTank.SM_OilTank"));
	StaticMC_Boiler = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("smc_Bolier"));
	StaticMC_Boiler->SetStaticMesh(StaticM.Object);
	SetRootComponent(StaticMC_Boiler);
	StaticMC_Boiler->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	StaticMC_Boiler->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMC_Boiler->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMC_Boiler->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 保留与地面的碰撞
	StaticMC_Boiler->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block); // 忽略角色碰撞
	
	static ConstructorHelpers::FClassFinder<UBoilerWidget> WidgetClass(TEXT("/Game/_project/Sources/Widgets/Boiler/BP_BoilerWidget"));
	if (WidgetClass.Succeeded())
	{
		TestClass = WidgetClass.Class;
	}
	
	GenerateResourceType = { 	EMySourceType::Plank, EMySourceType::Fire };	
}

// Called when the game starts or when spawned
void ABoiler::BeginPlay()
{
	Super::BeginPlay();
	
	BoilWidget = CreateWidget<UBoilerWidget>(GetWorld(), TestClass);	//创建新的控件添加到布局,注意此时不会调用NativeConstruct，而是在添加到视图时才调用
	BoilWidget->Init();	//这里因为还没有设置材质所以可以在添加视口之间就init
}

// Called every frame
void ABoiler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoiler::UI_Update(FActorResourceData* ActorResourceData)
{
	BoilWidget->Update(ActorResourceData);
}

void ABoiler::UI_Show()
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
	
	BoilWidget->AddToViewport();
}

void ABoiler::UI_Hide()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();	// 获取玩家控制器
	PlayerController->SetInputMode(FInputModeGameOnly());	//恢复输入模式
	PlayerController->bShowMouseCursor = false;	// 隐藏鼠标光标
	BoilWidget->RemoveFromParent();
}

void ABoiler::SetHighLight()
{
	StaticMC_Boiler->SetRenderCustomDepth(true);
}

void ABoiler::DisableHighLight()
{
	StaticMC_Boiler->SetRenderCustomDepth(false);
}
