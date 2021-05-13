/*----------------------------------------------------------
*                 sht20操作相关
*
*---------------------------------------------------------*/
#include "config.h"
void sht20getTempAndHumi()
{
  float _currentTemp = sht20.temperature();
  float _currentHumi = sht20.humidity();

  if (_currentTemp < 100)
  {
    currentTemp = _currentTemp;
    currentHumi = _currentHumi;
    tempAndHumi_Ready = true;
    //postMsgId++;
  }
}