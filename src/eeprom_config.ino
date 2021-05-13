#include "config.h"
/************************dataType_bytes************************
 * dataType                 bytes
 * ------------------------------------------------------------
 * char                     1
 * short                    2
 * int                      4
 * time_t                   4
 * int32_t                  8
 * long                     4
 * float                    4
 * double                   8
 * ************************************************************/

/************************EEPROM_table***************************
 * adress                   length(byte)        description
 * ------------------------------------------------------------
 * 1                        1                   firstLoad_flag
 * 2                        8                   sleeptime                             休眠时间
 * 10                       1                   temp Upper/Lower limit enabled
 * 11                       4                   temp Upper limit
 * 15                       4                   temp Lower limit
 * 19                       4                   factory date year
 * 23                       4                   factory date month
 * 27                       4                   factory date day
 * 31                       4                   factory time hour
 * 35                       4                   factory time min
 * 39                       8                   time unixtime                          最后一次发送时间
 * 43                       4                   screen_On_last_span                    亮屏时间
 * 47                       4                   screen_Off_to_sleep_span               息屏到休眠时间
 * ************************************************************/
void get_eeprom_firstBootFlag()
{
  Serial.printf("EEPROM 1: %d \r\n", EEPROM.read(1));
  firstBootFlag = EEPROM.read(1) == 1 ? false : true;
}

void eeprom_config_init()
{
  if (firstBootFlag)
  {
    Serial.println("this is the first load,begin to write default:");
    EEPROM.write(1, 1);
    EEPROM.writeInt(2, FACTORY_SLEEPTIME);
    EEPROM.write(10, FACTORY_TEMP_LIMIT_ENABLE);
    EEPROM.writeFloat(11, FACTORY_TEMP_UPPER_LIMIT);
    EEPROM.writeFloat(15, FACTORY_TEMP_LOWER_LIMIT);
    EEPROM.writeInt(19, FACTORY_DATE_YEAR);
    EEPROM.writeInt(23, FACTORY_DATE_MONTH);
    EEPROM.writeInt(27, FACTORY_DATE_DAY);
    EEPROM.writeInt(31, FACTORY_TIME_HOUR);
    EEPROM.writeInt(35, FACTORY_TIME_MIN);
    EEPROM.writeULong(39, 0);
    EEPROM.commit();
    firstBootFlag = false;
    //screen_On_Start = millis();
    //screen_On_now = millis();
    screen_On_Start = sys_sec;
    screen_On_now = sys_sec;
  }
  else
  {
    Serial.println("this is not the first load");

    sleeptime =         (time_t)EEPROM.readLong(2);        Serial.printf("sleeptime:%ld\r\n", sleeptime);
    tempLimit_enable =  EEPROM.read(10) == 0 ? false : true;
    tempUpperLimit =    EEPROM.readFloat(11);              Serial.printf("tempUpperLimit:%.2f\r\n", tempUpperLimit);
    tempLowerLimit =    EEPROM.readFloat(15);              Serial.printf("tempLowerLimit:%.2f\r\n", tempLowerLimit);
    last_rec_stamp =    (time_t)EEPROM.readLong(39);       Serial.printf("last_rec_stamp:%ld\r\n", last_rec_stamp);
    screen_On_last_span=(time_t)EEPROM.readInt(43);        Serial.printf("screen_On_last_span:%ld\r\n", last_rec_stamp);
    screen_Off_to_sleep_span=(time_t)EEPROM.readInt(47);        Serial.printf("screen_Off_to_sleep_span:%ld\r\n", last_rec_stamp);
    //处理时间
    Serial.printf("time now: %d-%d-%d %d:%d:%d\r\n", now1.year, now1.month, now1.day,now1.hour, now1.minute, now1.second);
 
  }
}

void eeprom_config_save_parameter(void)
{
   // EEPROM.write(1, 1);
    EEPROM.writeInt(2, sleeptime);
     EEPROM.write(10, FACTORY_TEMP_LIMIT_ENABLE);
    // EEPROM.writeFloat(11, FACTORY_TEMP_UPPER_LIMIT);
    // EEPROM.writeFloat(15, FACTORY_TEMP_LOWER_LIMIT);
    // EEPROM.writeInt(19, FACTORY_DATE_YEAR);
    // EEPROM.writeInt(23, FACTORY_DATE_MONTH);
    // EEPROM.writeInt(27, FACTORY_DATE_DAY);
    // EEPROM.writeInt(31, FACTORY_TIME_HOUR);
    // EEPROM.writeInt(35, FACTORY_TIME_MIN);
    EEPROM.writeULong(39, last_rec_stamp);
    EEPROM.writeInt(43, screen_On_last_span);  //亮屏时间
    EEPROM.writeInt(47, screen_Off_to_sleep_span);  // 息屏到休眠时间
    EEPROM.commit();
}