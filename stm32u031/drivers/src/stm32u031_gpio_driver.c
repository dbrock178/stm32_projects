#include "stm32u031_gpio_driver.h"
#include "stm32u031xx.h"

#define GPIO_PIN_MAX							15
#define GPIO_MODE_MAX							GPIO_MODE_IT_RTFT
#define GPIO_OTYPE_MAX							GPIO_OTYPE_OD
#define GPIO_SPEED_MAX							GPIO_SPEED_VHI
#define GPIO_PUPD_MAX							GPIO_PUDP_PD
#define GPIO_AF_MAX								GPIO_AF_15

#define IS_GPIO_PCLK_ENABLE_VALID(ENABLE)		((ENABLE == GPIO_PCLK_ENABLED) || \
										 	 	 (ENABLE == GPIO_PCLK_DISABLED))

#define IS_GPIO_PIN_VALID(PIN)					((PIN<=GPIO_PIN_MAX))
#define IS_GPIO_PIN_VALUE_VALID(VALUE)			((VALUE==GPIO_PIN_RESET) || (VALUE==GPIO_PIN_SET))
#define IS_GPIO_MODE_VALID(MODE)				((MODE<=GPIO_MODE_MAX))
#define IS_GPIO_OTYPE_VALID(OTYPE)				((OTYPE<=GPIO_OTYPE_MAX))
#define IS_GPIO_SPEED_VALID(SPEED)				((SPEED<=GPIO_SPEED_MAX))
#define IS_GPIO_PUPD_VALID(PUPD)				((PUPD<=GPIO_PUPD_MAX))
#define IS_GPIO_AF_VALID(AF)					((AF<=GPIO_AF_MAX))

#define REG_WIDTH_2								2
#define REG_WIDTH_4								4
#define REG_WIDTH_8								8
#define REG_WIDTH_16							16

static void GPIO_SetMode(GPIO_Handle_t *handle);
static void GPIO_EnableClock(GPIO_Handle_t *handle);

GPIO_Error_t GPIO_PeriphClock(GPIO_TypeDef *gpio,uint8_t enable)
{
	if(!IS_GPIO_ALL_INSTANCE(gpio) || !IS_GPIO_PCLK_ENABLE_VALID(enable)) { return GPIO_ERROR_ARG; }

	if(enable)
	{
		if(gpio == GPIOA)
		{
			RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
		}
		else if(gpio == GPIOB)
		{
			RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
		}
		else if(gpio == GPIOC)
		{
			RCC->IOPENR |= RCC_IOPENR_GPIOCEN;
		}
		else if(gpio == GPIOD)
		{
			RCC->IOPENR |= RCC_IOPENR_GPIODEN;
		}
		else if(gpio == GPIOF)
		{
			RCC->IOPENR |= RCC_IOPENR_GPIOFEN;
		}
	}
	else
	{
		if(gpio == GPIOA)
		{
			RCC->IOPENR &= ~RCC_IOPENR_GPIOAEN;
		}
		else if(gpio == GPIOB)
		{
			RCC->IOPENR &= ~RCC_IOPENR_GPIOBEN;
		}
		else if(gpio == GPIOC)
		{
			RCC->IOPENR &= ~RCC_IOPENR_GPIOCEN;
		}
		else if(gpio == GPIOD)
		{
			RCC->IOPENR &= ~RCC_IOPENR_GPIODEN;
		}
		else if(gpio == GPIOF)
		{
			RCC->IOPENR &= ~RCC_IOPENR_GPIOFEN;
		}
	}

	return GPIO_ERROR_NONE;
}

GPIO_Error_t GPIO_Init(GPIO_Handle_t *handle)
{
	if(handle == 0) { return GPIO_ERROR_HANDLE; }

	if(!IS_GPIO_ALL_INSTANCE(handle->gpio)) { return GPIO_ERROR_GPIO; }

	if(!IS_GPIO_PIN_VALID(handle->PinConfig.PinNumber)) { return GPIO_ERROR_PIN_NUMBER; }

	if(!IS_GPIO_MODE_VALID(handle->PinConfig.PinMode)) { return GPIO_ERROR_PIN_MODE; }

	if(!IS_GPIO_OTYPE_VALID(handle->PinConfig.PinOType)) { return GPIO_ERROR_PIN_OTYPE; }

	if(!IS_GPIO_SPEED_VALID(handle->PinConfig.PinSpeed)) { return GPIO_ERROR_PIN_SPEED; }

	if(!IS_GPIO_PUPD_VALID(handle->PinConfig.PinPuPdControl)) { return GPIO_ERROR_PIN_PUPD; }

	if(!IS_GPIO_AF_VALID((handle->PinConfig.PinAltFuncMode))) { return GPIO_ERROR_PIN_ALTFUNC_MODE; }

	GPIO_EnableClock(handle);
	GPIO_SetMode(handle);

	// Set Output Type
	handle->gpio->OTYPER &= ~(1<<handle->PinConfig.PinNumber);
	if (handle->PinConfig.PinOType)
	{
		handle->gpio->OTYPER |= (1<<handle->PinConfig.PinNumber);
	}

	// Set Speed
	handle->gpio->OSPEEDR &= ~(0x3<<(handle->PinConfig.PinNumber * REG_WIDTH_2));
	handle->gpio->OSPEEDR |= (handle->PinConfig.PinSpeed<<(handle->PinConfig.PinNumber * REG_WIDTH_2));

	// Set pullup
	handle->gpio->PUPDR &= ~(0x3<<(handle->PinConfig.PinNumber * REG_WIDTH_2));
	handle->gpio->PUPDR |= (handle->PinConfig.PinPuPdControl<<(handle->PinConfig.PinNumber * REG_WIDTH_2));

	return GPIO_ERROR_NONE;
}

GPIO_Error_t GPIO_DeInit(GPIO_TypeDef *gpio)
{
	if(!IS_GPIO_ALL_INSTANCE(gpio)) { return GPIO_ERROR_ARG; }

	if(gpio == GPIOA)
	{
		RCC->IOPRSTR |= RCC_IOPRSTR_GPIOARST;
		RCC->IOPRSTR &= ~RCC_IOPRSTR_GPIOARST;
	}
	else if(gpio == GPIOB)
	{
		RCC->IOPRSTR |= RCC_IOPRSTR_GPIOBRST;
		RCC->IOPRSTR &= ~RCC_IOPRSTR_GPIOBRST;
	}
	else if(gpio == GPIOC)
	{
		RCC->IOPRSTR |= RCC_IOPRSTR_GPIOCRST;
		RCC->IOPRSTR &= ~RCC_IOPRSTR_GPIOCRST;
	}
	else if(gpio == GPIOD)
	{
		RCC->IOPRSTR |= RCC_IOPRSTR_GPIODRST;
	}
	else if(gpio == GPIOF)
	{
		RCC->IOPRSTR |= RCC_IOPRSTR_GPIOFRST;
	}

	return GPIO_ERROR_NONE;
}

GPIO_Error_t GPIO_WritePort(GPIO_TypeDef *gpio,uint16_t value)
{
	if(!IS_GPIO_ALL_INSTANCE(gpio)) { return GPIO_ERROR_ARG; }

	gpio->ODR = value;

	return GPIO_ERROR_NONE;
}

GPIO_Error_t GPIO_ReadPort(GPIO_TypeDef *gpio,uint16_t *value)
{
	if(!IS_GPIO_ALL_INSTANCE(gpio)) { return GPIO_ERROR_ARG; }

	*value = gpio->IDR;

	return GPIO_ERROR_NONE;
}

GPIO_Error_t GPIO_WritePin(GPIO_TypeDef *gpio,uint8_t pin_number,uint16_t value)
{
	if(!IS_GPIO_ALL_INSTANCE(gpio) || !IS_GPIO_PIN_VALID(pin_number) || \
	   !IS_GPIO_PIN_VALUE_VALID(value))
	{
		return GPIO_ERROR_ARG;
	}

	if(value)
	{
		gpio->BSRR |= (1<<pin_number);
	}
	else
	{
		gpio->BSRR |= (1<<(pin_number+REG_WIDTH_16));
	}

	return GPIO_ERROR_NONE;
}

GPIO_Error_t GPIO_ReadPin(GPIO_TypeDef *gpio,uint8_t pin_number,uint8_t *value)
{
	if(!IS_GPIO_ALL_INSTANCE(gpio) || !IS_GPIO_PIN_VALID(pin_number))
	{
		return GPIO_ERROR_ARG;
	}

	*value= ((gpio->IDR & (1<<pin_number)) ? GPIO_PIN_SET : GPIO_PIN_RESET);

	return GPIO_ERROR_NONE;
}

GPIO_Error_t GPIO_TogglePin(GPIO_TypeDef *gpio,uint8_t pin_number)
{
	if(!IS_GPIO_ALL_INSTANCE(gpio) || !IS_GPIO_PIN_VALID(pin_number))
	{
		return GPIO_ERROR_ARG;
	}

	gpio->ODR ^= (1<<pin_number);

	return GPIO_ERROR_NONE;
}

static void GPIO_SetMode(GPIO_Handle_t *handle)
{
	// Clear selected MODER register
	handle->gpio->MODER &= ~(3<<(handle->PinConfig.PinNumber * REG_WIDTH_2));

	if((handle->PinConfig.PinMode == GPIO_MODE_INPUT) || \
	   (handle->PinConfig.PinMode == GPIO_MODE_OUTPUT) || \
	   (handle->PinConfig.PinMode == GPIO_MODE_ANALOG))
	{
		handle->gpio->MODER |= (handle->PinConfig.PinMode<<(handle->PinConfig.PinNumber * REG_WIDTH_2));

		return;
	}

	if(handle->PinConfig.PinMode == GPIO_MODE_ALTFUNC)
	{
		uint8_t afrIndex = handle->PinConfig.PinNumber / 8;
		uint8_t afrPos = handle->PinConfig.PinNumber % 8;

		// Clear AltFunc Mode
		handle->gpio->AFR[afrIndex] &= ~(0xF<<(afrPos * REG_WIDTH_4));

		// Set AltFunc Mode
		handle->gpio->AFR[afrIndex] |= (handle->PinConfig.PinAltFuncMode<<(afrPos * REG_WIDTH_4));

		// Set Pin as AltFunc
		handle->gpio->MODER |= (handle->PinConfig.PinMode<<(handle->PinConfig.PinNumber * REG_WIDTH_2));

		return;
	}

	// Enable SYSCFG Periph clk
	RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;

	if(handle->PinConfig.PinMode == GPIO_MODE_IT_RT)
	{
		// Disable falling edge interrupt trigger
		EXTI->FTSR1 &= ~(1<<handle->PinConfig.PinNumber);

		// Enable rising edge interrupt trigger
		EXTI->RTSR1 |= (1<<handle->PinConfig.PinNumber);

	}
	else if(handle->PinConfig.PinMode == GPIO_MODE_IT_FT)
	{
		// Disable rising edge interrupt trigger
		EXTI->RTSR1 &= ~(1<<handle->PinConfig.PinNumber);

		// Enable falling edge interrupt trigger
		EXTI->FTSR1 |= (1<<handle->PinConfig.PinNumber);
	}
	else if(handle->PinConfig.PinMode == GPIO_MODE_IT_RTFT)
	{
		// Enable rising edge interrupt trigger
		EXTI->RTSR1 |= (1<<handle->PinConfig.PinNumber);

		// Enable falling edge interrupt trigger
		EXTI->FTSR1 |= (1<<handle->PinConfig.PinNumber);
	}

	uint8_t extiIndex = handle->PinConfig.PinNumber / 4;
	uint8_t extiPos = handle->PinConfig.PinNumber % 4;

	// Clear
	EXTI->EXTICR[extiIndex] &= ~(0xFF<<(extiPos * REG_WIDTH_8));

	if(handle->gpio == GPIOB)
	{
		EXTI->EXTICR[extiIndex] |= (0x1<<(extiPos * REG_WIDTH_8));
	}
	else if(handle->gpio == GPIOC)
	{
		EXTI->EXTICR[extiIndex] |= (0x2<<(extiPos * REG_WIDTH_8));
	}
	else if(handle->gpio == GPIOD)
	{
		EXTI->EXTICR[extiIndex] |= (0x3<<(extiPos * REG_WIDTH_8));
	}
	else if(handle->gpio == GPIOF)
	{
		EXTI->EXTICR[extiIndex] |= (0x5<<(extiPos * REG_WIDTH_8));
	}

	// Unmask interrupt
	EXTI->IMR1 |= (1<<handle->PinConfig.PinNumber);
}

static void GPIO_EnableClock(GPIO_Handle_t *handle)
{
	if(handle->gpio==GPIOA)
	{
		RCC->IOPENR|=RCC_IOPENR_GPIOAEN;
	}
	if(handle->gpio==GPIOB)
	{
		RCC->IOPENR|=RCC_IOPENR_GPIOBEN;
	}
	if(handle->gpio==GPIOC)
	{
		RCC->IOPENR|=RCC_IOPENR_GPIOCEN;
	}
	if(handle->gpio==GPIOD)
	{
		RCC->IOPENR|=RCC_IOPENR_GPIODEN;
	}
	if(handle->gpio==GPIOF)
	{
		RCC->IOPENR|=RCC_IOPENR_GPIOFEN;
	}
}

GPIO_Error_t GPIO_IRQHandling(uint8_t pin_number)
{
	if(!IS_GPIO_PIN_VALID(pin_number)) { return GPIO_ERROR_PIN_NUMBER; }

	// See if it is RT or FT that caused interrupt
	if(EXTI->RPR1 & (1<<pin_number))
	{
		// Clear pending register
		EXTI->RPR1 |= (1<<pin_number);

		// Callback
		GPIO_RisingEdge_Callback(pin_number);
	}

	if(EXTI->FPR1 & (1<<pin_number))
	{
		// Clear pending register
		EXTI->FPR1 |= (1<<pin_number);

		// Callback
		GPIO_FallingEdge_Callback(pin_number);
	}

	return GPIO_ERROR_NONE;
}
__attribute__((weak)) void GPIO_RisingEdge_Callback(uint8_t pin_number)
{

}

__attribute__((weak)) void GPIO_FallingEdge_Callback(uint8_t pin_number)
{

}

