#include "flash.h"
#include "spi.h"
#include "delay.h"


//4Kbytes为一个Sector
//16个扇区为1个Block
//W25X16
//容量为2M字节,共有32个Block,512个Sector

//初始化SPI FLASH的IO口
void spi2_flash_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  // PB12 推挽
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB,GPIO_Pin_12);


    spi2_init();        //初始化SPI
    spi2_set_speed(SPI_BaudRatePrescaler_2);//设置为18M时钟,高速模式
    //SPI_FLASH_TYPE=spi2_flash_read_id();//读取FLASH ID.

}

//读取SPI_FLASH的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
uint8_t spi2_flash_read_sr(void)
{
    uint8_t byte = 0;
    SPI2_FLASH_CS = 0;                            //使能器件
    spi2_read_write_byte(W25X_ReadStatusReg);    //发送读取状态寄存器命令
    byte = spi2_read_write_byte(0Xff);             //读取一个字节
    SPI2_FLASH_CS = 1;                            //取消片选
    return byte;
}
//写SPI_FLASH状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void spi2_flash_wirete_sr(u8 sr)
{
    SPI2_FLASH_CS = 0;                              //使能器件
    spi2_read_write_byte(W25X_WriteStatusReg);      //发送写取状态寄存器命令
    spi2_read_write_byte(sr);                       //写入一个字节
    SPI2_FLASH_CS = 1;                              //取消片选
}
//SPI_FLASH写使能
//将WEL置位
void spi2_flash_write_enable(void)
{
    SPI2_FLASH_CS = 0;                            //使能器件
    spi2_read_write_byte(W25X_WriteEnable);      //发送写使能
    SPI2_FLASH_CS = 1;                            //取消片选
}
//SPI_FLASH写禁止
//将WEL清零
void spi2_flash_write_disable(void)
{
    SPI2_FLASH_CS = 0;                              //使能器件
    spi2_read_write_byte(W25X_WriteDisable);        //发送写禁止指令
    SPI2_FLASH_CS = 1;                              //取消片选
}
//读取芯片ID W25X16的ID:0XEF14
u16 spi2_flash_read_id(void)
{
    u16 temp = 0;
    SPI2_FLASH_CS = 0;
    spi2_read_write_byte(0x90);//发送读取ID命令
    spi2_read_write_byte(0x00);
    spi2_read_write_byte(0x00);
    spi2_read_write_byte(0x00);
    temp |= spi2_read_write_byte(0xFF) << 8;
    temp |= spi2_read_write_byte(0xFF);
    SPI2_FLASH_CS = 1;
    return temp;
}
//读取SPI FLASH
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void spi2_flash_read(u8* buffer,u32 read_addr,u16 num_byte_to_read)
{
    u16 i;
    SPI2_FLASH_CS=0;                            //使能器件
    spi2_read_write_byte(W25X_ReadData);         //发送读取命令
    spi2_read_write_byte((u8)((read_addr) >> 16));  //发送24bit地址
    spi2_read_write_byte((u8)((read_addr) >> 8));
    spi2_read_write_byte((u8)read_addr);
    for(i = 0; i < num_byte_to_read; i++)
    {
        buffer[i] = spi2_read_write_byte(0XFF);   //循环读数
    }
    SPI2_FLASH_CS = 1;                            //取消片选
}
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
void spi2_flash_write_page(u8* buffer,u32 write_addr,u16 num_to_write)
{
    u16 i;
    spi2_flash_write_enable();                  //SET WEL
    SPI2_FLASH_CS = 0;                            //使能器件
    spi2_read_write_byte(W25X_PageProgram);      //发送写页命令
    spi2_read_write_byte((u8)((write_addr) >> 16)); //发送24bit地址
    spi2_read_write_byte((u8)((write_addr) >> 8));
    spi2_read_write_byte((u8)write_addr);
    for(i = 0; i < num_to_write; i++)spi2_read_write_byte(buffer[i]);//循环写数
    SPI2_FLASH_CS = 1;                            //取消片选
    spi2_flash_wait_busy();                 //等待写入结束
}
//无检验写SPI FLASH
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void spi2_flash_write_no_check(u8* buffer,u32 write_addr,u16 num_to_write)
{
    u16 pageremain;
    pageremain = 256 - write_addr % 256; //单页剩余的字节数
    if(num_to_write <= pageremain)pageremain = num_to_write;//不大于256个字节
    while(1)
    {
        spi2_flash_write_page(buffer, write_addr, pageremain);
        if(num_to_write == pageremain)break;//写入结束了
        else //NumByteToWrite>pageremain
        {
            buffer += pageremain;
            write_addr += pageremain;

            num_to_write -= pageremain;             //减去已经写入了的字节数
            if(num_to_write > 256)pageremain = 256; //一次可以写入256个字节
            else pageremain = num_to_write;         //不够256个字节了
        }
    };
}
//写SPI FLASH
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
u8 spi2_flash_buf[4096];
void spi2_flash_write(u8* buffer,u32 write_addr,u16 num_to_write)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;

    secpos=write_addr / 4096;//扇区地址 0~511 for w25x16
    secoff=write_addr % 4096;//在扇区内的偏移
    secremain = 4096 - secoff;//扇区剩余空间大小

    if(num_to_write <= secremain)
    {
        secremain = num_to_write;//不大于4096个字节
    }
    while(1)
    {
        spi2_flash_read(spi2_flash_buf,secpos*4096,4096);//读出整个扇区的内容
        for(i=0;i<secremain;i++)//校验数据
        {
            if(spi2_flash_buf[secoff+i] != 0XFF)break;//需要擦除
        }
        if(i<secremain)//需要擦除
        {
            spi2_flash_erase_sector(secpos);//擦除这个扇区
            for(i=0;i<secremain;i++)        //复制
            {
                spi2_flash_buf[i+secoff]=buffer[i];
            }
            spi2_flash_write_no_check(spi2_flash_buf,secpos*4096,4096);//写入整个扇区

        }else spi2_flash_write_no_check(buffer, write_addr, secremain);//写已经擦除了的,直接写入扇区剩余区间.
        if(num_to_write == secremain)break;//写入结束了
        else//写入未结束
        {
            secpos++;//扇区地址增1
            secoff=0;//偏移位置为0

            buffer += secremain;  //指针偏移
            write_addr += secremain;//写地址偏移
            num_to_write -= secremain;                  //字节数递减
            if(num_to_write > 4096)secremain = 4096;    //下一个扇区还是写不完
            else secremain = num_to_write;              //下一个扇区可以写完了
        }
    };
}
//擦除整个芯片
//整片擦除时间:
//W25X16:25s
//W25X32:40s
//W25X64:40s
//等待时间超长...
void spi2_flash_erase_chip(void)
{
    spi2_flash_write_enable();                  //SET WEL
    spi2_flash_wait_busy();
    SPI2_FLASH_CS = 0;                            //使能器件
    spi2_read_write_byte(W25X_ChipErase);        //发送片擦除命令
    SPI2_FLASH_CS = 1;                            //取消片选
    spi2_flash_wait_busy();                     //等待芯片擦除结束
}
//擦除一个扇区
//Dst_Addr:扇区地址 0~511 for w25x16
//擦除一个山区的最少时间:150ms
void spi2_flash_erase_sector(u32 dst_addr)
{
    dst_addr *= 4096;
    spi2_flash_write_enable();                  //SET WEL
    spi2_flash_wait_busy();
    SPI2_FLASH_CS = 0;                            //使能器件
    spi2_read_write_byte(W25X_SectorErase);      //发送扇区擦除指令
    spi2_read_write_byte((u8)((dst_addr) >> 16));  //发送24bit地址
    spi2_read_write_byte((u8)((dst_addr) >> 8));
    spi2_read_write_byte((u8)dst_addr);
    SPI2_FLASH_CS = 1;                            //取消片选
    spi2_flash_wait_busy();                         //等待擦除完成
}
//等待空闲
void spi2_flash_wait_busy(void)
{
    while ((spi2_flash_read_sr() & 0x01) == 0x01);   // 等待BUSY位清空
}
//进入掉电模式
void spi2_flash_power_dow(void)
{
    SPI2_FLASH_CS=0;                            //使能器件
    spi2_read_write_byte(W25X_PowerDown);        //发送掉电命令
    SPI2_FLASH_CS=1;                            //取消片选
    delay_us(3);                               //等待TPD
}
//唤醒
void spi2_flash_wakeup(void)
{
    SPI2_FLASH_CS=0;                            //使能器件
    spi2_read_write_byte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB
    SPI2_FLASH_CS=1;                            //取消片选
    delay_us(3);                               //等待TRES1
}


uint8_t spi2_flash_write_data(uint32_t addr, uint8_t *data, uint16_t len)
{
    spi2_flash_write_enable();
//    spi2_flash_write_page(data, addr, len);
    spi2_flash_write(data, addr, len);
    spi2_flash_write_disable();
    return 1;
}


uint8_t spi2_flash_read_data(uint32_t addr, uint8_t *data, uint16_t len)
{
    spi2_flash_read(data, addr, len);
    return 1;
}





