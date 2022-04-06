// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThreadActor.generated.h"

DECLARE_DELEGATE_OneParam(FTaskDelegate_OnSomethingDone, const FString& /*OutResult*/);

UCLASS()
class LESSON_THREAD_API AThreadActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThreadActor();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Async Do Something"))
	void K2_TaskGraph_AsyncDoSomething(const FString& Input);	// TaskGraph

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Something Done"))
	void K2_TaskGraph_OnSomethingDone(const FString& OutResult); // TaskGraph

protected:
};