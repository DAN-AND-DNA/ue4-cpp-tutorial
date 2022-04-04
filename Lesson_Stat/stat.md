# 简单性能分析
1. 下载ue4-cpp-tutorial/Lesson_Stat工程
2. 然后设置为独立进程游戏:  
![ddd](/stat/img/step10.png)
3. 点击上面的高级设置添加启动参数 -NoVerifyGC -NoAILogging:  
![ddd](/stat/img/step2.png)
3. 游戏启动后，点击窗口，并按~键，可以输入stat startfile来开始统计数据
4. 游戏启动17秒后，当左上角出现文字时可以输入stat stopfile来结束统计数据:  
![ddd](/stat/img/step6.png)
5. 查看统计的数据可以打开会话前端:  
![ddd](/stat/img/step1.png)
6. 加载上面统计的数据  
![ddd](/stat/img/step4.png)
7. 找到左边代码里定义的DanProf组，双击组内要显示的统计数据，红色的内存线条不断上升，并随后出现断崖表示清理完内存，蓝色的线条表示操作的统计数据，可以查看调用栈和性能据:  
![ddd](/stat/img/stat.png)
