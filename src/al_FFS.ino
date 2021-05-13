#include "config.h"

void alFFS_init()
{
  if (firstBootFlag)
    Serial.printf("format FFS:%d", SPIFFS.format());
  else
    Serial.printf("FFS size:%d,used:%d\r\n", SPIFFS.totalBytes(), SPIFFS.usedBytes());
}

// void alFFS_addRec()
// {
//   char tempStr[15];
//   char tempStrtemplate[] = "%d%02d%02d %02d:%02d";
//   snprintf(tempStr, sizeof(tempStr), tempStrtemplate, now1.year, now1.month, now1.day, now1.hour, now1.minute);
//   Serial.print("DATE:");
//   Serial.println(tempStr);
//   Serial.print("now the alFFS_thisRec_firstData_flag value is :");
//   Serial.println(alFFS_thisRec_firstData_flag);
//   if (alFFS_thisRec_firstData_flag)
//   {
//     alFFS_thisRec_firstData_flag = false;
//     Serial.println("first rec, so create a file named:");
//     char tempPathtemplate[] = "/R%d%02d%02d_%02d%02d.json";
//     snprintf(nowREC_filepath, 21, tempPathtemplate, now1.year, now1.month, now1.day, now1.hour, now1.minute);
//     Serial.println(nowREC_filepath);
//     Serial.println("now first write content to it");
//     File f = SPIFFS.open(nowREC_filepath, FILE_WRITE);
//     String strtemp = "{\"st\":\"" + (String)tempStr +
//                      "\",\"data\": [{\"tm\":\"" + (String)tempStr +
//                      "\",\"tmsp\":" + (String)(unixtime()) + //- 8 * 60 * 60
//                      ",\"tp\":" + (String)currentTemp +
//                      ",\"h\":" + (String)currentHumi +
//                      ",\"E\":" + (String)locationE +
//                      ",\"N\":" + (String)locationN +
//                      "}";
//     f.println(strtemp);
//     Serial.println("ADD:" + strtemp);
//     f.close();
//     alFFS_thisRec_firstData_flag = 0;
//   }
//   else
//   {
//     Serial.println("not the first rec, so i can just append some content in to the file:");
//     Serial.println(nowREC_filepath);
//     File f = SPIFFS.open(nowREC_filepath, FILE_APPEND);
//     String strtemp = ",{\"tm\":\"" + (String)tempStr +
//                      "\",\"tmsp\":" + (String)(unixtime() - 8 * 60 * 60) +
//                      ",\"tp\":" + (String)currentTemp +
//                      ",\"h\":" + (String)currentHumi +
//                      ",\"E\":" + (String)locationE +
//                      ",\"N\":" + (String)locationN +
//                      "}";
//     f.println(strtemp);
//     Serial.println("ADD:" + strtemp);
//     f.close();
//   }
// }

// void alFFS_readRecing()
// {
//   File f = SPIFFS.open(nowREC_filepath, FILE_READ);
//   // String strtemp;
//   // strtemp = f.readString();
//   Serial.println("read out the file:");
//   Serial.println(f.readString());
//   Serial.println("Size of json_file :" + (String)(f.size()) + "B");
//   Serial.println("Size of json_file :" + (String)(f.size() / 1024.0) + "KB");
//   Serial.println("Size of json_file :" + (String)((f.size() / 1024.0) / 1024.0) + "MB");
//   Serial.printf("use:%dB\n", SPIFFS.totalBytes());
// }

// void alFFS_endRec()
// {
//   char tempStr[15];
//   char tempStrtemplate[] = "%d%02d%02d %02d:%02d";
//   snprintf(tempStr, 15, tempStrtemplate, now1.year, now1.month, now1.day, now1.hour, now1.minute);
//   Serial.print("DATE:");
//   Serial.println(tempStr);
//   File f = SPIFFS.open(nowREC_filepath, FILE_APPEND);
//   String strtemp = "],\"et\":\"" + (String)tempStr + "\"}";
//   f.println(strtemp);
//   Serial.println("ADD:" + strtemp);
//   f.close();
//   alFFS_readRecing();
// }

/*****************************************************************************************
 *                                    json读list文件
******************************************************************************************/
void alFFS_readlist()
{
  File f = SPIFFS.open("/list.json", FILE_READ);
  Serial.println("file size:" + (String)f.size());

  Serial.print(f.readString());
  Serial.println("]}");
  f.close();
  // Serial.println("Size of json_file :" + (String)(f.size()) + "B");
  // Serial.println("Size of json_file :" + (String)(f.size() / 1024.0) + "KB");
  // Serial.println("Size of json_file :" + (String)((f.size() / 1024.0) / 1024.0) + "MB");
}
/*****************************************************************************************
 *                                    json读lose文件
******************************************************************************************/
// void alFFS_readlose()
// {
//   File f = SPIFFS.open("/lose.json", FILE_READ);
//   Serial.println("lose file size:" + (String)f.size());
//   losestr1 = f.readString() + "]}";
//   Serial.println(losestr1);
//   f.close();
// }

/*****************************************************************************************
 *                            //记忆到LIST文件，判断第一次写标志
******************************************************************************************/
void alFFS_savelist()
{
  //1.读取温湿度，和当前时间。
  char tempStr[18];
  char tempStrtemplate[] = "%d%02d%02d %02d:%02d:%02d";
  snprintf(tempStr, sizeof(tempStr), tempStrtemplate, (now1.year + 2000), now1.month, now1.day, now1.hour, now1.minute, now1.second);
  sht20getTempAndHumi();
  //2.写入到list文件
  if (list_first_flag) //第一次发送写
  {
    Serial.println("list first rec!");
    list_first_flag = 0;
    alFFS_thisRec_firstData_flag = 0;
    File f = SPIFFS.open("/list.json", FILE_WRITE);
    String strtemp = "{\"st\":\"" + (String)tempStr +
                     "\",\"data\": [{\"tm\":\"" + (String)tempStr +
                     "\",\"tmsp\":" + (String)(unixtime()) +
                     ",\"tp\":" + (String)currentTemp +
                     ",\"h\":" + (String)currentHumi +
                     ",\"E\":" + (String)locationE +
                     ",\"N\":" + (String)locationN +
                     "}";
    f.println(strtemp);
    // Serial.println("ADD:" + strtemp);
    f.close();
    alFFS_readlist();
  }
  else //非第一次发送添加
  {
    list_first_flag = 0;
    alFFS_thisRec_firstData_flag = 0;
    Serial.println("list not the first rec!");
    File f = SPIFFS.open("/list.json", FILE_APPEND);
    String strtemp = ",{\"tm\":\"" + (String)tempStr +
                     "\",\"tmsp\":" + (String)(unixtime()) +
                     ",\"tp\":" + (String)currentTemp +
                     ",\"h\":" + (String)currentHumi +
                     ",\"E\":" + (String)locationE +
                     ",\"N\":" + (String)locationN +
                     "}";
    f.println(strtemp);
    f.close();
    alFFS_readlist();
  }
}

//保存到LOSE文件
// {
// Data:
//   [{
//    “temp”: 26.32,
//    “humi”: 54.55,
//    “time”: 9995959544},
//    {
//    “temp”: 26.32,
//    “humi”: 54.55,
//    “time”: 9995959544}]
// }
// /*****************************************************************************************
//  *
//  *                            //保存到LOSE文件
// ******************************************************************************************/

// void alFFS_savelose()
// {
//   sht20getTempAndHumi(); //读取温湿度，

//   //保存数据
//   if (lose_first_flag)
//   {
//     Serial.println("lose is first rec");
//     lose_first_flag = 0;
//     //发送头
//     String bb = "{\"data\":[{\"temp\":" + (String)currentTemp +
//                 ",\"humi\":" + (String)currentHumi +
//                 ",\"time\":" + (String)(unixtime()) +
//                 "}";
//     File f = SPIFFS.open("/lose.json", FILE_WRITE);
//     f.println(bb);
//     f.close();
//     alFFS_readlose();
//     lose_count++;
//     if (dbug)
//       Serial.printf("lose_cout:%d\n", lose_count);
//   }
//   else
//   {
//     Serial.println("lose not the first rec");
//     lose_first_flag = 0;

//     String bb = ",{\"temp\":" + (String)currentTemp +
//                 ",\"humi\":" + (String)currentHumi +
//                 ",\"time\":" + (String)(unixtime()) +
//                 "}";
//     File f = SPIFFS.open("/lose.json", FILE_APPEND);

//     f.println(bb);
//     f.close();
//     alFFS_readlose();
//     lose_count++;
//     if (dbug)
//       Serial.printf("lose_cout:%d\n", lose_count);
//   }
// }

/*****************************************************************************************
 *                                    读lose文件(HEX)
******************************************************************************************/
void alFFS_readlose()
{
  int read_count = 0;
  int tm = 0;

  //打开文件
  File f = SPIFFS.open("/test.txt", "r");
  //输出长度
  read_count = f.size();
  Serial.printf("lose file size:%d\n", read_count);
  tm = read_count / 8;
  Serial.printf("lose file num:%d\n", tm);
  if (tm == lose_count)
  {
    char aa[read_count + 1];
    f.readBytes(aa, read_count + 1); //读取数据
    //Serial.println(aa);

    char bb[8];
    int temp;
    int hum;
    uint32_t tim;

    int i = tm;
    for (int i = 0; i < tm; i++)
    {
      Serial.printf("di%dtiao data\n", i);
      bb[0] = aa[i * 8 + 0];
      bb[1] = aa[i * 8 + 1];
      bb[2] = aa[i * 8 + 2];
      bb[3] = aa[i * 8 + 3];
      bb[4] = aa[i * 8 + 4];
      bb[5] = aa[i * 8 + 5];
      bb[6] = aa[i * 8 + 6];
      bb[7] = aa[i * 8 + 7];

      //将所有内容输出

      temp = (bb[0] << 8) + bb[1];
      Serial.printf("temp:%f\n", (float)temp / 100);

      hum = (bb[2] << 8) + bb[3];
      Serial.printf("hum:%f\n", (float)hum / 100);

      tim = (bb[4] << 24) + (bb[5] << 16) + (bb[6] << 8) + bb[7];
      Serial.printf("time:%d\n", tim);
      f.close();
    }
  }
  else
  {
    f.close();
    Serial.println("alFFS_readlose error!");
    return;
  }
}

/*****************************************************************************************
 *
 *                            //保存到LOSE文件(HEX)
******************************************************************************************/

void alFFS_savelose()
{
  //存储以字节方式存储和读取
  File f;
  uint8_t a[4];          //写入缓存
  sht20getTempAndHumi(); //读取温湿度，

  int temp = (int)(currentTemp * 100); //温度值
  int hum = (int)(currentHumi * 100);  //湿度
  uint32_t uinxt = unixtime();

  if (lose_first_flag)
  {
    Serial.println("lose is first rec");
    f = SPIFFS.open("/test.txt", "w");
  }
  else
  {
    // Serial.println("lose not the first rec");
    f = SPIFFS.open("/test.txt", "a");
  }

  lose_first_flag = 0;
  a[0] = temp >> 8;
  a[1] = temp;
  f.write(a, 2);
  f.close();
  //添加湿度
  f = SPIFFS.open("/test.txt", "a");
  a[0] = hum >> 8;
  a[1] = hum;
  f.write(a, 2);
  //添加时间
  a[0] = uinxt >> 24;
  a[1] = uinxt >> 16;
  a[2] = uinxt >> 8;
  a[3] = uinxt;

  f.write(a, 4);
  //写入条数加1
  lose_count++;
  f.close();
  //完成
  //alFFS_readlose();
}

void test_r()
{

}


void test(bool x)
{
}