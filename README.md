=视频介绍（推荐）
[bilibili](https://www.bilibili.com/video/BV1Bf4y1S7gC? "bilibili")
=图文介绍
![](https://www.arduino.cn/data/attachment/forum/202004/23/232852mriwxgtfzckgxggk.jpg)
全部组建分为三部分，控制面板，单片机和稳压电池壳
![](https://www.arduino.cn/data/attachment/forum/202004/23/232859z1k0do58td0y0i00.jpg)
它拥有一个番茄钟所该有的标准功能，25分钟的工作计时和5分钟的休息计时奖励
当然你也可以自行设置计时时间
![](https://www.arduino.cn/data/attachment/forum/202004/23/233700ayiyrvnksah31ovi.png)
![](https://www.arduino.cn/data/attachment/forum/202004/23/233801voc6h6o621e5h2h2.png)
设置完成后会自动保存到EEPROM内部，自动检验修复机制能确保你的番茄钟，学习时间，积分正常使

##音效设置
![](https://www.arduino.cn/data/attachment/forum/202004/23/234114bj4tys7yk14x676k.png)
![](https://www.arduino.cn/data/attachment/forum/202004/23/234141pxzwbaaaw5zdc6jw.png)
##省电设置
![](https://www.arduino.cn/data/attachment/forum/202004/23/235102exp6lylx5l4cjc83.png)
![](https://www.arduino.cn/data/attachment/forum/202004/23/234251mjumsbidai4dsupm.png)
在任何模式中省电模式始终优先，如果超过设定时间控制面板没有任何操作番茄钟本体会进入待机状态，但保持显示。在看门狗再次醒来前将无法使用控制面板，同时面板背光自动关闭
即使如此我们依然不推荐您使用atmega控制器写入这个程序，因为功耗比较大，可以选择采用其他低功耗控制器实现它
##按键操作方式&音效商店
![](https://www.arduino.cn/data/attachment/forum/202004/23/235535klxyt2h3l2xhc33l.png)
为了防止误触发和实现确认、返回功能，一个按钮按下后根据按下时间分为3个阶段，同时控制面板上端有长条可以达到提示效果。一般而言部分高危设置确认以及进入上层菜单(UP)需要长按才能进入，普通操作如暂停计时，选择功能，确认可以短按按键，最后如果想返回下一级（DOWN）或者取消操作，轻轻按一下就可以了。
![](https://www.arduino.cn/data/attachment/forum/202004/24/000238dkjhheyngkhiekh0.png)
音效商店还内置播放器，允许进行试听
通常 按一下为停止播放，短按播放，长按确认购买
![](https://www.arduino.cn/data/attachment/forum/202004/24/000246d7nuxcbbibxzmk5c.png)
为了激励使用番茄钟进行学习，在最低等级你需要学习满25分钟才可以获得5个番茄用于更换音效，系统内置了超过5款音效可供选择，包含一些名牌大学的上下课铃可供使用，达到更好激励学习，及时反馈，获得成就的目的。
![](https://www.arduino.cn/data/attachment/forum/202004/24/000252n4ejie7no0mxy4fo.png)
由于我是临时刷程序进行演示，我并没有足够的番茄来购买音效（开发者DEBUG模式除外***需要修改程序进入***）
##番茄计时
![](https://www.arduino.cn/data/attachment/forum/202004/24/000939kttmk8blkaglba1e.png)
机器提供两种计时模式，一种是番茄钟紧迫倒计时，另外是正计时，上图为番茄钟倒计时模式
![](https://www.arduino.cn/data/attachment/forum/202004/24/000946vhb6msg684jcxu1q.png)
上图为正计时模式
![](https://www.arduino.cn/data/attachment/forum/202004/24/000954qmb5qtyqjbu4iqoi.png)
计时学习结束屏幕闪烁，同时播放设定好的铃声(默认旧版清华上课铃 车尼尔 OP599 NO.59)，如果有休息环节则进入休息环节
*休息也是番茄学习的重要组成部分，5分钟的休息可以使下一轮学习更高效，当然你可以选择跳过，但是如果这样你将失去休息带来的番茄奖励

##激励练级系统
休息结束后自动显示本阶段学习的收获，包含学习时长，等级，以及番茄代币
![](https://www.arduino.cn/data/attachment/forum/202004/24/001618u7w97nnepeerjp6n.png)
等级数据是通过学习时间计算的
一开始3天升级，到后面3周升级...1个月...1年
总之努力起来吧，为建设人类命运共同体奋斗
![](https://www.arduino.cn/data/attachment/forum/202004/24/001623vcu7eff227tt24cs.png)
等级的提高，每轮学习的番茄收益将有不同等级的提升
同时提示符会从初级的**'$'**升级为**'#'**
