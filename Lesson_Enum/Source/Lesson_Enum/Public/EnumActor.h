// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumActor.generated.h"

// 1. enum class ��Ϊ��ͼ���ͣ�����һ��ö��ֵ��ʾ��
UENUM(BlueprintType)
enum class EnumWeather : uint8 {
	ENone = 0	UMETA(DisplayName = "None"),
	ESunny = 3	UMETA(DisplayName = "Sunny"),
	ECloudy		UMETA(DisplayName = "Cloudy"),
	EWindy		UMETA(DisplayName = "Windy"),
};


// 2. enum class �޷��ṩ����ͼ��������һ����ʾ��
UENUM()
enum class EnumWeather1 : uint8 {
	ENone		UMETA(DisplayName = "None"),
	ESunny = 3	UMETA(DisplayName = "Sunny"),
	ECloudy		UMETA(DisplayName = "Cloudy"),
	EWindy		UMETA(DisplayName = "Windy"),
};

// 3. �޷��ṩ����ͼʹ��
UENUM()
enum class EnumWeather2 : uint8 {
	ENone,
	ESunny = 3,
	ECloudy,
	EWindy,
};

// 4. �޷��ṩ����ͼʹ��
UENUM()
namespace EnumWeather3 {
	enum Type {
		ENone,
		ESunny = 3,
		ECloudy,
		EWindy,
	};
}

// 5. ���׷�����ͻ
UENUM()
enum EnumWeather4 {
	ENone,
	ESunny = 3,
	ECloudy,
	EWindy,
};

// 6. �������EnumWeather4�ж����ö��ֵ��ͻ ������

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
