// Fill out your copyright notice in the Description page of Project Settings.


#include "My_Arrow.h"

#include "First/MyCharacter/NPCBaseCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

int32 AMy_Arrow::MaxStackNum = 10;

// Sets default values
AMy_Arrow::AMy_Arrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticM(TEXT("StaticMesh'/Game/_project/Sources/Weapon/Bow/SM_Bow_Arrow.SM_Bow_Arrow'"));
	StaticMC_Arrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticM"));
	StaticMC_Arrow->SetupAttachment(RootComponent);
	StaticMC_Arrow->SetStaticMesh(StaticM.Object);

	StaticMC_Arrow->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMC_Arrow->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); // 地面
	StaticMC_Arrow->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 角色
	StaticMC_Arrow->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 可见性测试
	StaticMC_Arrow->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block); 
	StaticMC_Arrow->OnComponentBeginOverlap.AddDynamic(this, &AMy_Arrow::BeginOverlapFunction);
	StaticMC_Arrow->OnComponentEndOverlap.AddDynamic(this, &AMy_Arrow::EndOverlapFunction);

}

// Called when the game starts or when spawned
void AMy_Arrow::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMy_Arrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isAttacking)
	{
		ArrowAttackCheck();
	}
}

void AMy_Arrow::CalculateDirection()
{
	// 获取摄像机的位置和前向矢量
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	
	// 瞄准方向从摄像机初始位置开始，到距前向矢量5000个单位距离的位置结束
	FVector LineTraceEnd = CameraLocation + (CameraRotation.Vector() * 5000.0f);
	// 追踪射线路径中是否会击中任何可见的物体，如果击中，那么设置箭的发射终点为击中的位置
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, LineTraceEnd, ECC_Visibility);
	if (HitResult.bBlockingHit)
	{
		ProjectileDirection = (HitResult.Location - StaticMC_Arrow->GetComponentLocation()).GetSafeNormal();
		// 绘制击中物体的射线
		//DrawDebugLine(GetWorld(), CameraLocation, HitResult.Location, FColor::Red, false, 2.0f, 0, 5.0f);
	}
	else
	{
		// 否则就还是原来射线的方向
		ProjectileDirection = (LineTraceEnd - StaticMC_Arrow->GetComponentLocation()).GetSafeNormal();
		// 绘制未击中物体的射线
		//DrawDebugLine(GetWorld(), CameraLocation, LineTraceEnd, FColor::Green, false, 2.0f, 0, 5.0f);
	}	
}

void AMy_Arrow::ArrowProjectile()
{

	CalculateDirection();
	isAttacking = true;
	
	// 创建并附加 ProjectileMovementComponent 发射箭
	UProjectileMovementComponent* ProjectileMovement = NewObject<UProjectileMovementComponent>(StaticMC_Arrow);
	ProjectileMovement->RegisterComponent();
	ProjectileMovement->SetUpdatedComponent(StaticMC_Arrow);

	// 设置初始速度和方向
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->Velocity = ProjectileDirection * ProjectileMovement->InitialSpeed;

	// 启用重力影响
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	
}

void AMy_Arrow::ArrowAttackCheck()
{
	// 每次都重新获取武器上的两个插槽的位置
	FVector StartLocation = StaticMC_Arrow->GetSocketLocation(TEXT("ArrowStart"));
	FVector EndLocation = StaticMC_Arrow->GetSocketLocation(TEXT("ArrowEnd"));
	// 胶囊体的半径和高度
	float CapsuleRadius = 1.0f;
	float CapsuleHalfHeight = 1.0f;
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);	// 忽略自身
	
	FHitResult HitResult;
	// 进行胶囊体检测
	bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(
		this,
		StartLocation,
		EndLocation,
		CapsuleRadius,
		CapsuleHalfHeight,
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1),	//检测通道为自定义的attack通道
		false, // 是否使用复杂碰撞
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame , // 显示检测胶囊
		HitResult,
		true
	);

	if (bHit && HitResult.GetActor()->IsA(ANPCBaseCharacter::StaticClass()))	// 如果检测到击中NPC
	{
		ANPCBaseCharacter* AttackedNPC = Cast<ANPCBaseCharacter>(HitResult.GetActor());
		// 减少血量
		AttackedNPC->ReduceNPCBlood(ArrowAttackNum);
		isAttacking = false;
		Destroy();
	}
}





void AMy_Arrow::Throw(FVector StartLocation, FVector EndLocation, FQuat ActorRotation, bool HitOrNot)
{
	// 使用此组件驱动发射物的运动
	ProjectileMovementComponent = NewObject<UProjectileMovementComponent>(StaticMC_Arrow);
	ProjectileMovementComponent->RegisterComponent();
	ProjectileMovementComponent->SetUpdatedComponent(StaticMC_Arrow);
	
	//旋转每帧更新，匹配速度方向
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	FVector OutVelocity;
	bool bSuccess = UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, OutVelocity, StartLocation, EndLocation);
	ProjectileMovementComponent->Velocity = OutVelocity * 1.0f;
	
	ThrowActorRotation = ActorRotation;
	ThrowActorLocation = EndLocation;
	HitTraceAEnd = HitOrNot;
}

void AMy_Arrow::BeginOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								 const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && !OtherActor->IsA(ACharacter::StaticClass()))	
	{
		isOverlap = true;
		
		if (ProjectileMovementComponent)
		{
			ProjectileMovementComponent->DestroyComponent();
			ProjectileMovementComponent = nullptr; // 防止重复销毁
		}
		if (HitTraceAEnd)
		{
			StaticMC_Arrow->SetRelativeLocation(ThrowActorLocation);
			StaticMC_Arrow->SetRelativeRotation(ThrowActorRotation);
			HitTraceAEnd = false;
		}
	}
	
}

void AMy_Arrow::EndOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	StaticMC_Arrow->SetRenderCustomDepth(false);
	isOverlap = false;
}

int32 AMy_Arrow::GetMaxStackNum()
{
	return MaxStackNum;
}

EMySourceType AMy_Arrow::GetSourceTypeName()
{
	return EMySourceType::Arrow;
}

void AMy_Arrow::SetHighLight()
{
	StaticMC_Arrow->SetRenderCustomDepth(true);
}

void AMy_Arrow::DisableHighLight()
{
	StaticMC_Arrow->SetRenderCustomDepth(false);
}
