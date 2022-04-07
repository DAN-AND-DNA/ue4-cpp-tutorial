// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThreadActor.generated.h"

DECLARE_DELEGATE_OneParam(FTaskDelegate_OnSomethingDone, const FString& /*OutResult*/);
DECLARE_DELEGATE_OneParam(FTaskDelegate_OnForkJoinDone, const FString& /*OutResult*/);

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

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Async ForkJoin"))
	void K2_TaskGraph_AsyncForkJoin(const FString& InArgs);		// TaskGraph

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On ForkJoin Done"))
	void K2_TaskGraph_OncForkJoinDone(const FString& OutResult); // TaskGraph


protected:

private:
	//FTaskContext* Ctx;
};