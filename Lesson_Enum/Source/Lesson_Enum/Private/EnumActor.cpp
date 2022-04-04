// Fill out your copyright notice in the Description page of Project Settings.


#include "EnumActor.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

// Sets default values
AEnumActor::AEnumActor()
	: W(EnumWeather::ESunny) 
	, W1(EnumWeather1::ESunny)
	, W2(EnumWeather2::ESunny)
	, W3(EnumWeather3::Type::ESunny) 
	, W4(EnumWeather4::ESunny) {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnumActor::BeginPlay() {
	Super::BeginPlay();
	
	check(this->W == EnumWeather::ESunny);
	check(uint8(this->W) == uint8(this->W1));
	this->W1 = EnumWeather1::EWindy;
	check(uint8(this->W1) == uint8(5));
	check(uint8(this->W2) == uint8(3));
	check(uint8(this->W3) == uint8(3));
	this->W3 = EnumWeather3::Type::EWindy;
	check(uint8(this->W3) == EnumWeather3::Type::EWindy);
	check(this->W4 == EnumWeather4::ESunny);

	UKismetSystemLibrary::PrintString(nullptr, "all test passed");
}

// Called every frame
void AEnumActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

