/*----------------------------------------------------------
                 軟、硬件初始化操作相关
*---------------------------------------------------------*/
#include "config.h"
//硬件初始化
void hardware_init()
{
  //IO设定
  pinMode     (13, OUTPUT); //KEY引脚
  digitalWrite(13,LOW); 
  pinMode     (MODEM_POWER_ON, OUTPUT); //电源引脚
  pinMode     (33, OUTPUT); //LED引脚
  digitalWrite(33,HIGH);
  //串口设定
  SerialMon.begin(115200); //初始化调试串口
  SerialMon.printf("/**************************************************************/\n");
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX); //初始化AT串口
  //I2C设置
  Wire.begin();
  
  sht20.begin();//溫濕度傳感器初始化
  EEPROM.begin(4096);//E2PROM初始化
  SPIFFS.begin();
  display.init();
  display.flipScreenVertically();
  key_init();
  //ds_rtc.Ds1302(uint8_t pin_ena, uint8_t pin_clk, uint8_t pin_dat);
  
  bool i;
  //初始化DS1302引脚
  ds_rtc.init();
  i=ds_rtc.isHalted();//检查运行DS1302
  if(i) ds_rtc.setDateTime(&now1);//数值不对，只是让它运行

 
  //唤醒方式
  Serial.print("esp_sleep_get_wakeup_cause");
  Serial.println(esp_sleep_get_wakeup_cause());
  
  
  adcAttachPin(BATTERY_ADC_PIN); //将引脚连接到ADC
   //保持升压芯片持续工作
  PowerManagment();
 
  //电量检测及欠压报警检测
  power_alarm_test();
    if (POWER_warning_flag)
  {
    Serial.println("dianliaodi!");
  }

  //adcStart(BATTERY_ADC_PIN);     //在连接的引脚总线上开始ADC转换
  
}
/*----------------------------------------------------------
                 软件初始化操作相关
*---------------------------------------------------------*/
void software_init()//軟件初始化
{
  tempAndHumi_Ready = false;
  Serial.printf("workingState:%d\r\n", workingState);
  Serial.printf("oledState:%d\r\n", oledState);
  loopStartTime = millis();
  screen_loopEnabled = true;
  //下面是固定参数，需要修改时再保存到EEPROM中
  show_tip_screen_last = 3;
  show_BLE_screen_last = 3;
  show_rec_stop_screen_last = 3;

  
  screen_On_Start = sys_sec;
  screen_On_now = sys_sec;
  
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) //如果是系统复位唤醒的, 则停止工作, 亮屏
  {
    workingState = NOT_WORKING;
    Serial.println("workingState = NOT_WORKING;2");

    oledState = OLED_ON;
    list_first_flag=true;
    lose_first_flag=true;
    postMsgId = 0; //清记录条数
    lose_count=0;
    deleteFile(SPIFFS, "/list.json");
    deleteFile(SPIFFS, "/lose.hex");
    ds_rtc.getDateTime(&now1); 
    Serial.printf("time now1: %d-%d-%d %d:%d:%d\r\n", now1.year, now1.month, now1.day,now1.hour, now1.minute, now1.second);
    last_rec_stamp = now_unixtime;//刷新最后发送时间
    eeprom_config_save_parameter();
  } 
  keyState = NOKEYDOWN;
  screenState = MAIN_TEMP_SCREEN;
  bleState = BLE_OFF;
  lockState = UNLOCKED;
  //qualifiedState = QUALITIFY_RIGHT;


  
}