// Fill out your copyright notice in the Description page of Project Settings.


#include "My_Bow.h"

#include "Components/CapsuleComponent.h"

// Sets default values
AMy_Bow::AMy_Bow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//弓箭的骨骼网格体
	SM_Bow = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BowSkeletalMeshComponent"));
	if (SM_Bow) {
		auto assetSkeletal = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/_project/Sources/Weapon/Bow/SK_Bow.SK_Bow'"));
		if (assetSkeletal.Succeeded()) {
			SM_Bow->SetSkeletalMesh(assetSkeletal.Object);
		}
	}
	SM_Bow->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));
	SM_Bow->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SM_Bow->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 保留与地面的碰撞
	SM_Bow->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 忽略角色的碰撞
	SM_Bow->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 可见性测试
	RootComponent = SM_Bow;
	
	//设置动画蓝图
	static ConstructorHelpers::FClassFinder<UAnimInstance> BowAnimBP(TEXT("AnimBlueprint'/Game/_project/Sources/Weapon/Bow/Animations/Bow_AnimBP.Bow_AnimBP_C'"));
	if (BowAnimBP.Succeeded())
	{
		SM_Bow->SetAnimInstanceClass(BowAnimBP.Class);
	}

	// //设置动画蒙太奇
	// static ConstructorHelpers::FObjectFinder<UAnimMontage> DrawOnlyObj(TEXT("AnimMontage'/Game/_project/Sources/Weapon/Bow/Animations/Bow_DrawOnly_Montage.Bow_DrawOnly_Montage'"));
	// if (DrawOnlyObj.Succeeded())
	// {
	// 	Montage_DrawOnly = DrawOnlyObj.Object;
	// }
	
	UCapsuleComponent* CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	CapsuleCollision->InitCapsuleSize(5.0f, 50.0f);
	// 设置碰撞
	CapsuleCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 保留与地面的碰撞
	CapsuleCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 忽略角色的碰撞
	CapsuleCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 可见性测试
	CapsuleCollision->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AMy_Bow::BeginPlay()
{
	Super::BeginPlay();
	
	// 在BeginPlay()中获取并保存动画实例
	BowAnimInstance = SM_Bow->GetAnimInstance();
}

// Called every frame
void AMy_Bow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMy_Bow::Bow_Idle()
{
	//设置弓箭状态
	SetBowState(EBowState::Idle);

	if (MyArrow)
	{
		MyArrow->Destroy();
		MyArrow = nullptr;
	}
}

void AMy_Bow::Bow_AimingOnly()
{
	SetBowState(EBowState::Aiming);
	
	if (MyArrow == nullptr)
	{
		// 创建箭对象并设置初始位置
		MyArrow = GetWorld()->SpawnActor<AMy_Arrow>(AMy_Arrow::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
		// 将箭对象附着到弓的插槽 ArrowSocket 上
		MyArrow->AttachToComponent(SM_Bow, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("ArrowSocket"));	
	}
}

void AMy_Bow::Bow_Fire()
{
	SetBowState(EBowState::Fire);

	if (MyArrow)
	{
		//发射箭
		MyArrow->ArrowProjectile();
	}
}

void AMy_Bow::SetBowState(EBowState state)
{
	//动画放在动画蓝图
	E_BowState = state;
}

EMySourceType AMy_Bow::GetSourceTypeName()
{
	return EMySourceType::Bow;
}
