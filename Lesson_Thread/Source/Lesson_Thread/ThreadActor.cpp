// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreadActor.h"

//===================== GraphTask =====================
/*
	Task_Something - Task_OnSomethingDone
*/

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

	TaskDelegateOnAutoDelAsyncTaskDone.BindUObject(this, &AThreadActor::K2_AysncTask_OnAutoDelTaskDone);
	TaskDelegateOnSomethingDone.BindUObject(this, &AThreadActor::K2_TaskGraph_OnSomethingDone);
	TaskDelegateOnManualDelAsyncTaskDone.BindUObject(this, &AThreadActor::K2_AysncTask_OnManualDelTaskDone);
}

void AThreadActor::BeginDestroy() {
	Super::BeginDestroy();

	TaskDelegateOnAutoDelAsyncTaskDone.Unbind();
	TaskDelegateOnSomethingDone.Unbind();
	TaskDelegateOnManualDelAsyncTaskDone.Unbind();
}

void AThreadActor::K2_TaskGraph_AsyncDoSomething(const FString& InArg) {
	//FTaskDelegate_OnSomethingDone TaskDelegate;

	TGraphTask<class FTask_Something>::CreateTask(nullptr, ENamedThreads::AnyThread).ConstructAndDispatchWhenReady(InArg, TaskDelegateOnSomethingDone);	// 创建和分配任务
}



/*
						FTask_ForkJoinSub1
Task_ForkJoinMain1	/						\	Task_ForkJoinMain2
					\						/
						FTask_ForkJoinSub2
*/

class FTaskContext {
public:
	~FTaskContext() {
		TaskDelegate.Unbind();
		UE_LOG(LogTemp, Log, TEXT("Context is destoryed")); 
	}
public:
	bool bRunning = false;
	FTaskDelegate_OnForkJoinDone TaskDelegate;
	FString InArg;
	FString Main1Result;
	FString Sub1Result;
	FString Sub2Result;
	FString Main2Result;
};



// ForkJoin 主任务1
class FTask_ForkJoinMain1 {

public:
	FTask_ForkJoinMain1(TSharedPtr<FTaskContext> InCtx)
		: Ctx(InCtx) {
	}

	// TGraphTask的TTask模板类型中需要实现如下4个函数
	FORCEINLINE  TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FTask_ForkJoinMain1, STATGROUP_TaskGraphTasks); }
	static ENamedThreads::Type GetDesiredThread() { return ENamedThreads::AnyThread; }
	static ESubsequentsMode::Type GetSubsequentsMode() { return ESubsequentsMode::TrackSubsequents; } // FireAndForget 会导致后续任务的依赖关系失败
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& Subsequents) {
		if (!Ctx->bRunning) {
			return;
		}

		Ctx->Main1Result = FString::Printf(TEXT("[main1]: %s"), *Ctx->InArg);
	}

private:
	TSharedPtr<FTaskContext> Ctx;
};

// ForkJoin 主任务2
class FTask_ForkJoinMain2 {

public:
	FTask_ForkJoinMain2(TSharedPtr<FTaskContext> InCtx)
		: Ctx(InCtx) {
	}

	// TGraphTask的TTask模板类型中需要实现如下4个函数
	FORCEINLINE  TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FTask_ForkJoinMain2, STATGROUP_TaskGraphTasks); }
	static ENamedThreads::Type GetDesiredThread() { return ENamedThreads::GameThread; }
	static ESubsequentsMode::Type GetSubsequentsMode() { return ESubsequentsMode::TrackSubsequents; } // FireAndForget 会导致后续任务的依赖关系失败
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& Subsequents) {
		if (!Ctx->bRunning) {
			return;
		}

		check(IsInGameThread());

		Ctx->Main2Result = FString::Printf(TEXT("[main2]: %s %s"), *Ctx->Sub1Result, *Ctx->Sub2Result);
		Ctx->TaskDelegate.ExecuteIfBound(Ctx->Main2Result);
	}

private:
	TSharedPtr<FTaskContext> Ctx;
};


// ForkJoin 子任务1
class FTask_ForkJoinSub1 {

public:
	FTask_ForkJoinSub1(TSharedPtr<FTaskContext> InCtx)
		: Ctx(InCtx) {
	}

	// TGraphTask的TTask模板类型中需要实现如下4个函数
	FORCEINLINE  TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FTask_ForkJoinSub1, STATGROUP_TaskGraphTasks); }
	static ENamedThreads::Type GetDesiredThread() { return ENamedThreads::AnyThread; }
	static ESubsequentsMode::Type GetSubsequentsMode() { return ESubsequentsMode::TrackSubsequents; } // FireAndForget 会导致后续任务的依赖关系失败
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& Subsequents) {
		if (!Ctx->bRunning) {
			return;
		}

		Ctx->Sub1Result = FString::Printf(TEXT("[sub1]: %s"), *Ctx->Main1Result);
	}

private:
	TSharedPtr<FTaskContext> Ctx;
};

// ForkJoin 子任务2
class FTask_ForkJoinSub2 {

public:
	FTask_ForkJoinSub2(TSharedPtr<FTaskContext> InCtx)
		: Ctx(InCtx) {
	}

	// TGraphTask的TTask模板类型中需要实现如下4个函数
	FORCEINLINE  TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FTask_ForkJoinSub2, STATGROUP_TaskGraphTasks); }
	static ENamedThreads::Type GetDesiredThread() { return ENamedThreads::AnyThread; }
	static ESubsequentsMode::Type GetSubsequentsMode() { return ESubsequentsMode::TrackSubsequents; } // FireAndForget 会导致后续任务的依赖关系失败
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& Subsequents) {
		if (!Ctx->bRunning) {
			return;
		}

		Ctx->Sub2Result = FString::Printf(TEXT("[sub2]: %s"), *Ctx->Main1Result);
	}

private:
	TSharedPtr<FTaskContext> Ctx;
};

void AThreadActor::K2_TaskGraph_AsyncForkJoin(const FString& InArg) {
	TSharedPtr<FTaskContext> Ctx(new FTaskContext);
	Ctx->bRunning = true;
	Ctx->InArg = InArg;
	Ctx->TaskDelegate.BindUObject(this, &AThreadActor::K2_TaskGraph_OncForkJoinDone);

	auto TaskMain1 = TGraphTask<FTask_ForkJoinMain1>::CreateTask(nullptr, ENamedThreads::AnyThread).ConstructAndDispatchWhenReady(Ctx);

	FGraphEventArray PreSub;
	PreSub.Add(TaskMain1);

	auto TaskSub1 = TGraphTask<FTask_ForkJoinSub1>::CreateTask(&PreSub, ENamedThreads::AnyThread).ConstructAndDispatchWhenReady(Ctx);
	auto TaskSub2 = TGraphTask<FTask_ForkJoinSub2>::CreateTask(&PreSub, ENamedThreads::AnyThread).ConstructAndDispatchWhenReady(Ctx);

	FGraphEventArray PreMain;
	PreMain.Add(TaskSub1);
	PreMain.Add(TaskSub2);

	auto TaskMain2 = TGraphTask<FTask_ForkJoinMain2>::CreateTask(&PreMain, ENamedThreads::AnyThread).ConstructAndDispatchWhenReady(Ctx);
}



//===================== Async Task=====================
class FAutoDelTask : public  FNonAbandonableTask {

public:
	FAutoDelTask(const FString& InName, FTaskDelegate_OnAutoDelAsyncTaskDone TaskDelegate)
		: Name(InName)
		, TaskDelegate(TaskDelegate) {
	}

	~FAutoDelTask(){}

	// 需提供如下2个函数
	void DoWork() {
		FPlatformProcess::Sleep(2);


		auto TaskDelegateCopied = this->TaskDelegate;
		auto NameCopied = this->Name;

		AsyncTask(ENamedThreads::GameThread, [TaskDelegateCopied, NameCopied]() {
			TaskDelegateCopied.ExecuteIfBound(FString::Printf(TEXT("Finish task: %s"), *NameCopied));
		});
		
	}
	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FAutoDelTask, STATGROUP_ThreadPoolAsyncTasks); }

private:
	friend class FAutoDeleteAsyncTask<FAutoDelTask>;
	FString Name;
	FTaskDelegate_OnAutoDelAsyncTaskDone TaskDelegate;
};

void AThreadActor::K2_AysncTask_AsyncDoAutoDelTask(const FString& InArg) {
	(new FAutoDeleteAsyncTask<FAutoDelTask>(InArg, this->TaskDelegateOnAutoDelAsyncTaskDone))->StartBackgroundTask();
}

class FManualDelTask : public  FNonAbandonableTask {

public:
	FManualDelTask(const FString& InName, FTaskDelegate_OnManualDelAsyncTaskDone TaskDelegate)
		: Name(InName)
		, TaskDelegate(TaskDelegate) {
	}

	~FManualDelTask() {}

	// 需提供如下2个函数
	void DoWork() {
		FPlatformProcess::Sleep(2);

		auto TaskDelegateCopied = this->TaskDelegate;
		auto NameCopied = this->Name;
		UE_LOG(LogTemp, Log, TEXT("do work"));
		
		AsyncTask(ENamedThreads::GameThread, [TaskDelegateCopied, NameCopied]() {
			TaskDelegateCopied.ExecuteIfBound(FString::Printf(TEXT("Finish task: %s"), *NameCopied));
		});
		
		return;
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FManualDelTask, STATGROUP_ThreadPoolAsyncTasks); }

private:
	friend class FAsyncTask<FManualDelTask>;
	FString Name;
	FTaskDelegate_OnManualDelAsyncTaskDone TaskDelegate;
};



void AThreadActor::K2_AysncTask_AsyncDoManualDelTask(const FString& InArg) {
	auto Task = new FAsyncTask<FManualDelTask>(InArg, this->TaskDelegateOnManualDelAsyncTaskDone);
	Task->StartBackgroundTask();

	GetWorldTimerManager().SetTimer(
		this->Timer, 
		FTimerDelegate::CreateLambda([Task, this]() {
			//UE_LOG(LogTemp, Log, TEXT("on time"));

			if (Task->IsDone()) {

				UE_LOG(LogTemp, Log, TEXT("is done"));
				
				Task->EnsureCompletion();
				delete Task;

				this->GetWorldTimerManager().ClearTimer(this->Timer);
			}
		}), 
		0.1f, 
		true
	);
}