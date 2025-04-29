// Fill out your copyright notice in the Description page of Project Settings.


#include "Wolf.h"

#include "EpicCharacter.h"
#include "AI/MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/PawnSensingComponent.h"

TArray<EMySourceType> AWolf::GenerateResourceType = { 
	EMySourceType::Plank,
	EMySourceType::Iron
};

// Sets default values
AWolf::AWolf()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 将 AI 控制器类指定为 AMyAIController
	AIControllerClass = AMyAIController::StaticClass();

	GetCapsuleComponent()->SetCapsuleSize(60.0f,60.0f);
	
	// GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	 GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 保留与地面的碰撞
	 GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 忽略角色碰撞
	 GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block); // 自定义攻击碰撞通道
	
	//骨骼网格体
	SM_Wolf = this->FindComponentByClass<USkeletalMeshComponent>();
	if (SM_Wolf)
	{
		auto assetSkeletal = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT(	"SkeletalMesh'/Game/AnimalVarietyPack/Wolf/Meshes/SK_Wolf.SK_Wolf'"));
		if (assetSkeletal.Succeeded())
		{
			SM_Wolf->SetSkeletalMesh(assetSkeletal.Object);
			SM_Wolf->SetRelativeLocation(FVector(0, 0, -60));
			SM_Wolf->SetRelativeRotation(FRotator(0, -90, 0));
		}
	}
	 // SM_Wolf->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	 // SM_Wolf->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 保留与地面的碰撞
	 // SM_Wolf->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 忽略角色碰撞
	
	
	//设置动画蓝图
	static ConstructorHelpers::FClassFinder<UAnimInstance> WolfAnimBP(TEXT("AnimBlueprint'/Game/_project/Character/NPC/Wolf/Animations/WolfCharacter_AnimBP.WolfCharacter_AnimBP_C'"));
	if (WolfAnimBP.Succeeded())
	{
		SM_Wolf->SetAnimInstanceClass(WolfAnimBP.Class);
	}
	
	//平滑角色的旋转
	bUseControllerRotationYaw = false;	//禁用使用控制器旋转 Yaw
	GetCharacterMovement()->bOrientRotationToMovement = true; //使角色面向移动方向
	
	// 初始化AI感知组件为 NPC添加视野
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1000.0f; // 视野半径
	SightConfig->LoseSightRadius = 1200.0f; // 失去视野半径
	SightConfig->PeripheralVisionAngleDegrees = 120.0f; // 视野角度
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->SetDominantSense(*SightConfig->GetSenseImplementation());
	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AWolf::OnTargetPerceptionUpdated);		// 绑定感知回调函数

	//初始化继承的属性
	NPCBloodNUm = 40;
}


float AWolf::GetWolfSpeed() const
{
	return GetVelocity().Size();
}

// Called when the game starts or when spawned
void AWolf::BeginPlay()
{
	Super::BeginPlay();
	
	AIController = Cast<AMyAIController>(GetController());// 获取 AI 控制器并转化为 AMyAIController
}

// Called every frame
void AWolf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bPerceivingCharacter && PerceivedCharacter && !IsDied)
	{
		AIController->GetBlackboardComponent()->SetValueAsBool("IsChase", true);
		this->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		// 追逐 Pawn, 设置到达目标的接受半径为 1.0
		EPathFollowingRequestResult::Type Result = AIController->MoveToActor(PerceivedCharacter, 1.0f);
		if (Result == EPathFollowingRequestResult::AlreadyAtGoal)	// 如果已经追逐到目标
		{
			IsAllowAttack = true;
		}
		else IsAllowAttack = false;	
	}
	else IsAllowAttack = false;

	if (IsDied && AIController)
	{
		PerceptionComp->DestroyComponent();
		AIController->StopMovement();
		AIController = nullptr;
	}
}

// Called to bind functionality to input
void AWolf::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AWolf::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 每次有目标进入或离开时都会调用该函数
	if (Stimulus.WasSuccessfullySensed())
	{
		if (Actor && Actor->IsA(AEpicCharacter::StaticClass()))	//如果有攻击目标并且目标是角色的话
		{
			bPerceivingCharacter = true;
			PerceivedCharacter = Actor;
			EpicCharacter = Cast<AEpicCharacter>(PerceivedCharacter);
		}
	}
	else
	{
		if (Actor && Actor->IsA(AEpicCharacter::StaticClass()))
		{
			bPerceivingCharacter = false;
			PerceivedCharacter = nullptr;
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Lost sight of: %s"), *Actor->GetName()));
			AIController->GetBlackboardComponent()->SetValueAsBool("IsChase", false);	
			EpicCharacter = nullptr;
		}
	}
	
}

void AWolf::AttackOccurredFromBP()
{
	if (EpicCharacter)
	{
		EpicCharacter->BloodSystem->ModifyBlood(WolfDamageAmount); // 修改角色血量
	}

}

bool AWolf::GetAllowAttackState()
{
	if (EpicCharacter && EpicCharacter->BloodSystem->GetCurrentBlood() == 0) //血量为 0之后就不来追了
	{
		bPerceivingCharacter = false;
		PerceivedCharacter = nullptr;
		IsAllowAttack = false;
	
	}
	return IsAllowAttack;
}

void AWolf::ReduceNPCBlood(int32 ReduceNum)
{
	if (NPCBloodNUm <= 0)
	{
		return;
	}
	NPCBloodNUm -= ReduceNum;

	FString String = FString::Printf(TEXT("%d"), NPCBloodNUm);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, String);
	if (NPCBloodNUm <= 0)
	{
		IsDied = true;
		
		GetController()->UnPossess(); // 解除控制
		
		// UCapsuleComponent* WolfCapsuleComponent = this->GetCapsuleComponent();
		// if (WolfCapsuleComponent)
		// {
		// 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("get wolf succeed"));
		// 	WolfCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 	WolfCapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		// }
	}
}

TArray<EMySourceType> AWolf::GetDropResourceType()	//获取掉落的资源类型
{
	//根据生成资源类型随机生成掉落总数量
	for (int i = 0; i < GenerateResourceType.Num(); ++i)
	{
		EMySourceType st = GenerateResourceType[i];
		int32 MaxNum = 2 * AMyAssets::GetResourceMaxStackNum(st); //每种的资源数量最多为最大堆叠数量的2倍
		int32 Count = FMath::RandRange(1, MaxNum);
		GenerateResourceNum.Add(Count);
	}
	//根据生成类型与数量计算实际分配的资源数组与数量数组
	for (int i = 0; i < GenerateResourceType.Num(); ++i)
	{
		EMySourceType st = GenerateResourceType[i];
		int32 MaxStackNum = AMyAssets::GetResourceMaxStackNum(st); 
		int32 RemainingCount = GenerateResourceNum[i]; // 当前资源的数量
		// 计算所需的格子数，并分配数量
		while (RemainingCount > 0)
		{
			int32 AllocatedCount = FMath::Min(MaxStackNum, RemainingCount); // 分配的数量不超过最大堆叠数量
			DropResourceTypeArray.Add(st);
			DropResourceNumArray.Add(AllocatedCount); 
			RemainingCount -= AllocatedCount; // 减少剩余的未分配数量
		}
	}

	return DropResourceTypeArray;
}

TArray<int32> AWolf::GetDropResourceNum()	//获取掉落每种类型的资源数量
{
	return DropResourceNumArray;
}

bool AWolf::GetWolfAliveState()
{
	return IsDied;
}
