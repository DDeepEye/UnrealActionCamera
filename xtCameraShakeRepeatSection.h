// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "xtCameraEffectManager.h"
#include "Camera/CameraShakeBase.h"
#include "PerlinNoiseCameraShakePattern.h"
#include "WaveOscillatorCameraShakePattern.h"
#include "xtCameraShakeRepeatSection.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct NEXTER_API FxtShakeRepeatSectionInfo
{
	GENERATED_USTRUCT_BODY()

	FxtShakeRepeatSectionInfo();
	FxtShakeRepeatSectionInfo(const UWaveOscillatorCameraShakePattern* pattern);
	FxtShakeRepeatSectionInfo operator = (const UWaveOscillatorCameraShakePattern* pattern);
	void RestorePattern(UWaveOscillatorCameraShakePattern* pattern);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FxtAnimation Animation;

	/** Duration in seconds of this shake. Zero or less means infinite. */
	UPROPERTY(EditAnywhere, Category = Timing)
	float Duration = 1.f;

	/** Blend-in time for this shake. Zero or less means no blend-in. */
	UPROPERTY(EditAnywhere, Category = Timing)
	float RepeatSectionRate = 0;

	/** Amplitude multiplier for all location oscillation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
	float LocationAmplitudeMultiplier = 1.f;

	/** Frequency multiplier for all location oscillation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
	float LocationFrequencyMultiplier = 1.f;

	/** Oscillation in the X axis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
	FWaveOscillator X;

	/** Oscillation in the Y axis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
	FWaveOscillator Y;

	/** Oscillation in the Z axis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
	FWaveOscillator Z;

	/** Amplitude multiplier for all rotation oscillation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotation)
	float RotationAmplitudeMultiplier = 1.f;

	/** Frequency multiplier for all rotation oscillation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotation)
	float RotationFrequencyMultiplier = 1.f;

	/** Roll oscillation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotation)
	FWaveOscillator Roll;

	/** Pitch oscillation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotation)
	FWaveOscillator Pitch;

	/** Yaw oscillation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotation)
	FWaveOscillator Yaw;

	/** FOV oscillation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FOV)
	FWaveOscillator FOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Charge)
	bool IsUseCharge = false;

	uint64 ActorUID;
	uint64 AnimUID;
};

UCLASS(Blueprintable, BlueprintType)
class NEXTER_API UxtCameraShakeRepeatSection : public UCameraShakeBase, public IxtCameraEffect
{
	GENERATED_BODY()
public:
	UxtCameraShakeRepeatSection(const FObjectInitializer& ObjInit);
	void Start(FxtShakeRepeatSectionInfo& shakeInfo);
	void Start();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "기획자분은 이곳을 주로 편집하시면 됩니다.", meta = (AllowPrivateAccess = "true"))
	FxtAnimation Animation;

private:
	void _reset();

public:
	virtual bool IsActive() override;
	virtual bool IsFinished() override;
	virtual CameraEffectType GetType() override;
	virtual void TickEffect(float DeltaTime, FVector& outLocation, FRotator& outRotator, float& outFov) override;
	virtual void CleanUp(FVector& outLocation, FRotator& outRotator, float& outFov) override;
	virtual bool IsArbitraryTermination(uint64 actorUID, uint64 animUID, FVector& outLocation, FRotator& outRotator, float& outFov, float& outLogValue) override;
	virtual bool IsArbitraryTermination(uint64 actorUID, FVector& outLocation, FRotator& outRotator, float& outFov) override;

protected:
	UPROPERTY()
	TObjectPtr<UWaveOscillatorCameraShakePattern> _pattern;

	UPROPERTY()
	FMinimalViewInfo _info;
	float _animationTime;
	float _repeatTime;
	uint64 _actorUID;
	uint64 _animUID;
	float _logValue;
	bool _onceFull;
	bool _isUseCharge;
};
