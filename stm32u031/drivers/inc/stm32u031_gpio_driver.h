#ifndef STM32U031_GPIO_DRIVER_H_
#define STM32U031_GPIO_DRIVER_H_

#include <stdint.h>
#include "stm32u031xx.h"

#define GPIO_PCLK_DISABLED		0
#define GPIO_PCLK_ENABLED		1

#define GPIO_PIN_RESET			0
#define GPIO_PIN_SET			1

#define GPIO_PIN_0				0
#define GPIO_PIN_1				1
#define GPIO_PIN_2				2
#define GPIO_PIN_3				3
#define GPIO_PIN_4				4
#define GPIO_PIN_5				5
#define GPIO_PIN_6				6
#define GPIO_PIN_7				7
#define GPIO_PIN_8				8
#define GPIO_PIN_9				9
#define GPIO_PIN_10				10
#define GPIO_PIN_11				11
#define GPIO_PIN_12				12
#define GPIO_PIN_13				13
#define GPIO_PIN_14				14
#define GPIO_PIN_15				15

#define GPIO_MODE_INPUT			0
#define GPIO_MODE_OUTPUT		1
#define GPIO_MODE_ALTFUNC		2
#define GPIO_MODE_ANALOG		3
#define GPIO_MODE_IT_RT			4
#define GPIO_MODE_IT_FT			5
#define GPIO_MODE_IT_RTFT		6

#define GPIO_OTYPE_PP			0
#define GPIO_OTYPE_OD			1

#define GPIO_SPEED_SLOW			0
#define GPIO_SPEED_MED			1
#define GPIO_SPEED_HI			2
#define GPIO_SPEED_VHI			3

#define GPIO_PUPD_NONE			0
#define GPIO_PUPD_PU			1
#define GPIO_PUDP_PD			2

#define GPIO_AF_0				0
#define GPIO_AF_1				1
#define GPIO_AF_2				2
#define GPIO_AF_3				3
#define GPIO_AF_4				4
#define GPIO_AF_5				5
#define GPIO_AF_6				6
#define GPIO_AF_7				7
#define GPIO_AF_8				8
#define GPIO_AF_9				9
#define GPIO_AF_10				10
#define GPIO_AF_11				11
#define GPIO_AF_12				12
#define GPIO_AF_13				13
#define GPIO_AF_14				14
#define GPIO_AF_15				15

typedef enum
{
	GPIO_ERROR_NONE,
	GPIO_ERROR_ARG,
	GPIO_ERROR_HANDLE,
	GPIO_ERROR_GPIO,
	GPIO_ERROR_PIN_NUMBER,
	GPIO_ERROR_PIN_MODE,
	GPIO_ERROR_PIN_OTYPE,
	GPIO_ERROR_PIN_SPEED,
	GPIO_ERROR_PIN_PUPD,
	GPIO_ERROR_PIN_ALTFUNC_MODE
}GPIO_Error_t;

typedef struct
{
	uint8_t PinNumber;
	uint8_t PinMode;
	uint8_t PinOType;
	uint8_t PinSpeed;
	uint8_t PinPuPdControl;
	uint8_t PinAltFuncMode;

}GPIO_PinConfig_t;

typedef struct
{
	GPIO_TypeDef *gpio;
	GPIO_PinConfig_t PinConfig;
}GPIO_Handle_t;


GPIO_Error_t GPIO_PeriphClock(GPIO_TypeDef *gpio,uint8_t enable);

GPIO_Error_t GPIO_Init(GPIO_Handle_t *handle);
GPIO_Error_t GPIO_DeInit(GPIO_TypeDef *gpio);

GPIO_Error_t GPIO_WritePort(GPIO_TypeDef *gpio,uint16_t value);
GPIO_Error_t GPIO_ReadPort(GPIO_TypeDef *gpio,uint16_t *value);

GPIO_Error_t GPIO_WritePin(GPIO_TypeDef *gpio,uint8_t pin_number,uint16_t value);
GPIO_Error_t GPIO_ReadPin(GPIO_TypeDef *gpio,uint8_t pin_number,uint8_t *value);
GPIO_Error_t GPIO_TogglePin(GPIO_TypeDef *gpio,uint8_t pin_number);

GPIO_Error_t GPIO_IRQHandling(uint8_t pin_number);
void GPIO_RisingEdge_Callback(uint8_t pin_number);
void GPIO_FallingEdge_Callback(uint8_t pin_number);

#endif /* STM32U031_GPIO_DRIVER_H_ */
