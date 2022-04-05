# 简单性能分析

## 会话前端(SessionFrontEnd)
1. 下载ue4-cpp-tutorial/Lesson_Stat工程
2. 然后设置为独立进程游戏:  
![ddd](/Lesson_Stat/img/step10.png)
3. 点击上面的高级设置添加启动参数 -NoVerifyGC -NoAILogging:  
![ddd](/Lesson_Stat/img/step2.png)
4. 游戏启动后，点击窗口，并按~键，可以输入stat startfile来开始统计数据
5. 游戏启动17秒后，当左上角出现文字时可以输入stat stopfile来结束统计数据:  
![ddd](/Lesson_Stat/img/step6.png)
6. 查看统计的数据可以打开会话前端:  
![ddd](/Lesson_Stat/img/step1.png)
7. 加载上面统计的数据  
![ddd](/Lesson_Stat/img/step4.png)
8. 找到左边代码里定义的DanProf组，双击组内要显示的统计数据，红色的内存线条不断上升，并随后出现断崖表示清理完内存，蓝色的线条表示操作的统计数据，可以查看调用栈和性能据:  
![ddd](/Lesson_Stat/img/stat.png)
  
  
## UnrealInsights
1. 在头文件Engine\Source\Runtime\Core\Public\Stats\StatsTrace.h修改该行以启动额外信息统计，重新编译UE4和UnrealInsights:
``` c++
    #define EXPERIMENTAL_STATSTRACE_ENABLED 1
```
![ddd](/Lesson_Stat/img/step14.png)
2. 启动\Engine\Binaries\Win64\UnrealInsights.exe，勾选自动实时分析(Auto-start analysis for LIVE trace ssessions)  
3. 参考上文给独立进程游戏添加启动参数 -trace=cpu,frame,log,bookmark,stats -statnamedevents  
4. 结果如下是GameThread，ReaderThread在某帧里的函数调用信息:   
![ddd](/Lesson_Stat/img/step13.png)
4. 结果如下是我们源代码里添加的统计函数的调用关系和时间消耗情况:  
![ddd](/Lesson_Stat/img/step11.png)
5. 结果如下是我们源代码里的内存分配和释放的统计情况:  
![ddd](/Lesson_Stat/img/step12.png)
