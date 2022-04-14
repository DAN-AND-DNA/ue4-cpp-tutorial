# 垃圾回收

简单的说，虚幻里的垃圾回收算法是在GC条件达成时（比如内存不足、GC间隔时间或者手动强制），分成Mark和Sweep两阶段，首先会暂停程序执行，然后开始从根节点开始遍历全部对象，判断是否可到达，然后对引用失效的对象进行清理、析构、释放内存和将指针赋值为nullptr，但普通的c++对象、UStruct对象和FGCObject对象都不在GC管理范围内，需要手动或者依靠智能指针管理其生命周期

## UPROPERTY

UStruct和UObject都可以用UPROPERTY来修饰他们的成员变量，但UStruct没法使用UFUNCTION来修饰他们的成员函数，这些修饰符最后都会拿去生产底层反射信息和GC信息

## UObject

1. 可以通过NewObject来创建新UObject的对象实例，并且可以通过UPROPERTY来修饰UObject类型的成员变量，防止下次GC发生时该成员变量自动被清理，但不可以用智能指针来管理UObject的对象:  

   ```c++
   // .h
   private:
        UPROPERTY()
	    UEasyObj* Obj1; 

	    UEasyObj* Obj2;

    // .cpp
    this->Obj1 = NewObject<UEasyObj>(this, UEasyObj::StaticClass());
    this->Obj2 = NewObject<UEasyObj>(this, UEasyObj::StaticClass());

    check(IsValid(this->Obj1));
    check(IsValid(this->Obj2));
   ```


2. 如果需要手动清理，我们可以将UObject的成员变量赋值为nullptr，或调用ConditionalBeginDestroy或MarkPendingKill来发起对UObject对象的异步清理:  
   ```c++

	this->Obj1 = NewObject<UEasyObj>(this, UEasyObj::StaticClass());
	this->Obj2 = NewObject<UEasyObj>();
	this->Obj3 = NewObject<UEasyObj>();

	check(IsValid(this->Obj1));
	check(IsValid(this->Obj2));
	check(IsValid(this->Obj3))

	this->Obj1 = nullptr;
	
	this->Obj2->ConditionalBeginDestroy();
	this->Obj2 = nullptr;

	this->Obj3->MarkPendingKill();
	this->Obj3 = nullptr;

	check(!IsValid(this->Obj1));
	check(!IsValid(this->Obj2));
	check(!IsValid(this->Obj3));

   ```
3. 但如果想要无UPROPERTY修饰的UObject成员变量或者局部创建的UObject不被GC清理，可以AddToRoot来将该UObject对象添加到根节点，而RemoveFromRoot则将UObject对象移除等待GC清理:  
   ```c++
   	UEasyObj* Obj4 = NewObject<UEasyObj>();
	Obj4->AddToRoot();

	Obj4->RemoveFromRoot();
	Obj4 = nullptr;
   ```

## UStruct

1. 对于类型为UStruct的成员变量，无法通过UPROPERTY修饰指针类型，和原始的c++的struct类型一样，在栈上创建的会自动清理，而通过malloc等方法创建，需要手动清理和回收:  
    ```c++
    // .h
    UPROPERTY()
	FEasyStruct StructEasy1;

	FEasyStruct* StructEasy2;

    // .cpp
    this->StructEasy1 = FEasyStruct{};

    this->StructEasy2 = (FEasyStruct*)(FMemory::Malloc(sizeof(FEasyStruct)));
    check(this->StructEasy2 != nullptr);
    FMemory::Free(this->StructEasy2);
    this->StructEasy2 = nullptr;
    check(this->StructEasy2 == nullptr);

	auto StructEasy3 = (FEasyStruct*)(FMemory::Malloc(sizeof(FEasyStruct)));
	check(StructEasy3 != nullptr);
    FMemory::Free(StructEasy3);
	StructEasy3 = nullptr;
    ```

## 非UObject对象
1. 普通的c++类或者结构体还是c++里的规则

## FGCObject
1. 比如一个普通的c++对象里有一个UObject的成员，这个成员肯定会被GC回收清理，如果想要该UObject成员不被GC清理，则需要普通的c++类继承自FGCObject，然后重写AddReferencedObjects:
    ```c++
    // .h
    class FEasyGcObj : public FGCObject {
    public:
	    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    private:
	    class UEasyObj* Obj1;
    };

    UCLASS()
    class UEasyObj : public UObject {
	    GENERATED_BODY()
    public:
	    UEasyObj();
    };

    // .cpp
    void FEasyGcObj::AddReferencedObjects(FReferenceCollector& Collector) {
	    Collector.AddReferencedObject(this->Obj1);
    }

    ```

## 强制GC
```c++
GEngine->ForceGarbageCollection(true);
```

## 有效性判断
```
    IsValid
```

## UObject添加自己的清理逻辑

1. 重写BeginDestroy:
