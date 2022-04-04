// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StatActor.generated.h"

UCLASS()
class LESSON_STAT_API AStatActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStatActor();

	void FakeOP(float InMs = 1.f) const;

	void FakeMemoryAlloc(int32 InId);
	void FakeMemoryClean();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	class A {
	public:
		A(int32 InId = 1001, const FString& InName = FString(TEXT("DAN")))
			: Id(InId)
			, Name(InName) {
		}
	private:
		int32 Id;
		FString Name;
	};

	TMap<int32, A*> RawPtrs;
	int32 BaseId;
};
