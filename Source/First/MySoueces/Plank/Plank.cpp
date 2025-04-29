// Fill out your copyright notice in the Description page of Project Settings.


#include "Plank.h"
#include "Components/StaticMeshComponent.h"
#include "Uobject/ConstructorHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

int32 APlank::MaxStackNum = 10;

// Sets default values
APlank::APlank()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticM(TEXT("StaticMesh'/Game/_project/Sources/Plank/SM_Wood_03.SM_Wood_03'"));
	StaticMC_Plank = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticM"));
	StaticMC_Plank->SetupAttachment(RootComponent);
	StaticMC_Plank->SetStaticMesh(StaticM.Object);
	StaticMC_Plank->SetRelativeRotation(FRotator(0, -90, 0));
	StaticMC_Plank->SetWorldScale3D(FVector(0.25f, 0.35f, 0.3f));
	StaticMC_Plank->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMC_Plank->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); // 地面
	StaticMC_Plank->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 角色
	StaticMC_Plank->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 可见性测试
	StaticMC_Plank->OnComponentBeginOverlap.AddDynamic(this, &APlank::BeginOverlapFunction);
	StaticMC_Plank->OnComponentEndOverlap.AddDynamic(this, &APlank::EndOverlapFunction);
	
}

// Called when the game starts or when spawned
void APlank::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlank::Throw(FVector StartLocation, FVector EndLocation, FQuat ActorRotation, bool HitOrNot)
{
	// 使用此组件驱动发射物的运动
	ProjectileMovementComponent = NewObject<UProjectileMovementComponent>(StaticMC_Plank);
	ProjectileMovementComponent->RegisterComponent();
	ProjectileMovementComponent->SetUpdatedComponent(StaticMC_Plank);
	
	//旋转每帧更新，匹配速度方向
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	FVector OutVelocity;
	bool bSuccess = UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, OutVelocity, StartLocation, EndLocation);
	ProjectileMovementComponent->Velocity = OutVelocity * 1.0f;
	
	ThrowActorRotation = ActorRotation;
	ThrowActorLocation = EndLocation;
	HitTraceAEnd = HitOrNot;
}

void APlank::BeginOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	//启用自定义深度渲染，实现描边
	//StaticMC_Plank->SetRenderCustomDepth(true);
	if (OtherActor && (OtherActor != this) && !OtherActor->IsA(ACharacter::StaticClass()))	
	{
		// FString String = FString::Printf(TEXT("%s"), *OtherActor->GetName());
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, String);
		isOverlap = true;
		
		if (ProjectileMovementComponent)
		{
			ProjectileMovementComponent->DestroyComponent();
			ProjectileMovementComponent = nullptr; // 防止重复销毁
		}
		if (HitTraceAEnd)
		{
			StaticMC_Plank->SetRelativeLocation(ThrowActorLocation);
			StaticMC_Plank->SetRelativeRotation(ThrowActorRotation);
			HitTraceAEnd = false;
		}
	}
	
}

void APlank::EndOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	StaticMC_Plank->SetRenderCustomDepth(false);
	isOverlap = false;
}

int32 APlank::GetMaxStackNum()
{
	return MaxStackNum;
}

EMySourceType APlank::GetSourceTypeName()
{
	return EMySourceType::Plank;
}

void APlank::SetHighLight()
{
	StaticMC_Plank->SetRenderCustomDepth(true);
}

void APlank::DisableHighLight()
{
	StaticMC_Plank->SetRenderCustomDepth(false);
}
