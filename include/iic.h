#ifndef __IIC_H__
#define __IIC_H__
#include "system.h"

//------------IIC ping define----------//
//GPIO_B
#define IIC_SDA_Out	PBout(11)
#define IIC_SDA_In	PBin(11)
#define IIC_SCL_Out	PBout(10)


void IIC_Init(void);

#endif /*__IIC_H__*/
