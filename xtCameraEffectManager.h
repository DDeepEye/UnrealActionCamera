// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NexterCommon/Foundations/xtSingleton.h"
#include "xtCameraEffectManager.generated.h"

class UxtCameraShake;
class UxtCameraBias;
class UxtCameraShakeRepeatSection;
class IxtCameraEffect;

UENUM(BlueprintType)
enum class CameraEffectType : uint8
{
	Shake,
	RepeatPointShake,
	Bias,
	Max,
};


UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class AnimationBitFlag
{
	X = 1,
	Y = (0x01 << 2),
	Z = (0x01 << 3),

	Pitch = (0x01 << 4),
	Yaw = (0x01 << 5),
	Roll = (0x01 << 6),

	Fov = (0x01 << 7),
};
ENUM_CLASS_FLAGS(AnimationBitFlag)


USTRUCT(BlueprintType)
struct NEXTER_API FxtAnimation
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "AnimationBitFlag"))
	uint8 flags = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> _animation = nullptr;
};


UINTERFACE(BlueprintType)
class UxtCameraEffect : public UInterface
{
	GENERATED_BODY()
};
class NEXTER_API IxtCameraEffect
{
	GENERATED_BODY()
public:
	virtual bool IsActive() = 0;
	virtual bool IsFinished() = 0;
	virtual CameraEffectType GetType() = 0;
	virtual void TickEffect(float DeltaTime, FVector& outLocation, FRotator& outRotator, float& outFov) = 0;
	virtual void CleanUp(FVector& outLocation, FRotator& outRotator, float& outFov) = 0;
	virtual bool IsArbitraryTermination(uint64 actorUID, uint64 animUID, FVector& outLocation, FRotator& outRotator, float& outFov, float& outLogValue) = 0;
	virtual bool IsArbitraryTermination(uint64 actorUID, FVector& outLocation, FRotator& outRotator, float& outFov) = 0;
	

protected:
	bool _isFlag(uint8 flags, uint8 check);
};

UCLASS()
class NEXTER_API UxtCameraEffectManager final : public UObject, public UxtSingleton< UxtCameraEffectManager >
{
	GENERATED_BODY()
public:	

	UxtCameraEffectManager();
	~UxtCameraEffectManager();

	void Tick(float deltaTime);
	void Shake(struct FxtShakeInfo& info);
	void Shake(const FString fileName, float locScale = 1.0f, float rotScale = 1.0f);
	void Shake(UxtCameraShake* shake, float locScale, float rotScale);

	void RepeatShake(struct FxtShakeRepeatSectionInfo& info);

	void Bias(struct FxtCameraBiasInfo& info);

	UxtCameraShake* PreLoadShake(const TCHAR* fileName);
	UxtCameraShake* PreLoadShake();
	void PreLoadShake(int32 loadCount);
	UxtCameraBias* PreLoadBias();

	UxtCameraShakeRepeatSection* PreLoadRepeatShake();
	void PreLoadRepeatShake(int32 loadCount);


	void PreLoadBias(int32 loadCount);

	float Arbitrary(uint64 actorUID, uint64 animUID);

	void Aboart(uint64 actorUID);

	void PostCreated();
	void PreDestroy();
	bool SkipShake;
private:
	enum size { MixCapacity = 32 };
	class AxtGameModeBase* _getGameMode();
	class UClass* _assetToClass(FString shakeAssetName);
	bool _mixBufferCheck(UObject* effect);
	void _keepOriginalBias();

	uint32 _mixCount;

	UPROPERTY()
	TScriptInterface<IxtCameraEffect> _effectBuffers[MixCapacity];

	UPROPERTY()
	TArray<TScriptInterface<IxtCameraEffect>> _pool;

	UPROPERTY()
	TObjectPtr<class AxtGameModeBase> _gameMode;

	FVector _originBias;
};



