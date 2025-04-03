#ifndef IICFUNCS_H
#define IICFUNCS_H
#include <avr/io.h>
#include <util/twi.h>

#ifdef __cplusplus
extern "C" {
#endif

  void IIC_Init();
  void IIC_WriteByte(uint8_t addr, uint8_t reg, uint8_t data);
  void IIC_WriteBytes(uint8_t addr, uint8_t reg, const uint8_t* data, uint8_t num);
  uint8_t IIC_ReadByte(uint8_t addr, uint8_t reg);
  void IIC_ReadBytes(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t num);

#ifdef __cplusplus
}
#endif

#endif  // IICFUNCS_H