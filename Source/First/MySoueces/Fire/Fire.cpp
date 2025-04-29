// Fill out your copyright notice in the Description page of Project Settings.

#include "Fire.h"

bool AFire::bPickedSource = true; // 静态变量初始化
int32 AFire::MaxStackNum = 2; 

// Sets default values
AFire::AFire()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//网格体
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sm_fireHandle(
		TEXT("StaticMesh'/Game/StarterContent/Props/SM_PillarFrame.SM_PillarFrame'"));
	fireHandle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("smc"));
	fireHandle->SetStaticMesh(sm_fireHandle.Object);
	SetRootComponent(fireHandle);
	fireHandle->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	fireHandle->SetCollisionResponseToAllChannels(ECR_Ignore);

	//粒子
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ps_fireParticle(
		TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Fire.P_Fire'"));
	fireParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("psc"));
	fireParticle->SetTemplate(ps_fireParticle.Object);
	fireParticle->SetupAttachment(RootComponent);

	//光源
	fireLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("plc"));
	fireLight->SetupAttachment(fireParticle);

	// CapsuleCollision Collision
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	CapsuleCollision->SetupAttachment(RootComponent);
	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); // 地面
	CapsuleCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 角色
	CapsuleCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 可见性测试
	CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &AFire::BeginOverlapFunction);
	CapsuleCollision->OnComponentEndOverlap.AddDynamic(this, &AFire::EndOverlapFunction);
	
	this->Construct(); //构造方法
}

// Called when the game starts or when spawned
void AFire::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFire::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFire::Construct()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(
		TEXT("/Script/Engine.Material'/Game/StarterContent/Materials/M_Wood_Pine.M_Wood_Pine'"));
	fireHandle->SetMaterial(0, Material.Object);
	fireHandle->SetWorldScale3D(FVector(0.3f, 0.3f, 0.15f));

	fireParticle->SetRelativeLocation(FVector(0, 0, 420));

	fireLight->SetRelativeLocation(FVector(0, 0, 20));
	fireLight->SetLightColor(FColor(243, 159, 24));
	fireLight->SetIntensity(10000);

	CapsuleCollision->InitCapsuleSize(16.0f, 200.0f);
	CapsuleCollision->SetRelativeLocation(FVector(10, 0, 200));
}

void AFire::EnablePhysics()
{
	//启用物理模拟和重力
	fireHandle->SetSimulatePhysics(true);
	fireHandle->SetEnableGravity(true);
}

void AFire::DisablePhysics()
{
	// 禁用物理模拟和重力
	fireHandle->SetSimulatePhysics(false);
	fireHandle->SetEnableGravity(false);
}

void AFire::Throw(FVector StartLocation, FVector EndLocation, FQuat ActorRotation, bool HitOrNot)
{
	// 使用此组件驱动发射物的运动
	ProjectileMovementComponent = NewObject<UProjectileMovementComponent>(fireHandle);
	ProjectileMovementComponent->RegisterComponent();
	ProjectileMovementComponent->SetUpdatedComponent(fireHandle);
	
	//旋转每帧更新，匹配速度方向
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	FVector OutVelocity;
	bool bSuccess = UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, OutVelocity, StartLocation, EndLocation);
	ProjectileMovementComponent->Velocity = OutVelocity * 1.0f;
	
	ThrowActorRotation = ActorRotation;
	ThrowActorLocation = EndLocation;
	HitTraceAEnd = HitOrNot;
}

void AFire::BeginOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	// //启用自定义深度渲染，实现描边
	// fireHandle->SetRenderCustomDepth(true);
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
			fireHandle->SetRelativeLocation(ThrowActorLocation);
			fireHandle->SetRelativeRotation(ThrowActorRotation);
			HitTraceAEnd = false;
		}
	}
	
}

void AFire::EndOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	fireHandle->SetRenderCustomDepth(false);
	isOverlap = false;
}

bool AFire::isPickedAsset()
{
	return bPickedSource;
}

int32 AFire::GetMaxStackNum()
{
	return MaxStackNum;
}

EMySourceType AFire::GetSourceTypeName()
{
	return EMySourceType::Fire;
}

void AFire::SetHighLight()
{
	fireHandle->SetRenderCustomDepth(true);
}

void AFire::DisableHighLight()
{
	fireHandle->SetRenderCustomDepth(false);
}
