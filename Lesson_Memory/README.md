# 虛幻的内存

## 内存分配

1. 在虚幻中，我们可以通过FMemory向操作系统申请和释放内存，FMemory类似统一接口层，API非常类似c语言中的malloc和free，其底层有很多不同的内存管理方案可以选择，由于虚幻主要是靠c++编写，所以也重写了new和delete使得用户可以在高层直接使用FMemory来申请内存和初始化对象，这么做可以提高分配性能，减少内存碎片化，监控内存的分配，减少泄漏  
2. 不同平台所能支持的内存管理方案不同，所以虚幻提供了不同的分配器可供选择，在文件GenericPlatformMemory.h中提供了如下的宏，其中Ansi为标准的c内存分配器，Binned系统为虚幻自带的分配器:  
    ``` c++
	/** Which allocator is being used */
	enum EMemoryAllocatorToUse
	{
		Ansi, // Default C allocator
		Stomp, // Allocator to check for memory stomping
		TBB, // Thread Building Blocks malloc
		Jemalloc, // Linux/FreeBSD malloc
		Binned, // Older binned malloc
		Binned2, // Newer binned malloc
		Binned3, // Newer VM-based binned malloc, 64 bit only
		Platform, // Custom platform specific allocator
		Mimalloc, // mimalloc
	};

    ```
3. 在UnrealMemory.cpp文件中，虚幻在FMemory_GCreateMalloc_ThreadUnsafe函数中创建了对应平台的全局内存分配器GMalloc，流程如下:
    ``` c++
    /** Helper function called on first allocation to create and initialize GMalloc */
    static int FMemory_GCreateMalloc_ThreadUnsafe()
    {
    #if !PLATFORM_MAC
	    FPlatformMemoryStats Stats = FPlatformMemory::GetStats();
	    uint64 ProgramSize = Stats.UsedPhysical;
    #endif

	    GMalloc = FPlatformMemory::BaseAllocator();
        ......
    ```

4. new和delete操作符都是在模块实现和注册时就被重写了，比如调用了new，从其调用栈可以看到，实际调用的是ModuleBoilerplate.h中的版本:  
![ddd](/Lesson_Memory/img/step2.png)  
![ddd](/Lesson_Memory/img/step1.png)  
5. 并且最终调用的内存分配器是TBB的FMalloc::Malloc函数，实际上虚幻里的智能指针、TArray、FString、TMap和UObject等都是类似的流程:  
![ddd](/Lesson_Memory/img/step3.png)  

## 内存剖析
虚幻提供的常用内存剖析工具有LLM(Low Level Memory Tracker)、Memreport和MallocProfiler等，其中比较新的是LLM即低级内存跟踪器，

## LLM
1. LLM是利用栈顶来标记当前作用域范围，将栈顶所在作用域范围内的FMemory所分配的内存统计到指定的标签下，最后结束时，再将整个栈所统计到的内存汇总到每个指定的标签下，未有关联标签的内存记到untagged标签下，这样就不会漏掉任何的分配内存
2. LowLevelMemTracker.h头文件中定义很多不同的标签，并给出了标签的起始范围，目前我们只关心项目标签:
    ```c++
    /*
    * Enum values to be passed in to LLM_SCOPE() macro
    */
    enum class ELLMTag : LLM_TAG_TYPE
    {
    #define LLM_ENUM(Enum,Str,Stat,Group,Parent) Enum,
	    LLM_ENUM_GENERIC_TAGS(LLM_ENUM)
    #undef LLM_ENUM

	    GenericTagCount,

	    //------------------------------
	    // Platform tags
	    PlatformTagStart = 100,
	    PlatformTagEnd = 149,

	    //------------------------------
	    // Project tags
	    ProjectTagStart = 150,
	    ProjectTagEnd = 255,

	    // anything above this value is treated as an FName for a stat section
    };
    ```

3. 简单使用可以直接添加启动参数 -LLM -LLMCSV:  
![ddd](/Lesson_Memory/img/step4.png)
![ddd](/Lesson_Memory/img/step5.png)
4. 启动后会在saved/profiling/llm/中输出类似LLM_Pid15256_2022.04.12-00.51.40_Windows.csv的csv文件，是调用FMemory的Malloc的内存分配情况，也可以输入按~建在控制台输入stat LLMFULL得到下面的结果:  
![ddd](/Lesson_Memory/img/step6.png)
5. 启动后会在saved/profiling/llm/中输出类似LLMPlatform_Pid15256_2022.04.12-00.51.40_Windows.csv的csv文件，这是是从系统进行内存分配的情况，也可以输入按~建在控制台输入stat LLMPlatform得到下面的结果:  
![ddd](/Lesson_Memory/img/step7.png)
6. 每个tag的详细介绍可以查询[资料](https://www.cnblogs.com/kekec/p/14593345.html)
7. 我们可以自定义自己的tag在LLMFULL中进行统计展现，例子如下:
    ```c++
    IMPLEMENT_PRIMARY_GAME_MODULE(FLessonMemoryModule, Lesson_Memory, "Lesson_Memory" );

    DECLARE_LLM_MEMORY_STAT(TEXT("DanMemProf"), STAT_DanMemProfLLM, STATGROUP_LLMFULL);


    void FLessonMemoryModule::StartupModule(){
	    LLM(FLowLevelMemTracker::Get().RegisterProjectTag((int32)EMyLLMTag::DanMemProf, TEXT("DanMemProf"), GET_STATFNAME(STAT_DanMemProfLLM), NAME_None));
    }

    ```
8. 使用tag的例子:  
   ```c++
   void AMemoryActor::Tick(float DeltaTime) {
	    {
		    LLM_SCOPE(ELLMTag((int)(EMyLLMTag::DanMemProf)));
		    Ptra = new(A);
		    //delete Ptra;
	    }
    }
   ```
9. 输出的csv文件包含每个标记的列，以MB为单位显示当前值。默认每隔5秒记录一下，可以使用LLM.LLMWriteInterval在控制台变量更改，这里可以发现DanMemProf统计的内存出现明显泄漏:  
![ddd](/Lesson_Memory/img/step8.png)