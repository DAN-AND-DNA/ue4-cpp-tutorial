# 资源加载

打包时，每种资源都会被序列化成二进制并压缩到多个pkg包中，当需要使用某种资源时，再反序列化成程序可使用的对象，又因为每个虚幻中的资源都有一个唯一路径，所以很容易通过api去加载路径对应的资源到内存

## 资源分类
1. Native Class在游戏启动时自动加载，并一直常驻内存
2. Blueprint Class既是类型，也是资源。可动态加载与卸载
3. 常见资源有Level（关卡）、UI（UMG）、Particle System（粒子）、Texture（贴图）、Mesh（网格）、Material（材质）、Animation（动画）、Foliage（植被）、Physics（物理）、AI（人工智能）、Sound（声音）和Sprite（精灵）等，详细可以参考[这里](https://www.cnblogs.com/kekec/p/14719018.html)

## 硬引用

1. 比如加载A对象，其成员变量中明确标识需要引用其他的资源，故必须保证这些依赖的资源都先于A加载完毕才算完成A的加载，如果这样的硬引用过多会导致内存消耗:  

    ```c++
    UCLASS()
    class LESSON_LOAD_API ADoLoadActor : public AActor {
	    GENERATED_BODY()
    public:
	    UPROPERTY(BlueprintReadWrite, EditAnywhere)
	    class UTexture2D* Icon;

	    UPROPERTY(BlueprintReadWrite, EditAnywhere)
	    class UStaticMesh* Body;

	    UPROPERTY(BlueprintReadWrite, EditAnywhere)
	    TSubclassOf<AActor> Hero;

	    UPROPERTY(BlueprintReadWrite, EditAnywhere)
	    TSubclassOf<UMyObject> NPC;

	    UPROPERTY(BlueprintReadWrite, EditAnywhere)
	    UClass* Enemy;
    };
    ```

2. 因为是属性中指定是EditAnywhere，所以实例和蓝图都可以编辑，结果如下:  
![ddd](/Lesson_Load/img/step2.png)
![ddd](/Lesson_Load/img/step1.png)

## 软引用

1. 跟硬引用相反，在业务需要时须手动去加载的资源，当资源不在被引用时，会随着GC进行清理:
    ```c++
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowedClasses = "Texture2D"))
	FSoftObjectPath SoftIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (MetaClass = "Actor"))
	FSoftClassPath SoftHero;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UTexture2D> SoftIcon2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftClassPtr<AActor> SoftHero2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftClassPtr<UClass> SoftEnemy1;
    ```
2. 其中FSoftClassPath继承自FSoftObjectPath，TSoftObjectPtr则是包含了FSoftObjectPath的TWeakObjectPtr，FSoftClassPath只能软引用Class、DynamicClass、BlueprintGeneratedClass、WidgetBlueprintGeneratedClass、AnimBluprintGeneratedClass和LinkerPlaceholderClass六大类，而FSoftObjectPath可以软引用任何资源和UClass，详细内容可以参考[这里](https://www.cnblogs.com/kekec/p/13357937.html)
3. 引用查看器中的结果如下，其中紫色的线表示软引用，白色线代表硬引用:  

![ddd](/Lesson_Load/img/step3.png)
![ddd](/Lesson_Load/img/step4.png)

## 同步加载
1. 硬引用中指的资源可以在UObject的构造函数中加载，蓝图类型和普通资源的字符串路径不同:  
	```c++
	ADoLoadActor::ADoLoadActor()
		: Icon2(nullptr)
		, Hero2(nullptr) {

		check(!IsValid(Icon2));
		auto EasyTextureFinder = ConstructorHelpers::FObjectFinder<UTexture2D>(TEXT("Texture2D'/Game/Texture/T_UI_ShowMove.T_UI_ShowMove'"));
		if (EasyTextureFinder.Succeeded()) {
			this->Icon2 = EasyTextureFinder.Object;
		}
		check(IsValid(Icon2));

		check(!IsValid(Hero2));
		static ConstructorHelpers::FClassFinder<AActor>  EasyClassFinder = ConstructorHelpers::FClassFinder<AActor>(TEXT( "/Game/Blueprint/BP_Hero.BP_Hero_C"));

		if (EasyClassFinder.Succeeded()) {
			this->Hero2 = EasyClassFinder.Class;
		}
	
		check(IsValid(Hero2));
	}

	```
效果如下，其中Icon2和Hero2是我们在上面赋值的:
![ddd](/Lesson_Load/img/step5.png)

3. 可以在使用LoadObject和LoadClass来同步加载资源和UClass，但不推荐，该方式不能被Reference Viewer工具识别，资源进行移动、改名等操作时，不会自动调整。也不能自动cook进安装包
     
4. FSoftObjectPath和FSoftClassPath的同步加载:
   ```c++
   void ADoLoadActor::Load() {
		auto RawIcon = this->SoftIcon.TryLoad();
		if (RawIcon != nullptr) {
			UTexture2D* NewIcon = Cast<UTexture2D>(RawIcon);
			check(IsValid(NewIcon));
		}

		UClass* RawHero = this->SoftHero.TryLoadClass<AActor>();
		if (RawHero != nullptr) {
			check(IsValid(RawHero));
		}
   }
   ```
5. TSoftObjectPtr和TSoftClassPtr的同步加载:  
   ```c++
   // .h
   	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UTexture2D> SoftIcon2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftClassPtr<AActor> SoftHero2;

   // .cpp
   if (!this->SoftIcon2.IsPending()) {
		check(IsValid(this->SoftIcon2.Get()));
	}

	auto NewIcon2 = SoftIcon2.LoadSynchronous();
	check(IsValid(NewIcon2));

	if (!this->SoftHero2.IsPending()) {
		check(IsValid(this->SoftHero2.Get()));
	}
	
	auto NewHero2 = SoftHero2.LoadSynchronous();
	check(IsValid(NewHero2));
   ```
6. TODO FStreamableManager
## 异步加载
TODO
FStreamableManager

## 关卡的加载
TODO