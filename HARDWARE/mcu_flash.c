#include "mcu_flash.h"
#include "delay.h"
#include "usart.h"
#include "string.h"


//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
    return *(vu16*)faddr;
}
#if STM32_FLASH_WREN    //如果使能了写
//不检查的写入
//write_addr:起始地址
//buffer:数据指针
//num_to_write:半字(16位)数
void STMFLASH_Write_NoCheck(u32 write_addr, u16 *buffer, u16 num_to_write)
{
    u16 i;
    for(i = 0; i < num_to_write; i++)
    {
        FLASH_ProgramHalfWord(write_addr, buffer[i]);
        write_addr += 2;//地址增加2.
    }
}
//从指定地址开始写入指定长度的数据
//write_addr:起始地址(此地址必须为2的倍数!!)
//buffer:数据指针
//num_to_write:半字(16位)数(就是要写入的16位数据的个数.)
u16 mcu_flash_buf[STM_SECTOR_SIZE / 2];//最多是2K字节
void flash_write(u32 write_addr, u16 *buffer, u16 num_to_write)
{
    u32 sector_pos;     //扇区地址
    u16 sector_offset;     //扇区内偏移地址(16位字计算)
    u16 sector_remain;  //扇区内剩余地址(16位字计算)
    u16 i;
    u32 offset_addr;    //去掉0X08000000后的地址
    if(write_addr < STM32_FLASH_BASE || (write_addr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE)))
    {
        return;//非法地址
    }
    FLASH_Unlock();     //解锁
    offset_addr = write_addr - STM32_FLASH_BASE;    //实际偏移地址.
    sector_pos = offset_addr / STM_SECTOR_SIZE;         //扇区地址  0~127 for STM32F103RBT6
    sector_offset = (offset_addr % STM_SECTOR_SIZE) / 2;   //在扇区内的偏移(2个字节为基本单位.)
    sector_remain = STM_SECTOR_SIZE / 2 - sector_offset;   //扇区剩余空间大小
    if(num_to_write <= sector_remain)
    {
        sector_remain = num_to_write;//不大于该扇区范围
    }
    while(1)
    {
        flash_read(sector_pos * STM_SECTOR_SIZE + STM32_FLASH_BASE, mcu_flash_buf, STM_SECTOR_SIZE / 2);//读出整个扇区的内容
        for(i = 0; i < sector_remain; i++)//校验数据
        {
            if(mcu_flash_buf[sector_offset + i] != 0XFFFF)
            {
                break;//需要擦除
            }
        }
        if(i < sector_remain)//需要擦除
        {
            FLASH_ErasePage(sector_pos * STM_SECTOR_SIZE + STM32_FLASH_BASE);//擦除这个扇区
            for(i = 0; i < sector_remain; i++)//复制
            {
                mcu_flash_buf[i + sector_offset] = buffer[i];
            }
            STMFLASH_Write_NoCheck(sector_pos * STM_SECTOR_SIZE + STM32_FLASH_BASE, mcu_flash_buf, STM_SECTOR_SIZE / 2);//写入整个扇区
        }else
        {
            STMFLASH_Write_NoCheck(write_addr, buffer, sector_remain);//写已经擦除了的,直接写入扇区剩余区间.
        }
        if(num_to_write == sector_remain)
        {
            break;//写入结束了
        }
        else//写入未结束
        {
            sector_pos++;               //扇区地址增1
            sector_offset = 0;             //偏移位置为0
            buffer += sector_remain;   //指针偏移
            write_addr += sector_remain;    //写地址偏移
            num_to_write -= sector_remain;  //字节(16位)数递减
            if(num_to_write > (STM_SECTOR_SIZE / 2))
            {
                sector_remain = STM_SECTOR_SIZE / 2;//下一个扇区还是写不完
            }
            else
            {
                sector_remain = num_to_write;//下一个扇区可以写完了
            }
        }
    };
    FLASH_Lock();//上锁
}
#endif

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//buffer:数据指针
//num_to_write:半字(16位)数
void flash_read(u32 ReadAddr,u16 *buffer,u16 NumToRead)
{
    u16 i;
    for(i=0;i<NumToRead;i++)
    {
        buffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
        ReadAddr+=2;//偏移2个字节.
    }
}


uint8_t flash_erase(uint32_t addr)
{
    if(addr % STM_SECTOR_SIZE == 0)
    {
        FLASH_Unlock(); //解锁
        FLASH_ErasePage(addr);
        FLASH_Lock();   //上锁
        return 1;
    }
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//write_addr:起始地址
//WriteData:要写入的数据
void Test_Write(u32 write_addr,u16 WriteData)
{
    flash_write(write_addr,&WriteData,1);//写入一个字
}



