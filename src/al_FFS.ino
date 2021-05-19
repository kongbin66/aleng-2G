#include "config.h"
//初始化
void alFFS_init()
{
  if (firstBootFlag)
    Serial.printf("format FFS:%d", SPIFFS.format());
  else
    Serial.printf("FFS size:%d,used:%d\r\n", SPIFFS.totalBytes(), SPIFFS.usedBytes());
}

//读文件列表
void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\r\n", dirname);
  //1.打开“/”开头文件
  File root = fs.open(dirname);
  //2.检查是否打开
  if (!root)
  {
    Serial.println("- failed to open directory");
    return;
  }
  //3.检查是否有“/”开头文件
  if (!root.isDirectory())
  {
    Serial.println(" - not a directory");
    return;
  }
  //4.有“/”开头的文件，再创建一个新对象打开下一个文件
  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}
//XIEYI 读文件列表
void my_listDir(bool x)
{
  listDir(SPIFFS, "/", x);
}
//写文件 用法： writeFile(SPIFFS, "/hello.txt", "Hello ");
void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\r\n", path);
  //1.输入文件名打开文件，写
  File file = fs.open(path, FILE_WRITE);
  //2.检查打开成功了吗？
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  //3.打开成功写信息
  if (file.print(message))
  {
    Serial.println("- file written");
  }
  //4.写失败返回信息
  else
  {
    Serial.println("- write failed");
  }
  //5.关闭文件
  file.close();
}
//文件系统添加文件 用法：appendFile(SPIFFS, "/hello.txt", "World!\r\n");
void appendFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Appending to file: %s\r\n", path);
  //1.输入文件名打开文件，添加文件
  File file = fs.open(path, FILE_APPEND);
  //2.检查打开成功了吗？
  if (!file)
  {
    Serial.println("- failed to open file for appending");
    return;
  }
  //3.打开成功添加信息
  if (file.print(message))
  {
    Serial.println("- message appended");
  }
  //4.添加失败返回信息
  else
  {
    Serial.println("- append failed");
  }
  //5.关闭文件
  file.close();
}
/*************************************list文件 json*************************************/
//json读list文件
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

//josn保存到LIST文件，判断第一次写标志
void alFFS_savelist()
{
  Serial.printf("start save list ... list_first_flag=%d\n",list_first_flag);
  //1.读取温湿度，和当前时间。
  char tempStr[18];
  char tempStrtemplate[] = "%d%02d%02d %02d:%02d:%02d";
  snprintf(tempStr, sizeof(tempStr), tempStrtemplate, (now1.year + 2000), now1.month, now1.day, now1.hour, now1.minute, now1.second);
  if((f_locat==1)&&(locationE!=locationE_))
  {
    locationN=locationN_;
    locationE=locationE_;
    locationA=locationA_; 
    Serial.printf("Temp=%f,Humi=%f,Time=%d,locationE=%f,locationN=%f.\n", currentTemp, currentHumi, now_unixtime, locationE, locationN);
  }
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
                     "\",\"tmsp\":" + (String)(now_unixtime) +
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
                     "\",\"tmsp\":" + (String)(now_unixtime) +
                     ",\"tp\":" + (String)currentTemp +
                     ",\"h\":" + (String)currentHumi +
                     ",\"E\":" + (String)locationE +
                     ",\"N\":" + (String)locationN +
                     "}";
    f.println(strtemp);
    f.close();
    alFFS_readlist();
  }
  postMsgId++;
  Serial.printf("postMsgId:%d\n",postMsgId);
}


/**************************************lose文件 （hex）************************************/
//读lose文件(HEX)
void alFFS_readlose()
{
  Serial.println("Reading file: lose.hex");
  File file = SPIFFS.open("/lose.hex", "r");
  if (!file || file.isDirectory())
  {
    Serial.println("- failed to open file for reading");
    return;
  }
  Serial.printf("lose size:%d\n",file.size()); 
  while(file.available())
  {
    Serial.print(file.read());
  }
  Serial.println();
  Serial.printf("/*****************************READ LOES END***********************************/\n");
        




  // int read_count = 0;
  // int tm = 0;

  // //打开文件
  // File f = SPIFFS.open("/lose.hex", "r");
  // //输出长度
  // read_count = f.size();
  // Serial.printf("lose file size:%d\n", read_count);
  // tm = read_count / 8;
  // Serial.printf("lose file num:%d\n", tm);
  // if (tm == lose_count)
  // {
  //   char aa[read_count + 1];
  //   f.readBytes(aa, read_count + 1); //读取数据
  //   //Serial.println(aa);

  //   char bb[8];
  //   int temp;
  //   int hum;
  //   uint32_t tim;

  //   int i = tm;
  //   for (int i = 0; i < tm; i++)
  //   {
  //     Serial.printf("di%dtiao data\n", i);
  //     bb[0] = aa[i * 8 + 0];
  //     bb[1] = aa[i * 8 + 1];
  //     bb[2] = aa[i * 8 + 2];
  //     bb[3] = aa[i * 8 + 3];
  //     bb[4] = aa[i * 8 + 4];
  //     bb[5] = aa[i * 8 + 5];
  //     bb[6] = aa[i * 8 + 6];
  //     bb[7] = aa[i * 8 + 7];

  //     //将所有内容输出

  //     temp = (bb[0] << 8) + bb[1];
  //     Serial.printf("temp:%f\n", (float)temp / 100);

  //     hum = (bb[2] << 8) + bb[3];
  //     Serial.printf("hum:%f\n", (float)hum / 100);

  //     tim = (bb[4] << 24) + (bb[5] << 16) + (bb[6] << 8) + bb[7];
  //     Serial.printf("time:%d\n", tim);
  //     f.close();
  //   }
  // }
  // else
  // {
  //   f.close();
  //   Serial.println("alFFS_readlose error!");
  //   return;
  // }
}

//KB :保存到LOSE文件(HEX)
void alFFS_savelose()
{
  //存储以字节方式存储和读取
  File f;
  uint8_t a[4];          //写入缓存
  //确定数据
  sht20getTempAndHumi(); //读取温湿度，
  if((f_locat==1)&&(locationE!=locationE_))
  {
    locationN=locationN_;
    locationE=locationE_;
    locationA=locationA_; 
    Serial.printf("Temp=%f,Humi=%f,Time=%d,locationE=%f,locationN=%f.\n", currentTemp, currentHumi, now_unixtime, locationE, locationN);
  }
  Serial.printf("start save lose ...lose_first_flag = %d\n",lose_first_flag);
  int temp = (int)(currentTemp * 100); //温度值
  int hum = (int)(currentHumi * 100);  //湿度
  uint32_t uinxt = now_unixtime;

  if (lose_first_flag)
  {
    Serial.println("lose is first rec");
    f = SPIFFS.open("/lose.hex", "w");
    if (!f|| f.isDirectory())
    {
    Serial.println("- failed to open file for reading");
    return;
    }
    lose_first_flag = 0;
  }
  else
  {
    // Serial.println("lose not the first rec");
    f = SPIFFS.open("/lose.hex", "a");
    if (!f|| f.isDirectory())
    {
    Serial.println("- failed to open file for reading");
    return;
    }
  }

  
  a[0] = temp >> 8;
  a[1] = temp;
  f.write(a, 2);
  f.close();
  //添加湿度
  f = SPIFFS.open("/lose.hex", "a");
  if (!f|| f.isDirectory())
    {
    Serial.println("- failed to open file for reading");
    return;
    }

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
  Serial.printf("lose_count:%d\n",lose_count);
  f.close();
  //完成
  alFFS_readlose();
}

//写文件 用法： writeFile(SPIFFS, "/hello.txt", "Hello ");
void writeFile_test(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\r\n", path);

  //1.输入文件名打开文件，写
  File file = fs.open(path, FILE_WRITE);
  //2.检查打开成功了吗？
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  //3.打开成功写信息

  if (file.write((const uint8_t *)message, 8))
  {
    Serial.println("- file written");
  }
  //4.写失败返回信息
  else
  {
    Serial.println("- write failed");
  }
  //5.关闭文件
  file.close();
}

//文件系统添加文件 用法：appendFile(SPIFFS, "/hello.txt", "World!\r\n");
void appendFile_test(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Appending to file: %s\r\n", path);
  //1.输入文件名打开文件，添加文件
  File file = fs.open(path, FILE_APPEND);
  //2.检查打开成功了吗？
  if (!file)
  {
    Serial.println("- failed to open file for appending");
    return;
  }
  //3.打开成功添加信息
  if (file.write((const uint8_t *)message, 8))
  {
    Serial.println("- message appended");
  }
  //4.添加失败返回信息
  else
  {
    Serial.println("- append failed");
  }
  //5.关闭文件
  file.close();
}

////通过条数得到数据  用法：get_data(SPIFFS, "/lose.hex", x,&a,&b, &c);
//输入一个条数 可得到漏发文件中的数值，当漏发条数相等时，漏发文件可以删除了
void get_data(fs::FS &fs, const char *path, uint32_t x,float* a,float *b, uint32_t *c)
{
  //1.创建变量
  uint32_t i, j, k;
  uint32_t addh;//首地址

  uint32_t work_page = 0; //工作页
  uint32_t work_num = 0;  //工作条数
  uint32_t work_size = 0; //工作字节长度
  uint32_t lose_page = 0; //漏发文件页数
  uint32_t lose_num = 0;  //漏发文件条数
  uint32_t lose_size = 0; //漏发文件字节长度
  bool work_flag = 0;     //失败重写标志
  //2.清解码缓存器
  for (int i = 0; i < 512; i++)
    work_data[i] = 0;

  //3.输入文件名打开文件，读文件
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if (!file || file.isDirectory())
  {
    Serial.println("- failed to open file for reading");
    return;
  }
  //4.计算参数
  lose_size = file.size(); //文件长度
  Serial.printf("lose_size:%d\n", lose_size);
  lose_page = (lose_size / 512);//漏发文件页长度
  Serial.printf("lose_page:%d\n", lose_page);
  lose_num = (lose_size / 8);//漏发信息长度
  Serial.printf("lose_num:%d\n", lose_num);
  work_size = (x * 8); //需要解码的指令地址
  Serial.printf("work_size:%d\n", work_size);
  work_page = (work_size) / 512; // 需要解码的指令页地址
  Serial.printf("work_page:%d\n", work_page);
  //5.开始解码
  if (lose_size > work_size) //指令条数正确 x=64
  {

    //不到一页或正好一页
    if (!work_page || work_size == 512) 
    {
      addh=x*8-8;
      for (i = 0; i < work_size; i++)//读到直接地址
      {
        if (file.available())
          work_data[i] = file.read();
        else
          Serial.println("sdfsd error!");
      }
        //解码提取数据
        *a = ((work_data[addh + 0] << 8) + work_data[addh+1]) / 100.0;
        *b = ((work_data[addh + 2] << 8) + work_data[addh + 3]) / 100.0;
        *c = (work_data[addh + 4] << 24) + (work_data[addh + 5] << 16) + (work_data[addh + 6] << 8) + work_data[addh + 7];
    }





     //超过一页
    else if (work_size >= (work_page * 512))
    {
     

      //正好一页时并没溢出本页，页码要保持未满。
      if (work_size % 512 == 0)
      {
        addh=x*8-(512*(work_page-1))-8;Serial.printf("addh:%d\n",addh);
        Serial.printf("aaeee\n");
        for (i = 0; i < (work_page - 1); i++)
        {
          for (j = 0; j < 512; j++)
          {
            file.read();
            //Serial.printf("fdggfdg%d\n", j);
          }
        }
        //读取指令所在页的有用数据
        for (j = 0; j < (work_size - ((work_page-1) * 512)); j++)
        {
          if (file.available())
            work_data[j] = file.read();
          else
            Serial.printf("fdgght error:%d\n", j);
        }
        //提取解码数据
        *a = ((work_data[addh + 0] << 8) + work_data[addh+1]) / 100.0;
        *b = ((work_data[addh + 2] << 8) + work_data[addh + 3]) / 100.0;
        *c = (work_data[addh + 4] << 24) + (work_data[addh + 5] << 16) + (work_data[addh + 6] << 8) + work_data[addh + 7];
      }




      //真正的超过多页，溢出了。
      else
      {
        addh=x*8-(512*(work_page))-8;Serial.printf("addh:%d\n",addh);
        for (i = 0; i < work_page; i++)
        {
          for (j = 0; j < 512; j++)
          {
            file.read();
           // Serial.printf("fdggfdg%d\n", j);
          }
        }
        //读取指令所在页数据
        for (j = 0; j < (work_size - (work_page * 512)); j++)
        {
          if (file.available())
            work_data[j] = file.read();
          else
            Serial.printf("fdgght error:%d\n", j);
        }
        //提取数据
        *a = ((work_data[addh + 0] << 8) + work_data[addh+1]) / 100.0;
        *b = ((work_data[addh + 2] << 8) + work_data[addh + 3]) / 100.0;
        *c = (work_data[addh + 4] << 24) + (work_data[addh + 5] << 16) + (work_data[addh + 6] << 8) + work_data[addh + 7];
      }
    }

    

    // //显示work_data
    // for (i = 0; i < 512; i++)
    // {
    //   Serial.printf("dat[%d]:%d\n", i, work_data[i]);
    // }

  }
  else//这里数据读完了，已经可以将LOSE文件删除了
  {
    Serial.printf("NUM_COUNT ERROR! not lose_wenjian\n");
  }
  //关闭文件
  file.close();
}
//xieyi 解析某个lose指令 （int）
void get_lose_data(int x)
{
  float a,b;
  uint32_t c;
  get_data(SPIFFS, "/lose.hex", x,&a,&b, &c);
  Serial.println(a);
  Serial.println(b);
  Serial.println(c);
 
}


//lose 0~1024 填充数据测试用 debug : 0b
//写入数据通过：
void lose_tiancong()
{
  lose_count = 0;
  lose_first_flag = 1;

  //数据处理
  char a[8];
  sht20getTempAndHumi(); //读取温湿度，
  int temp = 1;          //(int)(currentTemp* 100); //温度值currentTemp
  int hum = 1;           //(int)(currentHumi * 100);  //湿度currentHumi
  uint32_t uinxt = 1;    //unixtime();
  a[0] = temp >> 8;
  a[1] = temp;
  a[2] = hum >> 8;
  a[3] = hum;
  a[4] = uinxt >> 24;
  a[5] = uinxt >> 16;
  a[6] = uinxt >> 8;
  a[7] = uinxt;
  //存储512次
  for (uint8_t i = 0; i < 255; i++)
  {
    if (lose_first_flag)
    {
      Serial.println("lose is first rec2222222");
      writeFile_test(SPIFFS, "/lose.hex", a);
      lose_first_flag = 0;
    }
    else
    {
      Serial.printf("write count:%d\n", i);
      appendFile_test(SPIFFS, "/lose.hex", a);
    }
  }
  //alFFS_readlose();true
  File file = SPIFFS.open("/lose.hex", "r");
  //2.检查打开成功和存在此文件
  if (!file || file.isDirectory())
  {
    Serial.println("- failed to open file for reading");
    return;
  }
  Serial.printf("file_size: %d\r\n", file.size());
  file.close();
}

//读lose
void read_lose()
{
   Serial.printf("f_lose=%d;lose_count = %d;\n",f_lose,lose_count);
   alFFS_readlose();
   
}



char work_dat[8];

//这个函数是舍弃512缓存的解析程序，不存在512内存缓存，只有8字节的解析内存，也就是输入一条指令条数返回条数的数据。
//用法：get_ffs_lose_data(SPIFFS, "/lose.hex", x,&a,&b,&c);
void get_ffs_lose_data(fs::FS &fs, const char *path, uint32_t x,float* a,float *b, uint32_t *c)
{
   //1.创建变量
  uint32_t i;
  int32_t addh=x*8-8;//首地址
  Serial.printf("addh :%d\n", addh);
  uint32_t work_size = 0; //工作字节长度
  uint32_t lose_size = 0; //漏发文件字节长度
  if(addh<0)
  {
     Serial.printf("addh cmd error:%d\n", addh);
     return;
  }
     
 
  //2.清解码缓存器
  for (int i = 0; i < 8; i++) work_dat[i] = 0;

  //3.输入文件名打开文件，读文件
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if (!file || file.isDirectory())
  {
    Serial.println("- failed to open file for reading");
    return;
  }

  //4.计算参数
  lose_size = file.size(); //文件长度 
  Serial.printf("lose_size:%d\n", lose_size);
  work_size = (x * 8); //需要解码的指令地址
  Serial.printf("work_size:%d\n", work_size);

  //5.开始解码
  if (lose_size >= work_size) //指令条数正确 x=64
  {
     if(addh<0) addh=0;
     Serial.printf("addh:%d\n", addh);
      
     for (i = 0; i <addh; i++)
        {
          if (file.available())file.read();
          else Serial.printf("fdnhnghgg error:%d\n", i);
        }
        //读取指令所在页的有用数据
        for (i = 0; i < 8; i++)
        {
          if (file.available()) work_dat[i] = file.read();
          else Serial.printf("ersght error:%d\n", i);
        }
            //显示work_data
        for (i = 0; i < 8; i++) Serial.printf("dat[%d]:%d\n", i, work_dat[i]);
        //提取解码数据
        *a = ((work_dat[0] << 8) + work_dat[1]) / 100.0;
        *b = ((work_dat[2] << 8) + work_dat[3]) / 100.0;
        *c = (work_dat[4] << 24) + (work_dat[5] << 16) + (work_dat[6] << 8) + work_dat[7];
        Serial.println(*a);
        Serial.println(*b);
        Serial.println(*c);
  }
  else//这里数据读完了，已经可以将LOSE文件删除了
  {
    Serial.printf("NUM_COUNT ERROR! not lose_wenjian\n");
  }
  //关闭文件
  file.close();

}



void test(int x)
{
  float a,b;
  uint32_t c;
  get_ffs_lose_data(SPIFFS, "/lose.hex", x,&a,&b,&c);

 
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
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
//json读lose文件
// void alFFS_readlose()
// {
//   File f = SPIFFS.open("/lose.json", FILE_READ);
//   Serial.println("lose file size:" + (String)f.size());
//   losestr1 = f.readString() + "]}";
//   Serial.println(losestr1);
//   f.close();
// }