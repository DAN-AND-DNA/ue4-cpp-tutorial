// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreadActor.h"




// Something任务完成
class FTask_OnSomethingDone {
public:
	FTask_OnSomethingDone(const FString& OutResult, FTaskDelegate_OnSomethingDone TaskDelegate)
		: OutResult(OutResult)
		, TaskDelegate(TaskDelegate) {
	}

	// TGraphTask的TTask模板类型中需要实现如下4个函数
	FORCEINLINE  TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FTask_OnSomethingDone, STATGROUP_TaskGraphTasks); }
	static ENamedThreads::Type GetDesiredThread() { return ENamedThreads::GameThread; }
	static ESubsequentsMode::Type GetSubsequentsMode() { return ESubsequentsMode::TrackSubsequents; }  // FireAndForget 会导致任务的依赖关系失败
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& Subsequents) {
		check(IsInGameThread());

		this->TaskDelegate.ExecuteIfBound(this->OutResult);		// 通知游戏线程执行蓝图事件
	}

private:
	FString OutResult;
	FTaskDelegate_OnSomethingDone TaskDelegate;
};


// Something任务
class FTask_Something {

public:
	FTask_Something(const FString& InArg, FTaskDelegate_OnSomethingDone TaskDelegate)
		: InArg(InArg)
		, TaskDelegate(TaskDelegate) {
	}

	// TGraphTask的TTask模板类型中需要实现如下4个函数
	FORCEINLINE  TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FTask_Something, STATGROUP_TaskGraphTasks); }
	static ENamedThreads::Type GetDesiredThread() { return ENamedThreads::AnyThread; }
	static ESubsequentsMode::Type GetSubsequentsMode() { return ESubsequentsMode::TrackSubsequents; } // FireAndForget 会导致后续任务的依赖关系失败
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& Subsequents) {

		FString OutResult = FString::Printf(TEXT("Input is %s, so something is done !!!"), *InArg);
		Subsequents->DontCompleteUntil(TGraphTask<class FTask_OnSomethingDone>::CreateTask(nullptr, CurrentThread).ConstructAndDispatchWhenReady(MoveTemp(OutResult), TaskDelegate)); 
	}

private:
	FString InArg;
	FTaskDelegate_OnSomethingDone TaskDelegate;
};



// Sets default values
AThreadActor::AThreadActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AThreadActor::K2_TaskGraph_AsyncDoSomething(const FString& InArg) {
	FTaskDelegate_OnSomethingDone TaskDelegate;

	TaskDelegate.BindUObject(this, &AThreadActor::K2_TaskGraph_OnSomethingDone);

	TGraphTask<class FTask_Something>::CreateTask(nullptr, ENamedThreads::AnyThread).ConstructAndDispatchWhenReady(InArg, TaskDelegate);	// 创建和分配任务
}

