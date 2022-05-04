该版本说明：
1、修复了小程序门和窗按键不正常更新的bug
2、增加了继电器控制风扇功能
3、修复了质量传感器ADC读取（换了个AD引脚采集）
4、将与SIM900A的通信从串口1改为串口5
{
"results":[
	{"location":     {"id":"W7YGK0CKZKF9","name":"Maoming","country":"CN","path":"Maoming,Maomin g,Guangdong,China","timezone":"Asia/Shanghai","timezone_offset":"+08:00"},
	"now":               {"text":"Overcast","code":"9","temperature":"22"},
"last_update":"2022-03-29T16:10:16+08:00"}
	]
}

知心天气数据：
API私钥：Savcdz4uhjRLsSUOF
天气实况接口地址：https://api.seniverse.com/v3/weather/now.json?key=Savcdz4uhjRLsSUOF&location=maoming&language=zh-Hans&unit=c

ESP32指令：
连接WiFi部分：
	1、AT		     	//测试AT
	2、AT+CWMODE=1    	//设置为Station模式
	3、AT+RST	    	//模块重启
	4、AT+CIPMUX=0		//设置单路连接
	5、AT+CWJAP=“WiFi名"，”WIFI密码“	//连接WiFi
获取天气数据部分：
	6、AT+CIPSTART="TCP","api.seniverse.com",80	//和心知天气连接
	7、AT+CIPMODE=1	//开启透传模式
	8、AT+CIPSEND		//开始透传，出现”>“
	9、发送：GET https://api.seniverse.com/v3/weather/now.json?key=私钥&location=城市&language=zh-Hans&unit=c		//向网页请求天气数据
	



	