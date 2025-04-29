// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyAssets.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "MovieSceneTracksComponentTypes.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Fire.generated.h"

UCLASS()
class FIRST_API AFire : public AMyAssets
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	static bool bPickedSource;
	static int32 MaxStackNum;  //每个格子该资源的最多堆叠数量
	static int32 GetMaxStackNum();

	void EnablePhysics();
	void DisablePhysics();
	bool isPickedAsset();
	void Throw(FVector StartLocation, FVector EndLocation, FQuat ActorRotation, bool HitOrNot);
	EMySourceType GetSourceTypeName();
	void SetHighLight() override;
	void DisableHighLight() override;
	
private:
	void Construct();
	
	UPROPERTY(EditAnywhere, Category = "Fire")
	UStaticMeshComponent* fireHandle;
	UPROPERTY(EditAnywhere, Category = "Fire")
	UParticleSystemComponent* fireParticle;
	UPROPERTY(EditAnywhere, Category = "Fire")
	UPointLightComponent* fireLight;
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* CapsuleCollision;

	UPROPERTY(EditAnywhere, Category = "Fire")
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	FQuat ThrowActorRotation;
	FVector ThrowActorLocation;
	bool HitTraceAEnd = false;
	bool isOverlap = false;


	
	UFUNCTION()
	void BeginOverlapFunction(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void EndOverlapFunction(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);
	
};
