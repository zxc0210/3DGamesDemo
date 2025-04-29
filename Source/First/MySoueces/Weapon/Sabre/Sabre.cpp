// Fill out your copyright notice in the Description page of Project Settings.


#include "Sabre.h"

#include "First/MyCharacter/NPCBaseCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASabre::ASabre()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SM_Sabre = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SabreSkeletalMeshComponent"));
	if (SM_Sabre) {
		
		auto assetSkeletal = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/_project/Sources/Weapon/PirateSword/SM_PirateSword.SM_PirateSword'"));
		if (assetSkeletal.Succeeded()) {
			SM_Sabre->SetSkeletalMesh(assetSkeletal.Object);
			SM_Sabre->SetRelativeRotation(FRotator(0, -90, 0));
		}
	}
	SM_Sabre->SetWorldScale3D(FVector(1.3f, 1.3f, 1.3f));
	SM_Sabre->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SM_Sabre->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 保留与地面的碰撞
	SM_Sabre->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // 忽略角色的碰撞
	SM_Sabre->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 可见性测试
	SM_Sabre->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block); 
	RootComponent = SM_Sabre;
	
	SM_Sabre->OnComponentBeginOverlap.AddDynamic(this, &ASabre::BeginOverlapFunction);
	SM_Sabre->OnComponentEndOverlap.AddDynamic(this, &ASabre::EndOverlapFunction);
	
	ExplosionSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/StarterContent/Audio/Explosion02.Explosion02"));
}

// Called when the game starts or when spawned
void ASabre::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASabre::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isAttacking)
	{
		SabreAttackCheck();
	}
}

void ASabre::SabreAttackStart()
{
	isAttacking = true;
	AudioAlreadyPlayed = false;
}

void ASabre::SabreAttackEnd()
{
	isAttacking = false;
	//结束之后清空本次攻击的set
	if (!AlreadyAttackedNpcSet.IsEmpty())
	{
		AlreadyAttackedNpcSet.Empty();
	}
}

void ASabre::SabreAttackCheck()
{
	// 每次都重新获取武器上的两个插槽的位置
	FVector StartLocation = SM_Sabre->GetSocketLocation(TEXT("SabreStart"));
	FVector EndLocation = SM_Sabre->GetSocketLocation(TEXT("SabreEnd"));
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
		// 检查本次攻击是否已经攻击过这个NPC
		if (!AlreadyAttackedNpcSet.Contains(AttackedNPC))
		{
			// 没有攻击过才减少血量并记录
			AttackedNPC->ReduceNPCBlood(SabreAttackNum);
			AlreadyAttackedNpcSet.Add(AttackedNPC);
			// if (!AudioAlreadyPlayed)	// 播放声音
			// {
			// 	UGameplayStatics::PlaySoundAtLocation(
			// 			this, 
			// 			ExplosionSound, 
			// 			HitResult.Location, // NPC被击中的位置
			// 			1.0f, // 声音大小
			// 			1.0f, // 音调大小
			// 			0.0f, // 从声音的哪个时间点开始播放
			// 			nullptr, // 控制声音的衰减
			// 			nullptr 
			// 		);
			// 	AudioAlreadyPlayed = true;
			// }	
		}	
	}
	
}


void ASabre::Throw(FVector StartLocation, FVector EndLocation, FQuat ActorRotation, bool HitOrNot)
{
	// 使用此组件驱动发射物的运动
	ProjectileMovementComponent = NewObject<UProjectileMovementComponent>(SM_Sabre);
	ProjectileMovementComponent->RegisterComponent();
	ProjectileMovementComponent->SetUpdatedComponent(SM_Sabre);
	
	//旋转每帧更新，匹配速度方向
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	FVector OutVelocity;
	bool bSuccess = UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, OutVelocity, StartLocation, EndLocation);
	ProjectileMovementComponent->Velocity = OutVelocity * 1.0f;
	
	ThrowActorRotation = ActorRotation;
	ThrowActorLocation = EndLocation;
	HitTraceAEnd = HitOrNot;
}

void ASabre::BeginOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	// //启用自定义深度渲染，实现描边
	// SM_Sabre->SetRenderCustomDepth(true);
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
			SM_Sabre->SetRelativeLocation(ThrowActorLocation);
			SM_Sabre->SetRelativeRotation(ThrowActorRotation);
			HitTraceAEnd = false;
		}
	}
	
}

void ASabre::EndOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	SM_Sabre->SetRenderCustomDepth(false);
	isOverlap = false;
}

EMySourceType ASabre::GetSourceTypeName()
{
	return EMySourceType::Sabre;
}

void ASabre::SetHighLight()
{
	SM_Sabre->SetRenderCustomDepth(true);
}

void ASabre::DisableHighLight()
{
	SM_Sabre->SetRenderCustomDepth(false);
}
