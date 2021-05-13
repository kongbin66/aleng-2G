#include "config.h"

/*-------------------------------连接平台-------------------------------*/
void onenet_connect()
{
  //连接OneNet并上传数据
  Serial.println("connecting to OneNet IOT...");
  client.setServer(mqtt_server, port);                   //设置客户端连接的服务器,连接Onenet服务器, 使用6002端口
  client.connect(mqtt_devid, mqtt_pubid, mqtt_password); //客户端连接到指定的产品的指定设备.同时输入鉴权信息
}
/*-------------------------------向云平台发送温湿度数据-------------------------------*/
void sendTempAndHumi()
{
  if (client.connected())
  {
    //先拼接出json字符串
    char param[178];
    char jsonBuf[256];
    if (current_rec_State == START_RECING)
    {
      sprintf(param, "{\"temp\":{\"value\":%.2f},\"humi\":{\"value\":%.2f},\"le\":{\"value\":%.2f},\"ln\":{\"value\":%.2f},\"start_time\":{\"value\":%u000}}", currentTemp, currentHumi, locationE, locationN,now_unixtime); //我们把要上传的数据写在param里
    }else{
      sprintf(param, "{\"temp\":{\"value\":%.2f},\"humi\":{\"value\":%.2f},\"le\":{\"value\":%.2f},\"ln\":{\"value\":%.2f},\"last_time\":{\"value\":%u000}}", currentTemp, currentHumi, locationE, locationN,now_unixtime); //我们把要上传的数据写在param里
    }
    sprintf(jsonBuf, ONENET_POST_BODY_FORMAT,param);
    //再从mqtt客户端中发布post消息
    if (client.publish(ONENET_TOPIC_PROP_POST, jsonBuf))
    {
      Serial.print("Post message to cloud: ");
      Serial.println(jsonBuf);
      current_rec_State = KEEP_RECING;
    }
    else
    {
      Serial.println("Publish message to cloud failed!");
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

    // Serial.println("Publish message to cloud failed!");
  }
}
