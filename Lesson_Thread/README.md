# 虚幻的线程
 
这里介绍一些常见的线程，来作为性能分析和代码原理理解使用

## 线程的创建流程

进程启动，cpu都会执行其main函数，当然虚幻也不例外，在源代码的Engine\Source\Runtime\Launch目录里有各个平台的专属启动代码，一般是初始化完各平台的环境参数后就开始调用了公共部分的Launch.cpp中的GuardedMain函数进行虚幻自己的启动流程，其中就包含创建各种线程:  
![ddd](/Lesson_Thread/img/step1.png)

## NamedThread

在ENamedThreads::Type中定义的有名字的线程:  
1. GameThread即游戏线程，主线程，主要是来处理整个大的流程和更新游戏逻辑
2. RenderingThread即渲染线程，主要在游戏线程完成计算任务后重绘进行处理和优化
3. RHIThread单例即渲染硬件接口线程，主要为渲染线程进行异步底层图像API调用
4. StatsThread主要进行统计数据的收集
5. AudioThread主要进行声音相关的处理

## AnyThread

在ENamedThreads::Type中定义的没有名字工作线程:  
1. TaskGraphThread，主要作为TaskGraph中的工作线程来执行任务

## QueuedThreadPool

1. 传统意义上的线程池，由虚幻提前创建的大量线程，等待并执行队列上的任务，可以使用AsyncPool等API来进行任务指派

## 普通自定义线程

1. 传统意义上的线程，由用户自己创建和定义任务，比如虚幻里的FRunable

## TaskGraph的例子

1. TaskGraph是虚幻提供的并行编程框架，可以给创建的任务指定顺序和之间的依赖，即所谓的任务图
2. 创建的任务可以指定给某个线程执行
3. 官方代码中对任务定义的例程:  
   
``` c++
// 位置在Engine\Source\Runtime\Core\Public\Async\TaskGraphInterfaces.h

/** 
 The user defined task type can take arguments to a constructor. These arguments (unfortunately) must not be references.
 The API required of TTask:
**/

class FGenericTask
{
	TSomeType	SomeArgument;
public:
	FGenericTask(TSomeType InSomeArgument) // CAUTION!: Must not use references in the constructor args; use pointers instead if you need by reference
		: SomeArgument(InSomeArgument)
	{
		// Usually the constructor doesn't do anything except save the arguments for use in DoWork or GetDesiredThread.
	}
	~FGenericTask()
	{
		// you will be destroyed immediately after you execute. Might as well do cleanup in DoWork, but you could also use a destructor.
	}
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FGenericTask, STATGROUP_TaskGraphTasks);
	}

	[static] ENamedThreads::Type GetDesiredThread()
	{
		return ENamedThreads::[named thread or AnyThread];
	}
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	{
		// The arguments are useful for setting up other tasks. 
		// Do work here, probably using SomeArgument.
		MyCompletionGraphEvent->DontCompleteUntil(TGraphTask<FSomeChildTask>::CreateTask(NULL,CurrentThread).ConstructAndDispatchWhenReady());
	}
};
```

4. 这是我们定义的Something任务内容，代码里必须提供DoTask()函数，该函数会被GetDesiredThread()所返回的指定线程所运行，这里的AnyThread指的是任意的TaskGraph工作线程，GetSubsequentsMode()主要是针对是否存在后续任务，尽量使用TrackSubsequents来表示会存在后续任务，因为FireAndForget表示没有后续任务，但会容易因为使用了后续任务的相关操作而导致出现空指针或者其他错误:  
    
``` c++
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
```
6. 这里是分配任务
``` c++
void AThreadActor::K2_TaskGraph_AsyncDoSomething(const FString& InArg) {
	FTaskDelegate_OnSomethingDone TaskDelegate;

	TaskDelegate.BindUObject(this, &AThreadActor::K2_TaskGraph_OnSomethingDone);

	TGraphTask<class FTask_Something>::CreateTask(nullptr, ENamedThreads::AnyThread).ConstructAndDispatchWhenReady(InArg, TaskDelegate);	// 创建和分配任务
}

```





## AsyncTask的例子

1. AsyncTask可以指定NamedThread线程来执行任务

## FRunable的例子
