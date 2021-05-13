/** Ds1302.cpp
 *
 * Ds1302 class.
 *
 * @version 1.0.3
 * @author Rafa Couto <caligari@treboada.net>
 * @license GNU Affero General Public License v3.0
 * @see https://github.com/Treboada/Ds1302
 *
 */

#include "Ds1302.h"

#include <Arduino.h>
//ds1302寄存器
#define REG_SECONDS           0x80 //秒寄存器
#define REG_MINUTES           0x82 //分寄存器
#define REG_HOUR              0x84 //小时寄存器
#define REG_DATE              0x86 //日期寄存器
#define REG_MONTH             0x88  //月寄存器
#define REG_DAY               0x8A  //天寄存器
#define REG_YEAR              0x8C  //年寄存器
#define REG_WP                0x8E  //控制寄存器
#define REG_BURST             0xBE   //时钟脉冲

//构造函数 设置ds1302管脚
Ds1302::Ds1302(uint8_t pin_ena, uint8_t pin_clk, uint8_t pin_dat)
{
    _pin_ena = pin_ena;
    _pin_clk = pin_clk;
    _pin_dat = pin_dat;
    _dat_direction = INPUT;
}
void Ds1302::init()
{
    pinMode(_pin_ena, OUTPUT);
    pinMode(_pin_clk, OUTPUT);
    pinMode(_pin_dat, _dat_direction);

    digitalWrite(_pin_ena, LOW);
    digitalWrite(_pin_clk, LOW);
}

//检查DS1302运行状态
bool Ds1302::isHalted()
{
    _prepareRead(REG_SECONDS);
    uint8_t seconds = _readByte();
    _end();
    return (seconds & 0b10000000);//返回秒的最高位
}


void Ds1302::getDateTime(DateTime* dt)
{
    _prepareRead(REG_BURST);
    dt->second = _bcd2dec(_readByte() & 0b01111111);
    dt->minute = _bcd2dec(_readByte() & 0b01111111);
    dt->hour   = _bcd2dec(_readByte() & 0b00111111);
    dt->day    = _bcd2dec(_readByte() & 0b00111111);
    dt->month  = _bcd2dec(_readByte() & 0b00011111);
    dt->dow    = _bcd2dec(_readByte() & 0b00000111);
    dt->year   = _bcd2dec(_readByte() & 0b01111111);
    _end();
}


void Ds1302::setDateTime(DateTime* dt)
{
    _prepareWrite(REG_WP);
    _writeByte(0b00000000);
    _end();

    _prepareWrite(REG_BURST);
    _writeByte(_dec2bcd(dt->second % 60 ));
    _writeByte(_dec2bcd(dt->minute % 60 ));
    _writeByte(_dec2bcd(dt->hour   % 24 ));
    _writeByte(_dec2bcd(dt->day    % 32 ));
    _writeByte(_dec2bcd(dt->month  % 13 ));
    _writeByte(_dec2bcd(dt->dow    % 8  ));
    _writeByte(_dec2bcd(dt->year   % 100));
    _writeByte(0b10000000);
    _end();
}


void Ds1302::halt()
{
    _prepareWrite(REG_SECONDS);
    _writeByte(0b10000000);//停止震荡
    _end();
}

//准备读 ：发送读命令
void Ds1302::_prepareRead(uint8_t address)
{
    _setDirection(OUTPUT);
    digitalWrite(_pin_ena, HIGH);
    uint8_t command = 0b10000001 | address;
    _writeByte(command);
    _setDirection(INPUT);
}

//准备写 ： 发送写命令
void Ds1302::_prepareWrite(uint8_t address)
{
    _setDirection(OUTPUT);
    digitalWrite(_pin_ena, HIGH);
    uint8_t command = 0b10000000 | address;
    _writeByte(command);
}

//读写操作完成，使ds1302恢复运行
void Ds1302::_end()
{
    digitalWrite(_pin_ena, LOW);
}

//读取一个字节
uint8_t Ds1302::_readByte()
{
    uint8_t byte = 0;

    for(uint8_t b = 0; b < 8; b++)
    {
        if (digitalRead(_pin_dat) == HIGH) byte |= 0x01 << b;
        _nextBit();
    }

    return byte;
}

//写一个字节
void Ds1302::_writeByte(uint8_t value)
{
    for(uint8_t b = 0; b < 8; b++)
    {
        digitalWrite(_pin_dat, (value & 0x01) ? HIGH : LOW);
        _nextBit();
        value >>= 1;
    }
}
//clock操作
void Ds1302::_nextBit()
{
        digitalWrite(_pin_clk, HIGH);
        delayMicroseconds(1);//系统延时us

        digitalWrite(_pin_clk, LOW);
        delayMicroseconds(1);
}

//设置data引脚方向
void Ds1302::_setDirection(int direction)
{
    if (_dat_direction != direction)
    {
        _dat_direction = direction;
        pinMode(_pin_dat, direction);
    }
}

//十进制转BDC
uint8_t Ds1302::_dec2bcd(uint8_t dec)
{
    return ((dec / 10 * 16) + (dec % 10));
}

//bcd转十进制
uint8_t Ds1302::_bcd2dec(uint8_t bcd) 
{
    return ((bcd / 16 * 10) + (bcd % 16));
}


