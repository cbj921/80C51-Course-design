# 80C51-Course-design
80C51课程设计——通用钟表
========================

该工程为 KEIL4 工程，需用 keil4 来打开目录下的 project 文件，即可以进行编译和编写；
------------------------------

#功能已经全部完成，以下是基本操作：/<br>
串口助手用的是XCOM，串口发送一定要勾选 `发送新行`，接收要取消 `以16进制显示`。<br>
正常模式下，显示分秒，按键`S1（P3^4）`为显示`年份`，按键`S2（P3^5）`为显示`月日`；<br>

1.校对时间的格式  `YYYY/MM/DD-HH/mm/ss` 分别是 `年/月/日-时/分/秒`<br>
<br>
2.判断是否为闰年格式   `YYYY年是闰年吗？`<br>
<br>
3.判断为星期几格式     `YYYY年MM月DD日是星期几？`<br>
<br>
4.判断某年某月有几个工作日格式    `YYYY年MM月有几个工作日` 注意该问题不能加问号！<br>
<br>
5.设定闹钟格式               `YYYY年MM月DD日HH时mm分时通过蜂鸣器提醒我！`<br>
	蜂鸣器响，可通过`S3（P3^6）`来关闭蜂鸣器。<br>
<br>
6.通过按键`S4（P3^7）`进入码表模式，码表模式下：<br>
		S1开启码表计数<br>
		S2清零码表并暂停<br>
		S3退出码表<br>
		S4计时并发送至串口<br>


