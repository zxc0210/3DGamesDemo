// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseLadder.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceConstant.h"
#include "UObject/ConstructorHelpers.h"

ABaseLadder::ABaseLadder()
{
    PrimaryActorTick.bCanEverTick = false;
    
    // 创建并设置根组件
    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = RootSceneComponent;
    
    // 创建 Box 碰撞组件
    LadderCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LadderCollision"));
    LadderCollision->SetupAttachment(RootComponent);
    // LadderCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    // LadderCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    
    static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> LadderMaterialFinder(TEXT("/Game/_project/Character/Boat/Ship/Material_Instances/MI_Ladder_02"));
    UMaterialInstanceConstant* LadderMaterial = nullptr;
    if (LadderMaterialFinder.Succeeded())
    {
        LadderMaterial = LadderMaterialFinder.Object;
    }
    
        
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
    if (CubeMeshFinder.Succeeded())
    {
        CubeMesh = CubeMeshFinder.Object;
        CubeMesh->SetMaterial(0, LadderMaterial);
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshFinder(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
    if (CylinderMeshFinder.Succeeded())
    {
        CylinderMesh = CylinderMeshFinder.Object;
        CylinderMesh->SetMaterial(0, LadderMaterial);
    }
}

void ABaseLadder::BeginPlay()
{
    Super::BeginPlay();
    CreateLadderComponents();
 
}

void ABaseLadder::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABaseLadder::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    CreateLadderComponents();
}

FVector ABaseLadder::GetNormal() const
{
    // 梯子的法线方向
    return GetActorForwardVector();
}

void ABaseLadder::SetLadderRungsNum(int32 num)
{
    if (num < 1)
    {
        num = 1;
    }
    RungsCount = num;
    CreateLadderComponents();
}


void ABaseLadder::CreateLadderComponents()
{
    LadderHeight = RungsCount * SupportHeight;    // 计算梯子总高度
    
    ClearLadderComponents();
    SetLadderCollision();
    
    // 创建左右支撑柱
    for (int32 i = 0; i < 2; i++) // 只需要左右两根支撑柱
    {
        FString SupportName = (i == 0) ? "LeftSupport" : "RightSupport";
        UStaticMeshComponent* Support = NewObject<UStaticMeshComponent>(this, FName(*SupportName));
        Support->SetupAttachment(RootComponent);
        Support->SetStaticMesh(CubeMesh);
        
        (i == 0)? LeftSupports.Add(Support): RightSupports.Add(Support);//添加
  
        Support->SetRelativeScale3D(FVector(SupportScale.X, SupportScale.Y, LadderHeight / 100.0f)); // 高度根据梯子总高度调整，,这里的100是立方体的原本高度
        // 设置支撑柱尺寸和位置
        float YOffset = (i == 0) ? -SupportDistance / 2.0f : SupportDistance / 2.0f;
        Support->SetRelativeLocation(FVector(0.0f, YOffset, LadderHeight / 2.0f)); // 居中放置
        
        Support->RegisterComponent();
    }

    // 创建横档
    for (int32 i = 0; i < RungsCount; i++)
    {
        FString RungName = FString::Printf(TEXT("Rung_%d"), i);
        UStaticMeshComponent* Rung = NewObject<UStaticMeshComponent>(this, FName(*RungName));
        Rung->SetupAttachment(RootComponent);
        Rung->SetStaticMesh(CylinderMesh);
        
        // 设置横档位置和旋转
        float ZPosition = i * SupportHeight + (SupportHeight / 2.0f);
        Rung->SetRelativeScale3D(FVector(RungScale.X, RungScale.Y , SupportDistance / 100.0f)); // 长度匹配支撑距离,这里的100是圆柱的原本高度
        Rung->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f)); // 水平放置
        Rung->SetRelativeLocation(FVector(0.0f, 0.0f, ZPosition));
        
        Rung->RegisterComponent();
        Rungs.Add(Rung);

        // 注册组件后获取世界位置
        FVector WorldPosition = Rung->GetComponentLocation();
        
    }
}

void ABaseLadder::ClearLadderComponents()
{
    // 清除现有组件
    for (UStaticMeshComponent* Support : LeftSupports)
    {
        if (Support)
        {
            Support->DestroyComponent();
        }
    }
    LeftSupports.Empty();

    for (UStaticMeshComponent* Support : RightSupports)
    {
        if (Support)
        {
            Support->DestroyComponent();
        }
    }
    RightSupports.Empty();

    for (UStaticMeshComponent* Rung : Rungs)
    {
        if (Rung)
        {
            Rung->DestroyComponent();
        }
    }
    Rungs.Empty();
}

void ABaseLadder::SetLadderCollision()
{
    // 设置碰撞体的大小
    FVector CollisionSize(200.0f, 3.0f * SupportDistance, LadderHeight);
    LadderCollision->SetBoxExtent(CollisionSize / 2.0f);

    // 设置碰撞体的位置，使其居中覆盖整个梯子
    FVector CollisionLocation(0.0f, 0.0f, LadderHeight / 2.0f);
    LadderCollision->SetRelativeLocation(CollisionLocation);
    
    LadderCollision->RegisterComponent();
}


FVector ABaseLadder::GetClosestRungsLocation(FVector CharacterLocation ,bool isNext)
{
    float FinalZValue = 0;
    UStaticMeshComponent* ClosestRung = nullptr;
    float MinDistance = FLT_MAX;

    int32 Index = 0;
    for (int32 i = 0; i < Rungs.Num(); i++)
    {
        UStaticMeshComponent* Rung = Rungs[i];
        if (!Rung) continue;
        
        // 遍历找最近的横条
        FVector RungLocation = Rung->GetComponentLocation();
        float Distance = FMath::Abs(RungLocation.Z - CharacterLocation.Z);

        if (Distance <= MinDistance)
        {
            MinDistance = Distance;
            ClosestRung = Rung;
            Index = i;
        }
        else break;
    }
    
    FVector RungLocation = ClosestRung->GetComponentLocation();
    FinalZValue = RungLocation.Z;
    if (ClosestRung && RungLocation.Z < CharacterLocation.Z)
    {
        if (Index != Rungs.Num() - 1)
        {
            FinalZValue += SupportHeight;   //找到的横条比角色低那就下一级
            Index++;
        }
    }
    if (isNext)
    {
        if (Index != Rungs.Num() - 1)
        {
            FinalZValue += SupportHeight;   //已经在梯子上了那就再下一级
        }
    }

    FVector FinalRungLocation = FVector(RungLocation.X, RungLocation.Y, FinalZValue); //横条的位置
    
    // 获取梯子的前向方向（考虑旋转）
    FVector ForwardDirection = GetActorForwardVector();

    // 计算角色最终位置,实际上在两节阶梯之间，这里为了手的匹配因此要调整
    FVector FinalCharactorLocation = FinalRungLocation - (ForwardDirection * 38.0f);
    FinalCharactorLocation.Z -= 127.0f;
    return FinalCharactorLocation;
}
