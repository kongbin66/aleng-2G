#include "config.h"
//设置ip5306持续升压
int8_t BatteryLevel;



static bool setPowerBoostKeepOn(bool en)
{
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_SYS_CTL0);
  if (en)
  {
    Wire.write(0b00110111);//0x37
  }
  else
  {
    Wire.write(0b00110101); // x35 => Autoshutdown - Will kill power of ESP32 while sleeping! 0b110101
  }
  return (Wire.endTransmission() == 0);
}
//实验开关电压保持
void test_power(bool x)
{
   bool i= setPowerBoostKeepOn(x);
   Serial.println(String("IP5306 setPowerBoostKeepOn: ") + (i ? "OK" : "FAIL"));


}
/* 设定电源芯片保持升压的同时，读取电量的电量等级级电池电压 */
void PowerManagment() //保持升压芯片持续工作
{
  Serial.println(F("------------POWER-------------"));
  delay(100);
  bool isOk = setPowerBoostKeepOn(true); // Disables auto-standby, to keep 5V high during ESP32 sleep after 32Sec
  Serial.println(String("IP5306 setPowerBoostKeepOn: ") + (isOk ? "OK" : "FAIL"));

  //显示电量
  Serial.println(String("IP5306 Battery level:") + getBatteryLevel(getBatteryFromADC()) + String("%"));
}

/* //读取电池端实时电压
滤波100次，输出测量电压值 */
float getBatteryFromADC()
{
  float bat_mv=0;//电池电压
  uint32_t oversample = 0;
  for (size_t i = 0; i < 100; i++)
  {
    oversample += (uint32_t)analogRead(BATTERY_ADC_PIN );
  }
  bat_mv = (int)oversample / 100;
  bat_mv = ((float)bat_mv / 4096) * 3.35 * 2 ;

  Serial.println(String("Battery from ADC: ") + bat_mv + String("V"));
  Serial.println(F("------------POWER-------------"));
  return bat_mv;
}
//检测电池电量等级
uint8_t getBatteryLevel(float x)
{
  //将电压分段（4段）
  if(x>grade80&&x<grade100) return 3;
  else if(x>grade50&&x<(grade80-0.1)) return 2;
  else if(x>grade30&&x<(grade50-0.1)) return 1;
  else if(x>Power_min_voltage&&x<(grade30-0.1)) return 0;
}

//更新屏幕电量图片
/**********************************************************
      输出结果：
      -1：未读取到电量(异常)
      -2：其他不正确结果(异常)
      0:   1/4
      25： 2/4
      75： 3/4
      100：4/4
***********************************************************/
int8_t fun_Refresh_lcon(int x) //刷新更改图标
{
  int8_t i = -2;
 
  switch (x)
  {
  case 0 :
    p1 = F16x16_b0, i = 0;
    break;
  case 1:
    p1 = F16x16_b20, i = 25;
    break;
  case 2:
    p1 = F16x16_b60, i = 75;
    break;
  case 3:
    p1 = F16x16_b100, i = 100;
    break;
  default:
    i = -1; //其他不正确结果(异常)
    break;
  }
  
  return i;
}


//电量检测与电量低报警检测
/* 1.采集芯片电量，刷新屏显示，电量低于系统设定温度时，更新报警标志POWER_warning_flag */

void power_alarm_test()
{
  uint8_t i=0;
  float voltage = 0;
  voltage = getBatteryFromADC();
  i = getBatteryLevel(voltage);
  //刷新屏幕显示电量
  fun_Refresh_lcon(i);
   
  if (i <= 25)
  {
    
    if (voltage < Power_min_voltage) //电压低了
    {
      POWER_warning_flag = 1; //上传报警后进入关机状态。
      SerialMon.println("power_voltage_low--xxx");
    }
    else POWER_warning_flag = 0;
  }
}

































//这种方式已经测试过可以使用，以后备用。
//数据手册上给的I2C地址0xea是错误的，地址是0x75
//int lastbattery = -1;
// void displayBatteryLevel2()
// {
   
//         int battlevel = 0;
//         byte retval;
//         Wire.beginTransmission(0x75);
//         Wire.write(0x78);
//         if (Wire.endTransmission(false) == 0 && Wire.requestFrom(0x75, 1)) 
//         {
//             retval = Wire.read() & 0xF0;
//             if (retval == 0xE0) battlevel = 25;
//             else if (retval == 0xC0) battlevel = 50;
//             else if (retval == 0x80) battlevel = 75;
//             else if (retval == 0x00) battlevel = 100;
//         }
//         Serial.printf("retval:%d\n",retval);
//         Serial.printf("battlevel:%d\n",battlevel);
//         if (lastbattery != battlevel)
//         {
         
//             if (battlevel <= 25)
//                 Serial.printf("小于25\n");
//             else
//                 Serial.printf("大于\n");
//             lastbattery = battlevel;
//         }
// }




// static bool setPowerBoostKeepOn(bool en)
// {
//   Wire.beginTransmission(IP5306_ADDR);
//   Wire.write(IP5306_REG_SYS_CTL0);
//   if (en)
//   {
//     Wire.write(0b110111);
//     // 0x37 = 0b110111 TCALL example

//     /*
//  [1]      Boost EN (default 1)            [EXM note: if 0 ESP32 will not boot from battery]
//  [1]      Charger EN (1)                  [EXM note: did  not observe difference]
//  [1]      Reserved (1)                    [EXM note: did  not observe difference]
//  [1]      Insert load auto power EN (1)   [EXM note: did  not observe difference]
//  [1]      BOOST output normally open ( 1) [EXM note: if 0 will shutdown on ESP32 sleep after 32s]
//  [1]      Key off EN: (0)                 [EXM note: could not detect difference]
//   */
//   }
//   else
//   {
//     // Wire.write(0x35); // x35 => Autoshutdown - Will kill power of ESP32 while sleeping!
//     //HEX 35 = 0b110101
//   }
//   return Wire.endTransmission() == 0;
// }


// //检测电池电量等级
// int8_t getBatteryLevel()
// {
//   Wire.beginTransmission(IP5306_ADDR);
//   Wire.write(0x78);
//   if (Wire.endTransmission(false) == 0 && Wire.requestFrom(0x75, 1))
//   {
//     switch (Wire.read() & 0xF0)
//     {
     
//     case 0xE0 ... 0xff:    
//       return 25;
//     case 0xC0 ... 0xdf:
//       return 50;
//     case 0x80 ... 0xbf:
//       return 75;
//     case 0x00 ... 0x7f:
//       return 100;
//     default:
//       return 0;
//     }
//   }
//   return -1;
// }





/* //电池电量ADC采集
float battery_ADC()
{
  uint32_t i = 0;
   float j = 0.0;
  uint8_t k = 10; //滤波次数

  for (; k > 0; k--)
    i += analogRead(BATTERY_ADC_PIN);
  i = i / k;
  j = (i * 3.3 / 4096 + 0.23) * 2;
  SerialMon.print(j);
  SerialMon.printf("V\r\n");
  return j;
} */

/*  float power_getBatteryLevel() 
{
  uint8_t i = 0XFF, j = 0;
  float k=0.0;

  do
  {
    i = fun_getBatteryLevel(); //获取电源管理芯片电池电量
    j++;
  } while (i == -1 || j > POWER_READ_NUM ); //读取失败连续读直到读取次数溢出。


  // i=0;j=51;
  if (j > POWER_READ_NUM ) //读取不到电量（异常）
  {
    j = 0;
    SerialMon.println("ERR:Failed to read power!!");
  }
  else if (i == 0) //确定电源芯片反馈电压低
  {
    k = battery_ADC(); //AD检测和确定
  }
  return k;
} */

/* void Power_test(float a) //确定电量最小值
{
  SerialMon.println("++++++++++++++++++++++++++++++++++++++++++++++++++++");
  SerialMon.println(a);
  if (a < Power_min_voltage) //电压低了
  {
    POWER_warning_flag = 1;
  }
  else
  {
    power_getBatteryLevel(); //重新读取电量显示
    POWER_warning_flag = 0;
  }
  SerialMon.printf("POWER_warning_flag=%d",POWER_warning_flag);
} */