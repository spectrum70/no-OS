/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for aducm3029 platform of eval-adxl355-pmdz project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "common_data.h"

#ifdef DUMMY_EXAMPLE
#include "dummy_example.h"
#endif

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_trigger_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for ADUCM3029 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;
	adxl355_ip.comm_init.spi_init = adxl355_spi_ip;

	ret = platform_init();
	if (ret)
		goto error;

#ifdef DUMMY_EXAMPLE
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adxl355_uart_ip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart_desc);

	ret = dummy_example_main();
	if (ret)
		goto error_uart;
error_uart:
	return no_os_uart_remove(uart_desc);
#endif

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
	if (ret)
		goto error;
#endif

#ifdef IIO_TRIGGER_EXAMPLE

	struct no_os_gpio_desc *adxl355_gpio_desc;

	/* Initialize DATA READY pin */
	ret = no_os_gpio_get_optional(&adxl355_gpio_desc, &adxl355_gpio_drdy_ip);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_input(adxl355_gpio_desc);
	if (ret)
		goto error_gpio;

	ret = iio_trigger_example_main();
	if (ret)
		goto error_gpio;

error_gpio:
	return no_os_gpio_remove(adxl355_gpio_desc);
#endif

#if (DUMMY_EXAMPLE + IIO_EXAMPLE + IIO_TRIGGER_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (DUMMY_EXAMPLE + IIO_EXAMPLE + IIO_TRIGGER_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

error:
	return ret;
}
