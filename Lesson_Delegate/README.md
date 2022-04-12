# 委托
官方文档的内容很详尽，这边简单的补充一些内容

## 单播
1. 单播定义，其中XXX可以用OneParam、TwoParams等来指定需要传入的函数的参数个数，但最多8个函数参数:  
DECLARE_DELEGATE(代理名)
DECLARE_DELEGATE_XXX(代理名, 参数类型)
DECLARE_DELEGATE_RetVal_XXX(返回值类型，代理名, 参数类型)

2. 单播的简单定义如下:
    ```c++
    struct FError {
	    int32 Code;
	    FString Message;
    };

    DECLARE_DELEGATE(FOnConnected_Delegate)
    DECLARE_DELEGATE(FOnGameStart_Delegate)
    DECLARE_DELEGATE(FOnGameOver_Delegate)
    DECLARE_DELEGATE(FOnDisconnected_Delegate)
    DECLARE_DELEGATE_OneParam(FOnSendError_Delegate, FError /*error*/)
    DECLARE_DELEGATE_OneParam(FOnReadError_Delegate, FError  /*error*/)
    DECLARE_DELEGATE_OneParam(FOnRead_Delegate, FString /*msg*/)
    DECLARE_DELEGATE(FOnSend_Delegate)
    ```

3. BindUObject是只能绑定UObject对象的成员函数，而BindUFunction是只能绑定由UFUNCTION标记的函数，因为委托绑定都不会影响绑定对象的清理和释放，故容易因为绑定的对象或者Lambda捕获的外部对象被无意销毁而出现内存错误，调用Execute建议使用ExecuteIfBound代替或者IsBound判断绑定其的对象是否还有效再调用，简单使用如下:  
    ```c++
    // 绑定
    void ADeActor::BeginPlay() {
	    Super::BeginPlay();

	    this->client4 = NewObject<UClient>(this, UClient::StaticClass());

	    OnConnected.BindStatic(FClient::HandleOnConnected);
	    OnDisconnected.BindSP(this->client1, &FClient::HandleOnDisconnected);
	    OnGameStart.BindUObject(client4, &UClient::HandleOnGameStart);
	    OnGameOver.BindThreadSafeSP(this->client3, &FClient::HandleOnGameOver);
	    OnSendError.BindRaw(this->client2.Get(), &FClient::HandleOnSendError);
	    OnReadError.BindLambda([](FError Error) {UE_LOG(LogTemp, Log, TEXT("HandleOnReadError")); });
	    OnRead.BindWeakLambda(client4, [](FString msg) {UE_LOG(LogTemp, Log, TEXT("HandleOnRead")); });
	    OnSend.BindUFunction(client4, STATIC_FUNCTION_FNAME(TEXT("UClient::HandleOnSend")));
    }

    // 解绑
    void ADeActor::BeginDestroy() {
	    Super::BeginDestroy();

	    OnConnected.Unbind();
	    OnDisconnected.Unbind();
	    OnGameStart.Unbind();
	    OnGameOver.Unbind();
	    OnSendError.Unbind();
	    OnReadError.Unbind();
	    OnRead.Unbind();
	    OnSend.Unbind();
    }

    // 通知
    void ADeActor::ExecAll() {
	    auto SendError = FError{ -1, TEXT("Send error") };
	    auto ReadError = FError{ -1, TEXT("Read error") };

	    OnConnected.ExecuteIfBound();
	    OnDisconnected.ExecuteIfBound();
	    OnGameStart.ExecuteIfBound();
	    OnGameOver.ExecuteIfBound();
	    OnSendError.ExecuteIfBound(MoveTemp(SendError));
	    OnReadError.ExecuteIfBound(MoveTemp(ReadError));
	    OnRead.ExecuteIfBound(TEXT("Send something"));
	    OnSend.ExecuteIfBound();
    }
    ```
## 组播
类似单播，但没有返回值，可以绑定多个函数进行组播，也会出现绑定的对象或者Lambda捕获的外部对象被无意销毁而出现内存错误:  
```c++
    // 定义
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnRoleCreated, const FString& /*name*/);

    // 绑定
    OnRoleCreated.AddUObject(client4, &UClient::HandleRoleCreated);

    // 解绑
    OnRoleCreated.RemoveAll(this->client4);
	check(!OnRoleCreated.IsBound());
	check(!OnRoleCreated.IsBoundToObject(this->client4));
	OnRoleCreated.Clear();

    // 广播
    check(OnRoleCreated.IsBound());
	check(OnRoleCreated.IsBoundToObject(this->client4));
	OnRoleCreated.Broadcast("Hero");
```

## 事件
类似组播，简单定义和使用如下:  
```c++
    
    // 定义
	DECLARE_EVENT_OneParam(ADeActor, FUpdatedEvent, const FString&)
	FUpdatedEvent& OnUpdated() { return OnUpdatedEvent; }

    // 绑定
    this->OnUpdatedEventHandle = this->OnUpdated().AddUObject(this->client4, &UClient::OnUpdatedCallback);

    // 清理
    this->OnUpdated().Remove(this->OnUpdatedEventHandle);
	this->OnUpdated().Clear();


    // 通知
    this->OnUpdated().Broadcast(TEXT(R"({"Name":"Dan","Level":99})"));
```

## 动态委托
可以暴露给蓝图使用，可以作为函数的参数:  
```c++
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnOnline, const FString&, Name);
	
UFUNCTION(BlueprintCallable)
void RegisterOnline(FOnOnline InOnline) { OnOnline = InOnline; }

OnOnline.ExecuteIfBound(TEXT("Dan"));
```
蓝图中使用:  
![ddd](/Lesson_Delegate/img/step2.png)

## 动态组播
可以暴露给蓝图进行绑定:  

```c++
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOffline, const FString&, Name);

UPROPERTY(BlueprintAssignable)
FOnOffline OnOffline;

OnOffline.Broadcast(TEXT("Lulu"));
```

蓝图中使用如下:  
![ddd](/Lesson_Delegate/img/step1.png)

## 混合使用的简单例子
1. 可以直接使用定义的委托类型来创建委托实例，比如:
    ```c++
    OnConnected.BindStatic 等价 OnConnected = FOnConnected_Delegate::CreateStatic(xxx)
    OnGameStart.BindUObject 等价 OnGameStart = FOnGameStart_Delegate::CreateUObject(xxx)
    ......
   ```
2. 用普通委托的CreateLambda函数可以捕获动态委托，很容易复用普通的委托提供给蓝图使用:  

```c++
    // 定义
    DECLARE_DYNAMIC_DELEGATE(FOnUnderAttacked);
    DECLARE_DELEGATE(FOnUnderAttacked_Delegate)

	UFUNCTION(BlueprintCallable, meta = (Displayname="RegisterUnderAttacked"))
	void K2_RegisterUnderAttack(FOnUnderAttacked InUnderAttacked);

	void RegisterUnderAttack(FOnUnderAttacked_Delegate InUnderAttacked);

    // 实现
    void ADeActor::K2_RegisterUnderAttack(FOnUnderAttacked InUnderAttacked) {
	    this->RegisterUnderAttack(FOnUnderAttacked_Delegate::CreateLambda([InUnderAttacked]() {
		    InUnderAttacked.ExecuteIfBound();
	    }));
    }

    void ADeActor::RegisterUnderAttack(FOnUnderAttacked_Delegate InUnderAttacked) {
	    OnUnderAttacked = InUnderAttacked;
    }
```