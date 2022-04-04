// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumActor.generated.h"

// 1. enum class 作为蓝图类型，给出一个枚举值显示名
UENUM(BlueprintType)
enum class EnumWeather : uint8 {
	ENone = 0	UMETA(DisplayName = "None"),
	ESunny = 3	UMETA(DisplayName = "Sunny"),
	ECloudy		UMETA(DisplayName = "Cloudy"),
	EWindy		UMETA(DisplayName = "Windy"),
};


// 2. enum class 无法提供给蓝图，但给出一个显示名
UENUM()
enum class EnumWeather1 : uint8 {
	ENone		UMETA(DisplayName = "None"),
	ESunny = 3	UMETA(DisplayName = "Sunny"),
	ECloudy		UMETA(DisplayName = "Cloudy"),
	EWindy		UMETA(DisplayName = "Windy"),
};

// 3. 无法提供给蓝图使用
UENUM()
enum class EnumWeather2 : uint8 {
	ENone,
	ESunny = 3,
	ECloudy,
	EWindy,
};

// 4. 无法提供给蓝图使用
UENUM()
namespace EnumWeather3 {
	enum Type {
		ENone,
		ESunny = 3,
		ECloudy,
		EWindy,
	};
}

// 5. 容易发生冲突
UENUM()
enum EnumWeather4 {
	ENone,
	ESunny = 3,
	ECloudy,
	EWindy,
};

// 6. 会和上面EnumWeather4中定义的枚举值冲突 ！！！

/*
UENUM(BlueprintType)
enum EnumWeather5 {
	ENone,
	ESunny = 3,
	ECloudy,
	EWindy,
};
*/


UCLASS()
class LESSON_ENUM_API AEnumActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnumActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	EnumWeather W;

	UPROPERTY()
	EnumWeather1 W1;

	UPROPERTY()
	EnumWeather2 W2;

	UPROPERTY()
	int32 W3;

	UPROPERTY()
	int32 W4;
};
