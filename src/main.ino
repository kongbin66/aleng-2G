#include "config.h"


int rollback = 0;
void set_dbug(uint8_t a)
{
    dbug=a;
}
/////////////////////////////////////////////////////////////
//设置休眠时间：（S）
/////////////////////////////////////////////////////////////
void SET_SLEEPTIME(time_t t)
{
  sleeptime = t;
  eeprom_config_save_parameter();
  sleeptime = (time_t)EEPROM.readLong(2);
  Serial.printf("sleeptime:%ld\r\n", sleeptime);
}
////////////////////////////////////////////////////////////
//设置亮屏时间和息屏到休眠时间
////////////////////////////////////////////////////////////
void SET_Last_span_Sleep_span(int x, int y)
{
  screen_On_last_span = x;
  screen_Off_to_sleep_span = y;
  eeprom_config_save_parameter();
  screen_On_last_span = (time_t)EEPROM.readInt(43);
  Serial.printf("screen_On_last_span:%ld\r\n", screen_On_last_span);
  screen_Off_to_sleep_span = (time_t)EEPROM.readInt(47);
  Serial.printf("screen_Off_to_sleep_span:%ld\r\n", screen_Off_to_sleep_span);
}
void test1(char a, char b, char c)
{
  f_Flight_Mode = a;
  workingState = b;
  dbug = c;
}
//文件系统使用情况
void spiffs_size()
{
   Serial.printf("FFS SIZE:%d\n",SPIFFS.totalBytes()); 
   Serial.printf("FFS USE SIZE:%d\n",SPIFFS.usedBytes()); 
}
//格式化文件系统
void FFS_fromat()
{  
   bool i= SPIFFS.format();
   if(!i)Serial.printf("FFS_fromat is fail!\n");
   else Serial.printf("FFS_fromat ok!\n");
}
//核对系统时间
void sys_time()
{
   screen_loopEnabled = false;
   setupModem();
   modemToGPRS();
   if(getLBSLocation()) Serial.println("sys_time ok!\n");
   screen_loopEnabled = true;
}





/**************************************************************
//第二核创建按键任务
***************************************************************/
void codeForTask1(void *parameter)
{

  while (1) //这是核1 的loop
  {
    vTaskDelay(10);
    button.tick(); //扫描按键
  }
  vTaskDelete(NULL);
}
/*************************************************************
//ds1302任务
**************************************************************/
void ds1302_task(void *parameter)
{
  uint8_t sec = 0;
  while (1)
  {
    ds_rtc.getDateTime(&now1); //读取时间参数到NOW

    if (now1.second == sec + 1)
    {
      sys_sec++;
      // Serial.printf("sec:%d\n",sys_sec);
    }
    sec = now1.second;
    digitalWrite(LED, HIGH);
    vTaskDelay(250);
    digitalWrite(34, LOW);
    vTaskDelay(250);
  }
  vTaskDelete(NULL);
}
/*****************************************************************
//通讯协议任务
*****************************************************************/
void xieyi_Task(void *parameter)
{
  while (1) //这是核1 的loop
  {
    xieyi_scan();
    vTaskDelay(100);
  }
  vTaskDelete(NULL);
}


/*****************************************************************************************************************/
void setup()
{
  gpio_hold_dis(GPIO_NUM_32); //解锁电源引脚
  gpio_deep_sleep_hold_dis();

  hardware_init(); //硬件初始化
  software_init(); //软件初始化

  xTaskCreatePinnedToCore(xieyi_Task, "xieyi_Task", 3000, NULL, 2, &xieyi_task,tskNO_AFFINITY); //创建DS1302任务
  xTaskCreatePinnedToCore(ds1302_task, "ds1302_task", 2000, NULL, 2, &ds_task,tskNO_AFFINITY);  //创建DS1302任务
  xTaskCreatePinnedToCore(codeForTask1, "task1", 1000, NULL, 2, &task1, tskNO_AFFINITY);
  if (rollback)
  {
    /*************如果rollback置1, 会恢复出厂设置,数据全清***********/
    Serial.println("clean EEPROM");
    EEPROM.write(1, 0);
    EEPROM.commit();
    Serial.println("OK");
    ESP.deepSleep(300000000);
    modem.sleepEnable();
  }
  else
  {
    get_eeprom_firstBootFlag(); //获取EEPROM第1位,判断是否是初次开机
    alFFS_init();               //初始化FFS
    eeprom_config_init();       //初始化EEPROM
    wakeup_init_time();
  }

  if (oledState == OLED_ON)
  {
    showWelcome();
    postMsgId = 0; //清记录条数
  }
  else
  {
    if (workingState == WORKING && (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER)) //不是开机，是定时唤醒。
    {
      send_Msg_var_GSM_while_OLED_on(0);
      go_sleep_a_while_with_ext0(); //休眠
    }
  }
}
/******************************************************************************************************************/
 uint16_t ii;
void loop()
{
 
  ii++;
  if(ii%10==0)digitalWrite(33,LOW); 
  if(ii%5==0)digitalWrite(33,HIGH);


  if (oledState == OLED_ON)
  {
    sht20getTempAndHumi();
    screen_loop(); //展示和滚屏
    key_loop();
    screen_show(); //OLED最终显示
                   // send_Msg_var_GSM_while_OLED_on();
    send_Msg_var_GSM_while_OLED_on(1);
  }
  oled_on_off_switch();

}


/******************************************************************************
*******************************************************************************/
bool sendTempAndHumi2()
{

  //先拼接出json字符串
  char param[178];
  char jsonBuf[256];
  if (current_rec_State == START_RECING)
  {
    sprintf(param, "{\"temp\":{\"value\":%.2f},\"humi\":{\"value\":%.2f},\"le\":{\"value\":%.2f},\"ln\":{\"value\":%.2f},\"start_time\":{\"value\":%u000}}",
            currentTemp, currentHumi, locationE, locationN, now_unixtime); //我们把要上传的数据写在param里
  }
  else
  {
    sprintf(param, "{\"temp\":{\"value\":%.2f},\"humi\":{\"value\":%.2f},\"le\":{\"value\":%.2f},\"ln\":{\"value\":%.2f},\"last_time\":{\"value\":%u000}}",
            currentTemp, currentHumi, locationE, locationN, now_unixtime); //我们把要上传的数据写在param里
  }
  //加上JSON头组成一个完整的JSON格式
  sprintf(jsonBuf, ONENET_POST_BODY_FORMAT, param);
  //再从mqtt客户端中发布post消息
  if (client.publish(ONENET_TOPIC_PROP_POST, jsonBuf))
  {
    Serial.print("Post message to cloud: ");
    Serial.println(jsonBuf);
    current_rec_State = KEEP_RECING;
    return true;
  }
  else
  {
    Serial.println("Publish message to cloud failed!");
    return false;
  }
  // snprintf(msgJson, 256, dataTemplate, currentTemp, currentHumi, locationE, locationN); //将模拟温湿度数据套入dataTemplate模板中, 生成的字符串传给msgJson
  // Serial.print("public the data:");
  // Serial.println(msgJson);
  // char publicTopic[75];
  // char topicTemplate[] = "$sys/%s/%s/thing/property/post"; //主题模板
  // snprintf(publicTopic, 75, topicTemplate, mqtt_pubid, mqtt_devid);
  // Serial.println("publicTopic");
  // Serial.println(publicTopic);
  // if (client.publish(publicTopic, (uint8_t *)msgJson, strlen(msgJson)))
  // {
  //   Serial.print("Post message to cloud: ");
  //   Serial.println(msgJson);
  // }
}

/*****************************************************************************************
 *                            //发送数据
******************************************************************************************/
//#define D1 
void send_Msg_var_GSM_while_OLED_on(bool a)
{
  //Serial.printf("goto send_Msg_var_GSM_while_OLED_on(bool a)\n");
  //key_attach_null();
  //确定进入条件

  if (workingState == WORKING && f_Flight_Mode == false) //工作模式和飞行模式关闭（正常记录）
  {
    if(dbug)Serial.println("zhengchang jilu! ");
    now_rec_stamp = unixtime(); //读现在时间
    #if D1 
    Serial.printf("sleeptime:%d - (now_rec_stamp:%d - last_rec_stamp:%d)\n",sleeptime,now_rec_stamp,last_rec_stamp);
    #endif
    Serial.println("GSM transmission will start at:" + (String)(sleeptime - (now_rec_stamp - last_rec_stamp)));
    if (now_rec_stamp - last_rec_stamp >= sleeptime) //记录间隔到了吗？
    {
      Serial.printf("zhengchang mode time Ok!\n ");
      screen_loopEnabled = false;
      //1.需要联网测网络
      if (a)
      {
        display.clear();
        display.setFont(Roboto_Condensed_12);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(64, 5, "Initializing modem...");
        display.drawProgressBar(5, 50, 118, 8, 5);
        display.display();
      }
      setupModem();
      if (a)
      {
        display.clear();
        display.drawString(64, 5, "Waiting for network...");
        display.drawProgressBar(5, 50, 118, 8, 40);
        display.display();
      }
      modemToGPRS();
      if (a)
      {
        display.clear();
        display.drawString(64, 5, "getting LBS...");
        display.drawProgressBar(5, 50, 118, 8, 70);
        display.display();
      }
      if (getLBSLocation()) //检查网络情况
      {
        Serial.println("zhengchang mode wangluo  OK!");
        sht20getTempAndHumi();
        alFFS_savelist();
        postMsgId++;
        Serial.printf("1*******************************************************1");
        //检查有漏发文件和飞行模式标志吗
        if (f_Flight_Mode == true && f_lose == true) //正在飞行模式中//这种状态可能进不来
        {
          Serial.println("feixing mode zhijietuichu!");
          f_Flight_Mode = true;
          digitalWrite(MODEM_POWER_ON, LOW);
        }

        else if ((f_Flight_Mode == false) && (f_lose == false) && (workingState = WORKING)) //正常记录和发送
        {
          Serial.println("run!");
          if (a)
          {
            display.clear();
            display.drawString(64, 5, "connecting to OneNet");
            display.drawProgressBar(5, 50, 118, 8, 90);
            display.display();
          }
          //2.连接网络
          onenet_connect();
          if (client.connected())
          {
            if (a)
            {
              display.clear();
              display.drawString(64, 5, "uploading...");
            }
            if (sendTempAndHumi2())
            {
              Serial.printf("sendTempAndHumi2 ok!\n");
              if (a)
              {
                display.drawProgressBar(5, 50, 118, 8, 100);
                display.display();
                delay(200);
                display.setTextAlignment(TEXT_ALIGN_LEFT);
              }
              key_init();
              Serial.printf("time jiange:%d\n",(unixtime()-last_rec_stamp));
              last_rec_stamp = unixtime();//发送完成可以记录发送时间
              //screen_loopEnabled = true;
              screen_On_Start = sys_sec;
              screen_On_now = sys_sec;
              digitalWrite(MODEM_POWER_ON, LOW);
              //1.记录正常文件
            
            }
            else
            {
              Serial.printf("sendTempAndHumi2 fial!\n");

              //1.直接写漏发文件和正常记录文件
              alFFS_savelose();
              f_lose=true;
              //置位标志位
              key_init();
              last_rec_stamp = unixtime();//发送失败也要更新发送时间，开启下一次发送
             // screen_loopEnabled = true;
              screen_On_Start = sys_sec;
              screen_On_now = sys_sec;
              digitalWrite(MODEM_POWER_ON, LOW);
            }
          }
          else //数据发送失败
          {
            Serial.printf("onenet_connect fial!\n"); //这里应加入F_LOSE=1?
            //1.直接写漏发文件
            alFFS_savelose();
            f_lose=true;
            //置位标志位
            key_init();
            last_rec_stamp = unixtime();//这里是联网不成功退出，要更新发送时间
           // screen_loopEnabled = true;
            screen_On_Start = sys_sec;
            screen_On_now = sys_sec;
            digitalWrite(MODEM_POWER_ON, LOW);
          }
        }
        else if (f_Flight_Mode == false && f_lose == true) //有漏发文件非飞行模式
        {
          if (dbug)
            Serial.printf("fei feixingmoshi youloufa \n");
          //将本条加入到LOSE
          alFFS_savelose();
          f_lose=true;
          jiexi_lose(a);
          //补发本条
          key_init();
          //last_rec_stamp = unixtime(); 这里应该不需要等发送完漏发在更新
          //screen_loopEnabled = true;
          screen_On_Start = sys_sec;
          screen_On_now = sys_sec;
          digitalWrite(MODEM_POWER_ON, LOW);
        }
        else
        {
          while (1)
          {
            Serial.printf("qitamoshi zhong tuicu! qingjiancha !!!!!!!\n");
            delay(500);
          }
        }
      }
      else
      {
        Serial.printf("zhengchang mode wangluo  eer！\n ");
        //1.直接写漏发文件和正常记录文件
        alFFS_savelose();
        alFFS_savelist();
        postMsgId++;
        Serial.printf("2*******************************************************2");
        //置位标志位
        f_lose = true;
        key_init();
        last_rec_stamp = unixtime();//有更新
       // screen_loopEnabled = true;
        screen_On_Start = sys_sec;
        screen_On_now = sys_sec;
        digitalWrite(MODEM_POWER_ON, LOW);
      }
    }
    else
      if(dbug)Serial.printf("zhengchang mode time no!\n"); //发送时间未到。
  }
  else if (workingState == WORKING && f_Flight_Mode == true) //工作模式和飞行模式开启（不上传网络）
  {
    if (dbug)Serial.println("jilu no send");
    now_rec_stamp = unixtime(); //读现在时间
   #if D1 
   Serial.printf("sleeptime:%d - (now_rec_stamp:%d - last_rec_stamp:%d)\n",sleeptime,now_rec_stamp,last_rec_stamp);
   #endif
    Serial.println("GSM transmission will start at:" + (String)(sleeptime - (now_rec_stamp - last_rec_stamp)));
    if ((now_rec_stamp - last_rec_stamp >= sleeptime)) //记录间隔到了吗？
    {
      //1.直接写漏发文件和正常记录文件
      
      alFFS_savelist();
      Serial.printf("3*******************************************************3");
      postMsgId++;
      alFFS_savelose();

      //置位标志位
      f_lose = true;
      f_Flight_Mode = true;

      key_init();
      last_rec_stamp = unixtime();//间隔到了，不上传也要更新，方便开启下一次
     // screen_loopEnabled = true;
      screen_On_Start = sys_sec;
      screen_On_now = sys_sec;
    }
    else if (dbug)
      Serial.println("feixing mode timer no");
  }
  else if ((workingState == NOT_WORKING) && (f_lose == true) && (f_Flight_Mode == false)) //发现有漏传文件，开启自动上传漏发工作模式
  {
    old_workingstate = 0;

    Serial.println("bufa louchuan");
    //这里要显示补发图片

    old_workingstate = NOT_WORKING;
    workingState = WORKING;
    key_init();
    screen_On_Start = sys_sec;
    screen_On_now = sys_sec;
  }
}



/******************************************************************************
*******************************************************************************/
void jiexi_lose(bool a)
{

  if (lose_count != 0){
    //Serial.printf("lose_count:%d\n", lose_count);
    Serial.println("have lose");
    alFFS_readlose();
    uint32_t i = losestr1.length();
    Serial.printf("memory:%d\n",i);
    //DynamicJsonDocument doc(i * lose_count + 100);

    DynamicJsonDocument doc(losestr1.length()+ 100);
    DeserializationError error = deserializeJson(doc, losestr1);
    if (error)
    {
      Serial.print("error:");
      Serial.println(error.c_str());
      return;
    }

    float aa[lose_count];
    float bb[lose_count];
    uint32_t cc[lose_count];
    for (i = 0; i < lose_count; i++)
    {
      aa[i] = doc["data"][i]["temp"];
      bb[i] = doc["data"][i]["humi"];
      cc[i] = doc["data"][i]["time"];
    }
    for (i = 0; i < lose_count; i++)
    {
      Serial.printf("temp%d=%.2f\n", i, aa[i]);
      Serial.printf("humi%d=%.2f\n", i, bb[i]);
      Serial.printf("time%d=%d\n", i, cc[i]);
    }

    uint32_t locount = lose_count - f_send_ok;
    for (i = f_send_ok; i < locount; i++)
    {
      currentTemp = aa[i];
      currentHumi = bb[i];
      now_unixtime = cc[i];
      onenet_connect();
      if (client.connected())
      {
        char subscribeTopic[75];                           //订阅主题
        char topicTemplate[] = "$sys/%s/%s/cmd/request/#"; //信息模板
        snprintf(subscribeTopic, sizeof(subscribeTopic), topicTemplate, mqtt_pubid, mqtt_devid);
        client.subscribe(subscribeTopic); //订阅命令下发主题
        sendTempAndHumi2();
        f_send_ok++;
      }
      else
      {
        f_lose = 1;
        return;
      }
    }
    if (a)
    {
      display.drawProgressBar(5, 50, 118, 8, 100);
      display.display();
      delay(200);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
    }

    //补发完毕
    //请漏发文件

    lose_first_flag = true;
    lose_count = 0;
    f_send_ok = 0;
    f_lose = false;
    if (!old_workingstate) workingState = NOT_WORKING;
    old_workingstate = 0;
  }
  else
  {
    Serial.println("not lose");
  }
}

// /******************************************************************************
// *******************************************************************************/
// void jiexi_lose(bool a)
// {

//   if (lose_count != 0)
//   {
//     //Serial.printf("lose_count:%d\n", lose_count);
//     Serial.println("have lose");
//     alFFS_readlose();
//     uint32_t i = losestr1.length();
//     Serial.printf("memory:%d\n",i);
//     //DynamicJsonDocument doc(i * lose_count + 100);

//     DynamicJsonDocument doc(losestr1.length()+ 100);
//     DeserializationError error = deserializeJson(doc, losestr1);
//     if (error)
//     {
//       Serial.print("error:");
//       Serial.println(error.c_str());
//       return;
//     }

//     float aa[lose_count];
//     float bb[lose_count];
//     uint32_t cc[lose_count];
//     for (i = 0; i < lose_count; i++)
//     {
//       aa[i] = doc["data"][i]["temp"];
//       bb[i] = doc["data"][i]["humi"];
//       cc[i] = doc["data"][i]["time"];
//     }
//     for (i = 0; i < lose_count; i++)
//     {
//       Serial.printf("temp%d=%.2f\n", i, aa[i]);
//       Serial.printf("humi%d=%.2f\n", i, bb[i]);
//       Serial.printf("time%d=%d\n", i, cc[i]);
//     }

//     uint32_t locount = lose_count - f_send_ok;
//     for (i = f_send_ok; i < locount; i++)
//     {
//       currentTemp = aa[i];
//       currentHumi = bb[i];
//       now_unixtime = cc[i];
//       onenet_connect();
//       if (client.connected())
//       {
//         char subscribeTopic[75];                           //订阅主题
//         char topicTemplate[] = "$sys/%s/%s/cmd/request/#"; //信息模板
//         snprintf(subscribeTopic, sizeof(subscribeTopic), topicTemplate, mqtt_pubid, mqtt_devid);
//         client.subscribe(subscribeTopic); //订阅命令下发主题
//         sendTempAndHumi2();
//         f_send_ok++;
//       }
//       else
//       {
//         f_lose = 1;
//         return;
//       }
//     }
//     if (a)
//     {
//       display.drawProgressBar(5, 50, 118, 8, 100);
//       display.display();
//       delay(200);
//       display.setTextAlignment(TEXT_ALIGN_LEFT);
//     }

//     //补发完毕
//     //请漏发文件

//     lose_first_flag = true;
//     lose_count = 0;
//     f_send_ok = 0;
//     f_lose = false;
//     if (!old_workingstate) workingState = NOT_WORKING;
//     old_workingstate = 0;
//   }
//   else
//   {
//     Serial.println("not lose");
//   }
// }


void read_list()
{
   alFFS_readlist();
  
}


