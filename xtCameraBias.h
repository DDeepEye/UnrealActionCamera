// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "xtCameraEffectManager.h"
#include "UObject/NoExportTypes.h"
#include "xtCameraBias.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct NEXTER_API FxtCameraBiasInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TArray<FxtAnimation> Animations;

	/** Duration in seconds of this shake. Zero or less means infinite. */
	UPROPERTY(EditAnywhere, Category = Timing)
	float Duration = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocationBias")
	FVector LocationBias = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotationBias")
	FRotator RotationBias = FRotator(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FovBias")
	float FovBias = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Charge)
	bool IsMaintain = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Charge)
	bool IsUseCharge = false;

	uint64 ActorUID = 0;
	uint64 AnimUID = 0;
};


UCLASS(BlueprintType)
class NEXTER_API UxtCameraBias : public UObject, public IxtCameraEffect
{
	GENERATED_BODY()

public:
	UxtCameraBias(const FObjectInitializer& ObjInit);
	void Start(FxtCameraBiasInfo& shakeInfo);

	virtual bool IsActive() override;
	virtual bool IsFinished() override;

	virtual CameraEffectType GetType() override;
	virtual void TickEffect(float DeltaTime, FVector& outLocation, FRotator& outRotator, float& outFov) override;
	virtual void CleanUp(FVector& outLocation, FRotator& outRotator, float& outFov) override;
	virtual bool IsArbitraryTermination(uint64 actorUID, uint64 animUID, FVector& outLocation, FRotator& outRotator, float& outFov, float& outLogValue) override;
	virtual bool IsArbitraryTermination(uint64 actorUID, FVector& outLocation, FRotator& outRotator, float& outFov) override;

private:
	virtual void _reset();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	float LocationScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	float RotationScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	float FovScale;

private:
	UPROPERTY()
	FxtCameraBiasInfo _biasInfo;

	float _timer;

	FVector _curLocation;
	FRotator _curRotator;
	float _curFov;
	
	float _logValue;
};


