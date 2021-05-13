/** Ds1302.h
 *
 * Ds1302 class.
 *
 * @version 1.0.3
 * @author Rafa Couto <caligari@treboada.net>
 * @license GNU Affero General Public License v3.0
 * @see https://github.com/Treboada/Ds1302
 *
 */

#ifndef _DS_1302_H
#define _DS_1302_H

#include <stdint.h>

class Ds1302
{
    public:

        typedef struct {
            uint8_t year;
            uint8_t month;
            uint8_t day;
            uint8_t hour;
            uint8_t minute;
            uint8_t second;
            uint8_t dow;
        } DateTime;

        /**
         * Months of year
         */
        enum MONTH : uint8_t {
            MONTH_JAN = 1,
            MONTH_FEB = 2,
            MONTH_MAR = 3,
            MONTH_APR = 4,
            MONTH_MAY = 5,
            MONTH_JUN = 6,
            MONTH_JUL = 7,
            MONTH_AUG = 8,
            MONTH_SET = 9,
            MONTH_OCT = 10,
            MONTH_NOV = 11,
            MONTH_DEC = 12
        };

        /**
         * Days of week
         */
        enum DOW : uint8_t {
            DOW_MON = 1,
            DOW_TUE = 2,
            DOW_WED = 3,
            DOW_THU = 4,
            DOW_FRI = 5,
            DOW_SAT = 6,
            DOW_SUN = 7
        };

        /**
         * Constructor (pin configuration).构造函数（引脚配置）。
         */
        Ds1302(uint8_t pin_ena, uint8_t pin_clk, uint8_t pin_dat);

        /**
         * Initializes the DW1302 chip.初始化DW1302芯片通讯引脚，但不发命令。
         */
        void init();

        /**
         * Returns when the oscillator is disabled. 查询芯片运行状态，当振荡器被禁用时返回。
         * 秒寄存器的最高位记录ds1302状态
         */
        bool isHalted();

        /**
         * Stops the oscillator. 软件停止振荡器。
         * 1停止 0启动
         */
        void halt();

        /**
         * Returns the current date and time.返回当前日期和时间。
         * 是BCD转DEC后的数据
         */
        void getDateTime(DateTime* dt);;

        /**
         * Sets the current date and time. 设置当前日期和时间。
         * 是dec转bcd后的数据
         */
        void setDateTime(DateTime* dt);

    private:

        uint8_t _pin_ena;
        uint8_t _pin_clk;
        uint8_t _pin_dat;
        //设置为读数据
        void _prepareRead(uint8_t address);
        //设置为写命令
        void _prepareWrite(uint8_t address);
        //DS1302 片选失能
        void _end();

        int _dat_direction;
        //I/O脚设定状态
        void _setDirection(int direction);
        //读一字节数据
        uint8_t _readByte();
        //写数据
        void _writeByte(uint8_t value);
        //转下一个要发送的位
        void _nextBit();
        //编码转换10进制转BCD
        uint8_t _dec2bcd(uint8_t dec);
        //编码转换BCD转10进制
        uint8_t _bcd2dec(uint8_t bcd);
};

#endif // _DS_1302_H

