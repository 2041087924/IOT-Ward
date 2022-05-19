/*
//  多线程基于FreeRTOS，可以多个任务并行处理；
//  ESP32具有两个32位Tensilica Xtensa LX6微处理器；
 */

#define BLINKER_WIFI            //设置连接的方式，这里为WIFI连接
#include <Arduino.h>
#include <Blinker.h>
#include <Wire.h>
#include "SSD1306Wire.h"  
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "Chinese_Font.h"
#include "Image.h"
#include "MAX30105.h"
#include "heartRate.h"

#define DHTPIN 25                  //定义温湿度传感器的引脚
#define DHTTYPE  DHT11             //温湿度传感器的型号 DHT 11
#define SDA 21                     //定义OLED引脚   SDA -> GPIO21
#define SCL 22                    //定义OLED引脚   SCL -> GPIO22

DHT_Unified dht(DHTPIN, DHTTYPE);    //实例化一个DHT11 
SSD1306Wire display(0x3c, SDA, SCL);   //实例化一个SSD1306Wire对象
MAX30105 particleSensor;

char auth[] = "ec32d83ead06";   //手机端注册的密钥
char ssid[] = "ESATA2_4";       //所连接wifi的名称，注意必须是2.4G的WiFi
char pswd[] = "dianxieniubi";   //所连 接WiFi的密码
float Humi_read = 0, Temp_read = 0;
int8_t sec;            //秒
int8_t minu;           //分
int8_t hour;           //时
int8_t mday;           //日
int8_t month;          //月
const uint8_t RATE_SIZE = 4; //增加该值以获得更多平均值。4是好的。
uint8_t rates[RATE_SIZE];    //心率数组
uint8_t rateSpot = 0;
long lastBeat = 0;           //最后一拍发生的时间
float beatsPerMinute;
int beatAvg;
uint8_t xinlv_count;
uint8_t yanwu_state = 1;
uint8_t huoyan_state = 1;
uint8_t WIFi_state = 0;
uint16_t Time_state_count ;     //时间显示状态的计数
uint16_t Sound_count ;
uint16_t Guangmin_count;
uint8_t LED_state;
uint8_t Temp_state;
uint8_t Humi_state;
uint8_t i;




// 新建组件对象，对应app上的一个对象
BlinkerButton Button1("btn-abc");   //按键组件
BlinkerButton Button2("btn-3nd");   //空调组件
BlinkerButton Button3("btn-u1p");   //加湿器组件
BlinkerNumber TEMP("num-kon");      //温度组件
BlinkerNumber HUMI("num-ebi");      //湿度组件
BlinkerNumber Sound("num-qly");     //声音组件
BlinkerNumber Guangmin("num-845");  //光敏组件
BlinkerNumber Xinlv("num-9k4");     //心率组件
BlinkerText Text1("tex-35w");       //火焰报警组件
BlinkerText Text2("tex-iyo");       //烟雾报警组件
BlinkerText Text3("tex-avq");       //心率报警组件
void DHT11_Read_dat();              //温湿度读取
void GPIO_Init();  
void OLED_Display();  
void Time_Read();                   //向网络请求，获取时间
void Baojing();
void Sound_Read();
void Xinlv_Read();


// 如果未绑定的组件被触发，则会执行其中内容
void dataRead(const String & data)
{
  BLINKER_LOG("Blinker readString: ", data);
}

void heartbeat()     //回传数据给app组件
{
  TEMP.print(Temp_read);
  HUMI.print(Humi_read);
  Sound.print(Sound_count);
  Guangmin.print(Guangmin_count);
  Xinlv.print(xinlv_count);
  
}

void button1_callback(const String & state)    // 按下按键即会执行该函数
{
  BLINKER_LOG("get button state: ", state);
  
  if (state=="on") {
      digitalWrite(LED_BUILTIN, HIGH);  
      LED_state=1;   
      Button1.print("on");    // 反馈开关状态
    }
  else if (state=="off") {
      digitalWrite(LED_BUILTIN, LOW); 
      LED_state=0;
      Button1.print("off");   // 反馈开关状态
    } 
}

void button2_callback(const String & state)    // 按下按键即会执行该函数
{
  BLINKER_LOG("get button state: ", state);
  if (state=="off") {
      digitalWrite(16, LOW);
      Temp_state=0;  
      Button2.print("off");   // 反馈开关状态
    } 
  else if (state=="on") {
      digitalWrite(16, HIGH);  
      Temp_state=1;   
      Button2.print("on");    // 反馈开关状态
    }

}
void button3_callback(const String & state)    // 按下按键即会执行该函数
{
  BLINKER_LOG("get button state: ", state);
  if (state=="off") {
      digitalWrite(17, LOW); 
      Humi_state=0;
      Button3.print("off");   // 反馈开关状态
    } 
  else if (state=="on") {
      digitalWrite(17, HIGH);   
      Humi_state=1;  
      Button3.print("on");    // 反馈开关状态
    }
}

void dataStorage()   //发送数据到app的串口监视
{
  Blinker.dataStorage("num-kon", Temp_read);
  Blinker.dataStorage("num-ebi", Humi_read);
  Blinker.dataStorage("num-9k4", xinlv_count);
  Blinker.dataStorage("num-qly", Sound_count);
  Blinker.dataStorage("num-845", Guangmin_count);
}

void xTaskOne(void *xTask1)     //多任务一，屏幕显示
{
  while (1)
  {
    Xinlv_Read();
    delay(10);
  }
}

void xTaskTwo(void *xTask2)     //多任务二
{
  while (1)
  {
    uint32_t times = Blinker.time();
    DHT11_Read_dat();         //温湿度采集函数
    Time_Read(); 
    Sound_Read(); 
        
    delay(200);
    i++;
    if(i==8){
      i=0;
      heartbeat();
      Baojing();
    }
  }
}


void setup() {
  Serial.begin(115200);                    //设置波特率
  GPIO_Init();                             //定义相关的引脚
  display.init();                          //初始化OLED显示
 // digitalWrite(2, HIGH);                   //打开开发板自带的调试灯
  BLINKER_DEBUG.stream(Serial);            //定调试信息输出的串口，设备开发时调试使用
  BLINKER_DEBUG.debugAll();                //查看更多内部信息
  Blinker.begin(auth, ssid, pswd);         //开始进行WIFI的连接
  Blinker.attachData(dataRead);            //设置未绑定组件的回调函数
  Blinker.attachHeartbeat(heartbeat);      //一个心脏包的函数，用于数据的回传
  Blinker.attachDataStorage(dataStorage);
  Button1.attach(button1_callback);        //绑定回调函数
  Button2.attach(button2_callback);        //绑定回调函数
  Button3.attach(button3_callback);        //绑定回调函数
  dht.begin();                             //初始化温度传感器
  Blinker.setTimezone(8.0);                //设置时区
  delay(10);

  Serial.println("Initializing...");
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");
   
  particleSensor.setup(); //使用默认设置配置传感器
}

void loop() {
  
  xTaskCreate(xTaskOne, "TaskOne", 5120, NULL, 1, NULL);      //对任务一进行相关的设置
  xTaskCreate(xTaskTwo, "TaskTwo", 5120, NULL, 2, NULL);      //对任务二进行相关的设置

  while(1){      
    Blinker.run();
    OLED_Display();            //OLED屏幕显示  
  }
}

void GPIO_Init()
{
  pinMode(2,OUTPUT);     //开发板上的LED灯 
  pinMode(23,OUTPUT);    //蜂鸣器
  pinMode(26,OUTPUT);    //室内灯光
  pinMode(18,INPUT);     //烟雾传感器
  pinMode(19,INPUT);     //火焰传感器
  pinMode(34,INPUT);     //光敏传感器
  pinMode(35,INPUT);     //声音传感器
  pinMode(16,OUTPUT);    //空调
  pinMode(17,OUTPUT);    //加湿器
} 

void Time_Read(){
  uint32_t times = Blinker.time();
  sec = Blinker.second();
  minu = Blinker.minute();
  hour = Blinker.hour();
  mday = Blinker.mday();
  month = Blinker.month();
  if(sec > -1){
    WIFi_state = 1;
  }
}

void Xinlv_Read(){
  long irValue = particleSensor.getIR();   //通过读取红外值，我们可以知道传感器上是否有手指

  if (checkForBeat(irValue) == true)   //如果检测到心跳
  {
    long delta = millis() - lastBeat;   //测量两拍之间的持续时间
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);    //计算BPM

    if (beatsPerMinute < 255 && beatsPerMinute > 20)  //为了计算平均值，我们先选择一些值，然后做一些数学运算来计算平均值
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //将此读数存储在数组中
      rateSpot %= RATE_SIZE;                    //包装变量

      beatAvg = 0; //取读数的平均值
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000){ //如果没有检测到手指derk
    Serial.print(" No finger?");
    xinlv_count = 0;
  }
  else{
    xinlv_count = beatAvg;
  }
  Serial.println();

}

void Sound_Read(){
  Sound_count = map(analogRead(35),0,4095,0,100);
  Serial.println("Sound_count=");
  Serial.println(Sound_count);
  Guangmin_count = map(analogRead(34),0,4095,0,255);
  Serial.println("Guangmin_count=");
  Serial.println(Guangmin_count);
  if(LED_state==1){
    analogWrite(26,Guangmin_count);
  }
  else{
    analogWrite(26,0);
  }
  yanwu_state = digitalRead(18);
  huoyan_state = digitalRead(19);

}

void Baojing(){
  
  digitalWrite(23,LOW);
  

  if(huoyan_state == 0){
    Text1.color("#FF0000");
    digitalWrite(23,HIGH);
    Text1.print("报警");
    Blinker.vibrate(200);  
  }
  else{
    Text1.print("正常");
  }
  
  if(yanwu_state == 0){
    Text2.color("#FF0000");
    digitalWrite(23,HIGH);
    Text2.print("报警");
    Blinker.vibrate(200);  
  }
  else{
    Text2.print("正常");
  }

  if(xinlv_count > 80){
    Text3.color("#FF0000");
    Text3.print("报警");
    digitalWrite(23,HIGH);
    Blinker.vibrate(200);  
  }
  else{
    Text3.print("正常");
  }
}

void OLED_Display(){
  Time_state_count++;
  display.clear();
  if(Time_state_count < 500){
    if(hour<10){
      display.drawString(0, 0, String(0)+String(hour));
    }
    else{
      display.drawString(0, 0, String(hour));
    }
    display.drawIco16x16(20, 0, Font_1, 0);
    if(minu<10){
      display.drawString(30, 0, String(0)+String(minu));
    }
    else{
      display.drawString(30, 0, String(minu));
    }
    display.drawIco16x16(50, 0, Font_1, 0);
    if(sec<10){
      display.drawString(60, 0, String(0)+String(sec));
    }
    else{
      display.drawString(60, 0, String(sec));
    }
  }
  else if(Time_state_count < 550){
    display.drawString(5, 0, String(month));
    display.drawIco16x16(20, 0, Font_4, 0);
    display.drawString(35, 0, String(mday));
  }
  else{
    Time_state_count = 0;
  }

  display.drawIco16x16(0,16,image_7,0);
  display.drawIco16x16(16, 16, Font_1, 0);
  display.drawString(28, 16, String(xinlv_count));

  display.drawIco16x16(0, 32, image_4, 0);
  display.drawIco16x16(16, 32, Font_1, 0);
  display.drawString(28, 32, String(Temp_read));
  display.drawIco16x16(72, 32, Font_2, 0);

  display.drawIco16x16(0, 48, image_3, 0);
  display.drawIco16x16(16, 48, Font_1, 0);
  display.drawString(28, 48, String(Humi_read));
  display.drawString(72, 48, "%");

  if(WIFi_state == 1 ){
    display.drawIco16x16(112, 0, image_1, 0);
  }
  if(huoyan_state == 0){
    display.drawIco16x16(112, 32, image_5, 0);
  }
  if(yanwu_state == 0){
    display.drawIco16x16(112, 48, image_6, 0);
  }
  display.flipScreenVertically();
  display.display();
}

void DHT11_Read_dat()
{
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {     //检测读取的温度数值是否正确
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));      //串口输出温度值
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    float t = event.temperature;
    Temp_read = t;
  }

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {   //检测读取的湿度数值是否正确
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));        //串口输出湿度值
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
    float h = event.relative_humidity;
    Humi_read = h;
  }
  if(Temp_state==0){
    if(Temp_read > 35){
    digitalWrite(16, HIGH);
    }
    else{
      digitalWrite(16, LOW); 
    }
  }
  if(Humi_state==0){
    if(Humi_read < 15){
    digitalWrite(17, HIGH);
    }
    else{
      digitalWrite(17, LOW); 
    }
  }
}

