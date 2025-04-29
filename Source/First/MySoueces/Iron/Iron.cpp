// Fill out your copyright notice in the Description page of Project Settings.


#include "Iron.h"
#include "Components/StaticMeshComponent.h"
#include "Uobject/ConstructorHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

int32 AIron::MaxStackNum = 10;

// Sets default values
AIron::AIron()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticM(TEXT("StaticMesh'/Game/_project/Sources/Iron/SM_IronBar.SM_IronBar'"));
	StaticMC_Iron = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticM"));
	StaticMC_Iron->SetupAttachment(RootComponent);
	StaticMC_Iron->SetStaticMesh(StaticM.Object);
	StaticMC_Iron->SetWorldScale3D(FVector(0.2f, 0.15f, 0.15f));
	StaticMC_Iron->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMC_Iron->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); // 地面
	StaticMC_Iron->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 角色
	StaticMC_Iron->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 可见性测试
	StaticMC_Iron->OnComponentBeginOverlap.AddDynamic(this, &AIron::BeginOverlapFunction);
	StaticMC_Iron->OnComponentEndOverlap.AddDynamic(this, &AIron::EndOverlapFunction);
}


void AIron::Throw(FVector StartLocation, FVector EndLocation, FQuat ActorRotation, bool HitOrNot)
{
	// 使用此组件驱动发射物的运动
	ProjectileMovementComponent = NewObject<UProjectileMovementComponent>(StaticMC_Iron);
	ProjectileMovementComponent->RegisterComponent();
	ProjectileMovementComponent->SetUpdatedComponent(StaticMC_Iron);
	
	//旋转每帧更新，匹配速度方向
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	FVector OutVelocity;
	bool bSuccess = UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, OutVelocity, StartLocation, EndLocation);
	ProjectileMovementComponent->Velocity = OutVelocity * 1.0f;
	
	ThrowActorRotation = ActorRotation;
	ThrowActorLocation = EndLocation;
	HitTraceAEnd = HitOrNot;
}

void AIron::BeginOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
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
			StaticMC_Iron->SetRelativeLocation(ThrowActorLocation);
			StaticMC_Iron->SetRelativeRotation(ThrowActorRotation);
			HitTraceAEnd = false;
		}
	}
	
}

void AIron::EndOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	StaticMC_Iron->SetRenderCustomDepth(false);
	isOverlap = false;
}

int32 AIron::GetMaxStackNum()
{
	return MaxStackNum;
}

EMySourceType AIron::GetSourceTypeName()
{
	return EMySourceType::Iron;
}

void AIron::SetHighLight()
{
	//启用自定义深度渲染，实现描边
	StaticMC_Iron->SetRenderCustomDepth(true);
}

void AIron::DisableHighLight()
{
	StaticMC_Iron->SetRenderCustomDepth(false);
}
