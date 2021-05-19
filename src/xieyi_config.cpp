 #include "xieyi.h"
 //1.引用外部函数 

void SET_SLEEPTIME(time_t t);
void SET_Last_span_Sleep_span(int x,int y);
void set_fxmode(char ,char,char);
void read_list();
void jiexi_lose(bool);
void set_dbug(uint8_t a);
void test_power(bool x);
void read_lose();
void spiffs_size();
void FFS_fromat();
void alFFS_savelist();
void sys_time();
void lose_tiancong();
void my_listDir(bool x);
void get_lose_data(int);
void SET_ds1302(int year, char momtch, char day, char hour, char minute, char second);

void test(int);

//2.填写指令参数格式
  NAME_TAB name_tab[]=
 {
	  (void*)SET_SLEEPTIME,                                   {0xaa,0x00, 1, 1, 1, 1, 0, 0,0,0, 0xbb},
      (void*)SET_Last_span_Sleep_span,                        {0xaa,0x01, 1, 1, 1, 1, 2, 2,2,2, 0xbb},
	  (void*)set_fxmode,                                      {0xaa,0x02, 1, 2, 3, 0, 0, 0,0,0, 0xbb},
      (void*)read_list,                                       {0xaa,0x03, 1, 2, 3, 0, 0, 0,0,0, 0xbb},
	  (void*)jiexi_lose,                                      {0xaa,0x04, 0, 0, 0, 0, 0, 0,0,0, 0xbb},
      (void*)set_dbug,                                        {0xaa,0xdb, 1, 0, 0, 0, 0, 0,0,0, 0xbb},
	  (void*)test_power,                                      {0xaa,0x05, 1, 0, 0, 0, 0, 0,0,0, 0xbb},
	  (void*)read_lose,                                       {0xaa,0x06, 1, 0, 0, 0, 0, 0,0,0, 0xbb},
	  (void*)spiffs_size,                                     {0xaa,0x07, 0, 0, 0, 0, 0, 0,0,0, 0xbb},
	  (void*)FFS_fromat,                                      {0xaa,0x08, 0, 0, 0, 0, 0, 0,0,0, 0xbb},
	  (void*)alFFS_savelist,                                  {0xaa,0x09, 0, 0, 0, 0, 0, 0,0,0, 0xbb},
	  (void*)sys_time,                                        {0xaa,0x0a, 0, 0, 0, 0, 0, 0,0,0, 0xbb},
	  (void*)lose_tiancong,                                   {0xaa,0x0b, 0, 0, 0, 0, 0, 0,0,0, 0xbb},
	  (void*)my_listDir,                                      {0xaa,0x0c, 1, 0, 0, 0, 0, 0,0,0, 0xbb},
	  (void*)get_lose_data,                                   {0xaa,0x0d, 1, 1, 0, 0, 0, 0,0,0, 0xbb},
	  (void*)SET_ds1302,                                      {0xaa,0x0e, 1, 1, 2, 3, 4, 5,6,0, 0xbb},
	  (void*)test,                                            {0xaa,0xff, 1, 1, 0, 0, 0, 0,0,0, 0xbb},
	 
 };
//3.根据函数表修改xieyi.h的tab
//4.在主程序中调用xieyi_scan();






















 
  void init_xieyi(uint8_t *p)
 {
     *p = (sizeof(name_tab)/sizeof(name_tab[0]));
 } 
 
NAME_par  M_name_par =
{
	 name_tab,
	 exe,
	 0,//ID
	 0,//CMD
	 0,// uint8_t GS;
	 0,//uint8_t pra1;
	 0,//uint8_t cs2;
};


