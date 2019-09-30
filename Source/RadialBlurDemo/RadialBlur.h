// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Engine/Classes/Particles/Emitter.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "RadialBlur.generated.h"

UCLASS()
class RADIALBLURDEMO_API ARadialBlur : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARadialBlur();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Trigger();
	UFUNCTION(BlueprintCallable)
	void Shutdown();

	static void Start(UObject* InWorldContext);
	static void End();

private:

public:
	UPROPERTY(VisibleAnywhere)
	UMaterial* RadialBlurMat = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInstanceDynamic* RadialBlurMID = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APostProcessVolume* PostProcessVolumeActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ASceneCapture2D* SceneCapture2D = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CenterX = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CenterY = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Offset = 0.05f;

	UPROPERTY(Transient)
	ACharacter* Character;

	UPROPERTY(Transient)
	AEmitter* Emitter;
};

static ARadialBlur* _RadialBlur = nullptr;