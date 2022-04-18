# 行为树
[文档位置](https://docs.unrealengine.com/4.27/zh-CN/InteractiveExperiences/ArtificialIntelligence/BehaviorTrees/BehaviorTreeNodeReference/BehaviorTreeNodeReferenceTasks/)

## 常用合成节点
合成节点（Composite）定义分支的根，以及执行该分支的基本规则，可以对其应用装饰器（Decorators）节点，从而修改进入它们分支的条目，甚至取消执行中的条目，此外，它们还可以连接服务（Services）节点，这些服务节点只有在合成节点的子节点正在被执行时才会激活

1. 选择器（Selector）:  
   从左到右的顺序执行其子节点，当其中一个子节点执行成功时，选择器节点将停止执行，如果选择器的一个子节点成功运行，则选择器运行成功，如果选择器的所有子节点运行失败，则选择器运行失败
2. 序列（Sequence）:  
   节点按从左到右的顺序执行其子节点。当其中一个子节点失败时，序列节点也将停止执行。如果有子节点失败，那么序列就会失败。如果该序列的所有子节点运行都成功执行，则序列节点成功

## 常用装饰器节点

连接到合成（Composite）或任务（Task）节点，作为条件判断判断单个节点是否可以执行
1. 比较重要的是属性Notify Observer，用来设置重新评估条件，On Result Change就是条件变化，比如条件为值 > 7，而On Value Change则是值发生改变，比如从3变成7
2. 而属性Observer Aborts，是当重新评估发生时，条件不再满足时，之前的任务的仍然运行，是终止该任务还是不管

## 服务
服务（Service）节点通常连接至合成（Composite）节点或任务（Task）节点，只要其分支被执行，它们就会以定义的频率执行，这些节点常用于检查和更新黑板

## 任务
任务节点的功能是实现操作，例如移动AI或调整黑板值。可以连接装饰器（Decorators）节点或服务（Services）节点到任务上，作为执行的条件或者额外的服务