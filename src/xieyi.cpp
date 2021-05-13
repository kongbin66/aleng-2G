#include "Arduino.h"
#include "xieyi.h"

 uint8_t CMD_NUM=0;//±»µ÷ÓÃº¯Êý×ÜÊý

 uint8_t rx_buff[num];
//æ¸…æŽ¥æ”¶ç¼“å†²åŒº
void clear_rx_buff(void)
{
  int i;
  for(i=0;i<num;i++) rx_buff[i]=0;
}
uint8_t exe(void)
{
    int res=0;
    switch(M_name_par.GS)
    {
    case 0://æ— å‚æ•?(voidç±»åž‹)
        res=(*(int(*)())M_name_par.pfune->pf)();
        break;
    case 1:
        res=(*(int(*)(uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0]);
        break;
    case 2://æœ?2ä¸?å‚æ•°
        res=(*(int(*)(uint32_t,uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0],M_name_par.cs[1]);
        break;
    case 3://æœ?3ä¸?å‚æ•°
        res=(*(int(*)(uint32_t,uint32_t,uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0],M_name_par.cs[1],M_name_par.cs[2]);
        break;
    case 4://æœ?4ä¸?å‚æ•°
        res=(*(int(*)(uint32_t,uint32_t,uint32_t,uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0],M_name_par.cs[1],M_name_par.cs[2],M_name_par.cs[3]);
        break;
    case 5://æœ?5ä¸?å‚æ•°
        res=(*(int(*)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0],M_name_par.cs[1],M_name_par.cs[2],M_name_par.cs[3],M_name_par.cs[4]);
        break;
    case 6://æœ?6ä¸?å‚æ•°
        res=(*(int(*)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0],M_name_par.cs[1],M_name_par.cs[2],M_name_par.cs[3],M_name_par.cs[4],M_name_par.cs[5]);
        break;
    case 7://æœ?7ä¸?å‚æ•°
        res=(*(int(*)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0],M_name_par.cs[1],M_name_par.cs[2],M_name_par.cs[3],M_name_par.cs[4],M_name_par.cs[5],M_name_par.cs[6]);
        break;
    case 8://æœ?8ä¸?å‚æ•°
        res=(*(int(*)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0],M_name_par.cs[1],M_name_par.cs[2],M_name_par.cs[3],M_name_par.cs[4],M_name_par.cs[5],M_name_par.cs[6],M_name_par.cs[7]);
        break;
    }
    return res;
}

//»ñÈ¡µ÷ÓÃº¯Êý¹¦ÄÜÂëÎ»ÖÃ
//ÊäÈë£º½ÓÊÕµ½µÄ¹¦ÄÜÂë CMD ,
//Êä³ö£º¹¦ÄÜÂëÎ»ÖÃ *p
//·µ»Ø£º0³É¹¦ £¬£¡0´íÎó
uint8_t get_fun_ID(uint8_t cmd,uint8_t *p)
{
    uint8_t i=0;
    for(i=0; i<CMD_NUM; i++)
    {
        if(cmd==name_tab[i].cmd[1])
        {
            *p=i;
            return 0;
        }
    }
    return 1;
}
//è¾¨è?¤å‚æ•°æ ¼å¼?:ä¸?æ•°å’Œç±»åž‹
void get_fun_par_gslx()
{
     uint8_t i,j;
     uint8_t buf[CMD_data];
     uint8_t par[CMD_data]; //çŠ¶æ€æš‚å­?
    for(i=0; i<(CMD_data); i++) par[i]=0;
    j=2;//æŒ‡ä»¤å­—èŠ‚æ‰€åœ¨ä½ç½?
    for(i=0; i<(CMD_data); i++) //å°†çº¯å‚æ•°æ”¾å…¥æ•°ç»„
    {
        buf[i]=name_tab[M_name_par.ID].cmd[j];
        j++;
    }
#if 0
    printf("*****************************************\r\n");
    for(i=0; i<(CMD_data); i++) //æ˜¾ç¤ºä¸²å£æŽ¥æ”¶å€?
    {
        printf("buf[%d]=%d\r\n",i,buf[i]);
    }
#endif
    for(i=0; i<(CMD_data); i++) //è¾¨è?¤å‚æ•? pra
    {
        if(buf[i]!=0)  par[buf[i]-1]++;
    }
    j=0;
    for(i=0; i<(CMD_data); i++) //è¾¨è?¤å‚æ•? GS
    {
        if(par[i]!=0) j++;
    }
    M_name_par.GS=j;
    for(i=0; i<(CMD_data); i++) M_name_par.pra[i]=par[i];
#if 0
    printf("*****************************************\r\n");
    for(i=0; i<(CMD_data); i++) //æ˜¾ç¤ºä¸²å£æŽ¥æ”¶å€?
    {
        printf("par[%d]=%d\r\n",i,M_name_par.pra[i]);
    }
    printf("GS=%d\r\n",M_name_par.GS);
#endif
}

//»ñÈ¡Ö¸Áî¸ñÊ½¡¢Ö¸Áî¡¢²ÎÊý¸öÊý¡¢²ÎÊýÀàÐÍ¡¢·µ»ØÀàÐÍ
//ÊäÈë£º»ñÈ¡µ½µÄº¯Êý±íÎ»ÖÃ id
void get_fun_par(uint8_t id)
{
    M_name_par.pfune =&name_tab[id];//Î»ÖÃ
    M_name_par.ID=id;
    M_name_par.CMD=name_tab[id].cmd[1];
    //½âÎöº¯Êý²ÎÊýµÄ¸öÊýºÍÀàÐÍ
    get_fun_par_gslx();
}


//å–æ•°æ?
uint32_t yiwei1(uint8_t *buf)
{
    uint32_t i;
    uint8_t j;
    i=buf[0];//å°†æ•°æ?å­˜æ”¾

    for(j=0; j<(CMD_data-1); j++)//å°†ç??ä¸€ä¸?æ•°æ®ç§»é™¤æŽ?
    {
        buf[j]=buf[j+1];
    }
    return i;
}
//å–uint16_tæ•°æ®
uint32_t yiwei2(uint8_t*buf)
{
    uint32_t i;
    uint8_t j;
    uint8_t k=2;
    i=((uint16_t)buf[0]<<8)+buf[1];


    for(j=0; j<(CMD_data-2); j++)
    {
        buf[j]=buf[k];//ç¬?äºŒæ?¡è?…è½½BUF,
        k++;
    }
    return i;

}
//å–uint32_tæ•°æ®
uint32_t yiwei4(uint8_t *buf)
{
    uint32_t i;
    uint8_t j;
    uint8_t k=4;
    i=((uint32_t)buf[0]<<24)+((uint32_t)buf[1]<<16)+((uint32_t)buf[2]<<8)+buf[3];

    for(j=0; j<(CMD_data-4); j++)
    {
        buf[j]=buf[k];;//ç¬?äºŒæ?¡è?…è½½BUF,
        k++;
    }

    return i;
}



void get_uart_par(uint8_t *buf)
{
     uint8_t i;
     uint8_t cbuf[CMD_data];

    for(i=0; i<(CMD_data); i++)
    {
        cbuf[i]=buf[i];
    }
#if 0
    printf("*****************************************\r\n");
    for(i=0; i<(CMD_data); i++) //æ˜¾ç¤ºä¸²å£æŽ¥æ”¶å€?
    {
        printf("cbuf[%d]=%d\r\n",i,cbuf[i]);
    }
#endif
    //å‚æ•°æ˜?æœ‰é™åˆ¶çš„
    for(i=0; i<CMD_data; i++)
    {
        if(M_name_par.pra[i]!=0)
        {
            switch(M_name_par.pra[i])
            {
            case 1://
                M_name_par.cs[i]= yiwei1(cbuf);
                break;
            case 2://uint16_t
                M_name_par.cs[i]= yiwei2(cbuf);
                break;
            case 4://uint32_t
                M_name_par.cs[i]=yiwei4(cbuf);
                break;
            }
        }
    }

#if 0
    printf("*****************************************\r\n");
    for(i=0; i<(CMD_data); i++) //æ˜¾ç¤ºä¸²å£æŽ¥æ”¶å€?
    {
        printf("cs[%d]=%d\r\n",i,M_name_par.cs[i]);
    }
#endif
}

uint8_t jiexi(uint8_t *buf,int x)
{
  uint8_t res=0,id=0;
  //ºË¶ÔAA BB
  if(rx_buff[0]==0xaa&&rx_buff[x-1]==0xbb)
  {
       #if 0
       Serial.println("Ö¸Áî»ñÈ¡AABBÕýÈ·\n");
       #endif

       
       res=get_fun_ID(rx_buff[1],&id);
       if(!res)
       {
           //Serial.printf("»ñÈ¡ID:%d\n",id);
                get_fun_par(id);////èŽ·å–å‡½æ•°è¡¨ä¸­çš„æ ¼å¼? æŒ‡ä»¤ å‚æ•°ä¸?æ•°ï¼Œå‚æ•°ç±»åž‹ è¿”å›žç±»åž‹
                get_uart_par(&rx_buff[2]);
                clear_rx_buff();
                exe();
                return 0;
       }
       else Serial.println("ÎÞ´ËÖ¸Áî£¡");
  }
  else
  {
     Serial.println("Ö¸ÁîÃ»»ñÈ¡µ½AABB!\n");
  }
  return 1;
}





//aa 00 a0 01 01 01 01 01 01 01 bb
void xieyi_scan(void)
{
    static uint8_t f=0;
    static int i= 0,J=0;
   // uint8_t sta=0;//×´Ì¬
    if(J==0)
	 {
		  J=1;
		  init_xieyi(&CMD_NUM);
		  #if DEBUG
		  printf("NUM=%d\r\n",CMD_NUM);
		  #endif
	 }
    //½ÓÊÕÍ¨Ñ¶Êý¾Ý£¬ÕÒµ½Ö¸Áî³¤¶È£¬ºÍÊý¾Ý³¤¶È
    while(Serial.available())
   {
      rx_buff[i++] =Serial.read(); 
      f=1;    
   }
   if(f==1)
   {
      f=0;


      
      #if DEBUG
        //²é¿´½ÓÊÕµÄÊý¾ÝÃ»ÑéÖ¤
      Serial.println("½ÓÊÕµ½Êý¾Ý£¡");
      Serial.println();
    //   for(i=0;i<x;i++)
    //   Serial.write(rx_buff[i]);
      Serial.println();
      Serial.println();
      uint8_t I_CMDlong=i;//Ö¸Áî³¤¶È
      uint8_t I_CMDdata =i-3;//Êý¾Ý³¤¶È
      uint8_t CMD=rx_buff[1];//±¾Ö¸Áî¹¦ÄÜÂë
      Serial.printf("Í¨Ñ¶Ö¸Áî³¤%d×Ö½Ú£¬²ÎÊý³¤¶È%d×Ö½Ú£¬±¾Ö¸Áî¹¦ÄÜÂë%d\r\n",I_CMDlong,I_CMDdata,CMD);
      #endif




      if(i==CMD_long)//µÈ³¤Ö¸Áî
      {
              //½âÎöÖ¸Áî
        jiexi(rx_buff,i);
      
      }
      else Serial.println("Ö¸Áî³¤¶È¸ñÊ½´íÎó£¡");
      
 
   }
   i=0;
  





	//   if(i==0)
	//  {
	// 	  i=1;
	// 	  init_xieyi(&CMD_NUM);
	// 	  #if 0
	// 	  printf("NUM=%d\r\n",CMD_NUM);
	// 	  #endif
	//  }
//     if(USART1_RX_STA&0x8000)
//     {
//         if((USART1_rx_buff[0]==0xaa)&&(USART1_rx_buff[CMD_long-1]==0xbb))
//         {
//             res=get_fun_ID(USART1_rx_buff[1],&id);//å°†ä¸²å£çš„æ•°æ®è§£æžæˆå‡½æ•°è¡¨ä¸?çš„å“ªä¸?å‡½æ•°ID
//             if(!res)
//             {
//                 get_fun_par(id);////èŽ·å–å‡½æ•°è¡¨ä¸­çš„æ ¼å¼? æŒ‡ä»¤ å‚æ•°ä¸?æ•°ï¼Œå‚æ•°ç±»åž‹ è¿”å›žç±»åž‹
//                 get_uart_par(&USART1_rx_buff[2]);
//                 exe();
//             }
// #if DEBUG
//             else printf("æ— æ?¤æŒ‡ä»¤ï¼ï¼ï¼\r\n");
// #endif
// #if 0
//             printf("*****************************************\r\n");
// 						if(!res)//æ˜¾ç¤ºæŒ‡ä»¤çŠ¶æ€?
//                 printf("è¯»å–æŒ‡ä»¤æˆåŠŸï¼?,è¯»å–çš„æŒ‡ä»?=%d,ä¸²å£æŒ‡ä»¤å?=%d,çŠ¶æ€ï¼šOK!!\r\n",id,USART1_rx_buff[1]);
//             else printf("è¯»å–æŒ‡ä»¤å¤±è´¥ï¼?,è¯»å–çš„æŒ‡ä»?=%d,cmd=%d,err!!\r\n",id,USART1_rx_buff[1]);
//             for(id=0; id<CMD_long; id++) //æ˜¾ç¤ºä¸²å£æŽ¥æ”¶å€?
//             {
//                 printf("buf[%d]=%d\r\n",id,USART1_rx_buff[id]);
//             }

// #endif
//             USART1_rx_buff[0]=0;
//              USART1_rx_buff[CMD_long-1]=0;
//         }
// #if DEBUG
//         else printf("æ ¼å¼é”™è??ï¼\r\n");
// #endif
//         USART1_RX_STA=0;
//     }
}




// void jiexi(char *buf,int x)
// {
//   uint8_t i,res,id;
//   //æ˜¾ç¤ºä¼ é€è¿‡æ¥çš„å†…å??
//   #if 1
//      for(i=0;i<x;i++)
//       Serial.write(rx_buff[i]);
//       Serial.println();
//   #endif
//   //åˆ¤æ–­AA BB
//   if(rx_buff[0]==0xaa&&rx_buff[x-1]==0xbb)
//   {
//        Serial.println("CMD ok!");
//        //åˆ¤æ–­åŠŸèƒ½ç ?
//        res=get_fun_ID(rx_buff[1],&id);
    
//        //åˆ¤æ–­å‚æ•°ä¸?æ•?
//        //æ‰§è?Œå‘½ä»¤æ“ä½?
//   }
//   else
//   {
//      Serial.println("CMD ERR!");

//   }
// }


// void xieyi(void)
// {
//    static uint8_t f=0;
//    static int i= 0;
//    while(Serial.available())
//    {
//       rx_buff[i++] =Serial.read(); 
//       f=1;    
//    }
//    if(f==1)
//    {
//       f=0;
//       jiexi(rx_buff,i);
//    }
//    i=0;
// }






















































