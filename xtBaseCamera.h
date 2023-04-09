// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "xtBaseCamera.generated.h"

UCLASS(BlueprintType, Blueprintable)
class NEXTER_API AxtBaseCamera : public AActor
{	
	GENERATED_BODY()
public:	
	
	// Sets default values for this actor's properties
	AxtBaseCamera();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Prepare();
	virtual void Destroy();

	void SetFov(float fov);
	float GetFov();

	float GetDistance();
	void SetDistance(float dist);
	float AddDistance(float value);

	void SetTraceTarget(class USceneComponent* chaseTarget);
	void LookUp(class USceneComponent* lookUp);

	void SetIsTrace(bool isTrace);

	UFUNCTION()
	void Enlarge(float value);
	UFUNCTION()
	void OnInputPitch(float value);
	UFUNCTION()
	void OnInputYaw(float value);

	virtual void NotifyActorBeginOverlap(AActor* otherActor) override;
	virtual void NotifyActorEndOverlap(AActor* otherActor) override;

	virtual void SetBias(FVector2D bias);
	virtual void SetBias(FVector bias);
	FVector GetBias() { return _prevBias;}

	bool IsTrace;

private:

	FVector _prevBias;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	TSubclassOf< class UxtCameraFunctionalParts > _moveMethodClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	TSubclassOf< class UxtCameraFunctionalParts > _lookUpMethodClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraRod;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USceneComponent> Center;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> OverlapSpace;

	UPROPERTY()
	TObjectPtr<class UCameraShakeBase> Shake;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	bool IsFovEdit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	float MinDist;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	float MaxDist;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	float AddMoveRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true", ClampMin = "1.0", ClampMax = "180.0", UIMin = "1.0", UIMax = "180.0"))
	float MinFov;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true", ClampMin = "1.0", ClampMax = "180.0", UIMin = "1.0", UIMax = "180.0"))
	float MaxFov;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	float MaxBotPitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	float MaxTopPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	float PitchRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	bool PitchInverse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	float YawRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	bool YawInverse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	float MovingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	float UpDownMorphRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	float LimitChaseTailDist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	float OverlapAlpha;

	UPROPERTY()
	TObjectPtr<class USceneComponent> ChaseTarget;

	UPROPERTY()
	TObjectPtr<class USceneComponent> LookUpTarget;

	UPROPERTY()
	TObjectPtr<class UxtCameraFunctionalParts> MoveMethod;

	UPROPERTY()
	TObjectPtr<class UxtCameraFunctionalParts> LookUpMethod;

	FVector PrevLocation;
};


