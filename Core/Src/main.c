/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ai_datatypes_defines.h"
#include "ai_platform.h"
#include "sinewave_model.h"
#include "sinewave_model_data.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

DMA2D_HandleTypeDef hdma2d;

I2C_HandleTypeDef hi2c3;

LTDC_HandleTypeDef hltdc;

SPI_HandleTypeDef hspi5;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart1;

HCD_HandleTypeDef hhcd_USB_OTG_HS;

SDRAM_HandleTypeDef hsdram1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
static void MX_DMA2D_Init(void);
static void MX_FMC_Init(void);
static void MX_I2C3_Init(void);
static void MX_LTDC_Init(void);
static void MX_SPI5_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM7_Init(void);
static void MX_USB_OTG_HS_HCD_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

	 /* USER CODE BEGIN 1 */
	  char buf[50];
	  int buf_len = 0;
	  ai_error ai_err;
	  ai_i32 nbatch;
	  uint32_t timestamp;
	  float y_val;

	  // Chunk of memory used to hold intermediate values for neural network
	  AI_ALIGNED(4) ai_u8 activations[AI_SINEWAVE_MODEL_DATA_ACTIVATIONS_SIZE];

	  // Buffers used to store input and output tensors
	  AI_ALIGNED(4) ai_i8 in_data[AI_SINEWAVE_MODEL_IN_1_SIZE_BYTES];
	  AI_ALIGNED(4) ai_i8 out_data[AI_SINEWAVE_MODEL_OUT_1_SIZE_BYTES];

	  // Pointer to our model
	  ai_handle sine_model = AI_HANDLE_NULL;

	  // Initialize wrapper structs that hold pointers to data and info about the
	  // data (tensor height, width, channels)
      ai_buffer ai_input[AI_SINEWAVE_MODEL_IN_NUM] = {
        {
          .format = AI_SINEWAVE_MODEL_IN_1_FORMAT,
          .data = AI_HANDLE_PTR(in_data),
          .meta_info = NULL,
          .flags = AI_FLAG_NONE,
          .size = AI_SINEWAVE_MODEL_IN_1_SIZE,
          .shape = { .type = 0, .size = 4, .data = (ai_shape_dimension[]){1, AI_SINEWAVE_MODEL_IN_1_CHANNEL, 1, 1} }
        }
      };
      ai_buffer ai_output[AI_SINEWAVE_MODEL_OUT_NUM] = {
        {
          .format = AI_SINEWAVE_MODEL_OUT_1_FORMAT,
          .data = AI_HANDLE_PTR(out_data),
          .meta_info = NULL,
          .flags = AI_FLAG_NONE,
          .size = AI_SINEWAVE_MODEL_OUT_1_SIZE,
          .shape = { .type = 0, .size = 4, .data = (ai_shape_dimension[]){1, AI_SINEWAVE_MODEL_OUT_1_CHANNEL, 1, 1} }
        }
      };

	  // Set working memory and get weights/biases from model
      ai_network_params ai_params = {
        AI_SINEWAVE_MODEL_DATA_WEIGHTS(ai_sinewave_model_data_weights_get()),
        AI_SINEWAVE_MODEL_DATA_ACTIVATIONS(activations)
      };

	  // Set pointers wrapper structs to our data buffers
    // ai_buffer struct does not have n_batches in this version, so skip those assignments
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_I2C3_Init();
  MX_LTDC_Init();
  MX_SPI5_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM7_Init();
  MX_USB_OTG_HS_HCD_Init();
  /* USER CODE BEGIN 2 */
  // Start timer/counter
    HAL_TIM_Base_Start(&htim7);

    // Greetings!
    buf_len = sprintf(buf, "\r\n\r\nSTM32 X-Cube-AI test\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);

    // Create instance of neural network
    ai_err = ai_sinewave_model_create(&sine_model, AI_SINEWAVE_MODEL_DATA_CONFIG);
    if (ai_err.type != AI_ERROR_NONE)
    {
      buf_len = sprintf(buf, "Error: could not create NN instance\r\n");
      HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
      while(1);
    }

    // Initialize neural network
    if (!ai_sinewave_model_init(sine_model, &ai_params))
    {
      buf_len = sprintf(buf, "Error: could not initialize NN\r\n");
      HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
      while(1);
    }


    /* USER CODE END 2 */


  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
        // Test case selection
        int test_case_id = 7; // 1: sine sweep, 2: edge/out-of-range, 3: negative values only, 4: positive values only, 5: random noise test

        switch (test_case_id) {
                        case 7:
                // Test Case 7: 100 Hz Frequency Test - Generate and send 100 Hz sine wave
                {
                  float freq = 100.0f; // Target frequency: 100 Hz
                  float sample_rate = 1000.0f; // Samples per second
                  uint32_t num_samples = 100; // Number of samples (0.1 second of data)
                  
                  for (uint32_t i = 0; i < num_samples; i++) {
                    float t = (float)i / sample_rate; // Time in seconds
                    float input_val = sinf(2.0f * 3.1415926f * freq * t); // 100 Hz sine wave
                    ((ai_float *)in_data)[0] = input_val;
                    timestamp = htim7.Instance->CNT;
                    nbatch = ai_sinewave_model_run(sine_model, &ai_input[0], &ai_output[0]);
                    if (nbatch != 1) {
                      buf_len = sprintf(buf, "Error: could not run inference\r\n");
                      HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                    }
                    y_val = ((float *)out_data)[0];
                    buf_len = sprintf(buf,
                              "TestCase7: %f | Output: %f | Duration: %lu\r\n",
                              ((ai_float *)in_data)[0],
                              y_val,
                              htim7.Instance->CNT - timestamp);
                    HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                    HAL_Delay(50); // Reliable UART transmission with proper delay
                  }
                }
                break;
            case 6:
                // Test Case 6: Large Amplitude Noise Test
                for (uint32_t i = 0; i < 100; i++) {
                  float base_value = 1.0f; // You can change this base value
                  float noise = ((float)rand() / (float)RAND_MAX) * 1.0f - 0.5f; // Noise in [-0.5, 0.5]
                  float noisy_input = base_value + noise;
                  ((ai_float *)in_data)[0] = noisy_input;
                  timestamp = htim7.Instance->CNT;
                  nbatch = ai_sinewave_model_run(sine_model, &ai_input[0], &ai_output[0]);
                  if (nbatch != 1) {
                    buf_len = sprintf(buf, "Error: could not run inference\r\n");
                    HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                  }
                  y_val = ((float *)out_data)[0];
                  float accuracy = 1.0f - fabsf(y_val - noisy_input); // accuracy as 1 - abs(error)
                  float prediction_percentage = (noisy_input != 0.0f) ? (y_val / noisy_input) * 100.0f : 0.0f;
                  buf_len = sprintf(buf,
                            "TestCase6: %f | Output: %f | Duration: %lu | Accuracy: %f | Prediction%%: %f\r\n",
                            ((ai_float *)in_data)[0],
                            y_val,
                            htim7.Instance->CNT - timestamp,
                            accuracy,
                            prediction_percentage);
                  HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                  HAL_Delay(50);
                }
                break;
            case 5:
                // Test Case 5: Random Noise Test
                for (uint32_t i = 0; i < 100; i++) {
                  float base_value = 1.0f; // You can change this base value
                  float noise = ((float)rand() / (float)RAND_MAX) * 0.2f - 0.1f; // Noise in [-0.1, 0.1]
                  float noisy_input = base_value + noise;
                  ((ai_float *)in_data)[0] = noisy_input;
                  timestamp = htim7.Instance->CNT;
                  nbatch = ai_sinewave_model_run(sine_model, &ai_input[0], &ai_output[0]);
                  if (nbatch != 1) {
                    buf_len = sprintf(buf, "Error: could not run inference\r\n");
                    HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                  }
                  y_val = ((float *)out_data)[0];
                  buf_len = sprintf(buf,
                            "TestCase5: %f | Output: %f | Duration: %lu\r\n",
                            ((ai_float *)in_data)[0],
                            y_val,
                            htim7.Instance->CNT - timestamp);
                  HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                  HAL_Delay(50);
                }
                break;
            case 4:
              // Test Case 4: Smooth positive sinewave sweep (only positive values)
              for (uint32_t i = 0; i < 100; i++) {
                float x = ((float)i / 100.0f) * 2.0f * 3.1415926f;
                float val = sinf(x);
                if (val < 0) val = 0; // Only positive part
                ((ai_float *)in_data)[0] = val;
                timestamp = htim7.Instance->CNT;
                nbatch = ai_sinewave_model_run(sine_model, &ai_input[0], &ai_output[0]);
                if (nbatch != 1) {
                  buf_len = sprintf(buf, "Error: could not run inference\r\n");
                  HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                }
                y_val = ((float *)out_data)[0];
                buf_len = sprintf(buf,
                          "TestCase4: %f | Output: %f | Duration: %lu\r\n",
                          ((ai_float *)in_data)[0],
                          y_val,
                          htim7.Instance->CNT - timestamp);
                HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                HAL_Delay(50);
              }
              break;
                  case 3:
                    // Test Case 3: Only negative values
                    {
                      float test_cases[] = {-0.1f, -1.0f, -3.1415926f/2, -3.1415926f, -10.0f, -100.0f, -1000.0f};
                      int num_cases = sizeof(test_cases)/sizeof(test_cases[0]);
                      for (int i = 0; i < num_cases; i++) {
                        ((ai_float *)in_data)[0] = test_cases[i];
                        timestamp = htim7.Instance->CNT;
                        nbatch = ai_sinewave_model_run(sine_model, &ai_input[0], &ai_output[0]);
                        if (nbatch != 1) {
                          buf_len = sprintf(buf, "Error: could not run inference\r\n");
                          HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                        }
                        y_val = ((float *)out_data)[0];
                        buf_len = sprintf(buf,
                                  "TestCase3: %f | Output: %f | Duration: %lu\r\n",
                                  ((ai_float *)in_data)[0],
                                  y_val,
                                  htim7.Instance->CNT - timestamp);
                        HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                        HAL_Delay(100);
                      }
                    }
                    break;
            case 1:
                // Test Case 1: Sweep sinewave from 0 to 2π
                for (uint32_t i = 0; i < 100; i++) {
                    float x = ((float)i / 100.0f) * 2.0f * 3.1415926f;
                    ((ai_float *)in_data)[0] = sinf(x);
                    timestamp = htim7.Instance->CNT;
                    nbatch = ai_sinewave_model_run(sine_model, &ai_input[0], &ai_output[0]);
                    if (nbatch != 1) {
                        buf_len = sprintf(buf, "Error: could not run inference\r\n");
                        HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                    }
                    y_val = ((float *)out_data)[0];
                    buf_len = sprintf(buf,
                                      "TestCase1: %f | Output: %f | Duration: %lu\r\n",
                                      ((ai_float *)in_data)[0],
                                      y_val,
                                      htim7.Instance->CNT - timestamp);
                    HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                    HAL_Delay(50);
                }
                break;
            case 2:
                // Test Case 2: Edge and out-of-range values
                {
                    float test_cases[] = {0.0f, 1.0f, 3.1415926f/2, 3.1415926f, 3.0f*3.1415926f/2, 2.0f*3.1415926f, -1.0f, 10.0f, -3.1415926f, 100.0f};
                    int num_cases = sizeof(test_cases)/sizeof(test_cases[0]);
                    for (int i = 0; i < num_cases; i++) {
                        ((ai_float *)in_data)[0] = test_cases[i];
                        timestamp = htim7.Instance->CNT;
                        nbatch = ai_sinewave_model_run(sine_model, &ai_input[0], &ai_output[0]);
                        if (nbatch != 1) {
                            buf_len = sprintf(buf, "Error: could not run inference\r\n");
                            HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                        }
                        y_val = ((float *)out_data)[0];
                        buf_len = sprintf(buf,
                                          "TestCase2: %f | Output: %f | Duration: %lu\r\n",
                                          ((ai_float *)in_data)[0],
                                          y_val,
                                          htim7.Instance->CNT - timestamp);
                        HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                        HAL_Delay(100);
                    }
                }
                break;
            default:
                buf_len = sprintf(buf, "No valid test case selected\r\n");
                HAL_UART_Transmit(&huart1, (uint8_t *)buf, buf_len, 100);
                break;
        }
        HAL_Delay(1000); // Wait before next round
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

    // AI buffer not needed here, remove leftover line

}


/**
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{
  /* USER CODE BEGIN DMA2D_Init 1 */
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */
}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 9;
  hltdc.Init.VerticalSync = 1;
  hltdc.Init.AccumulatedHBP = 29;
  hltdc.Init.AccumulatedVBP = 3;
  hltdc.Init.AccumulatedActiveW = 269;
  hltdc.Init.AccumulatedActiveH = 323;
  hltdc.Init.TotalWidth = 279;
  hltdc.Init.TotalHeigh = 327;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 240;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 320;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg.FBStartAdress = 0xD0000000;
  pLayerCfg.ImageWidth = 240;
  pLayerCfg.ImageHeight = 320;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief SPI5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI5_Init(void)
{

  /* USER CODE BEGIN SPI5_Init 0 */

  /* USER CODE END SPI5_Init 0 */

  /* USER CODE BEGIN SPI5_Init 1 */

  /* USER CODE END SPI5_Init 1 */
  /* SPI5 parameter configuration*/
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI5_Init 2 */

  /* USER CODE END SPI5_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 80-1;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 65536-1;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USB_OTG_HS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_HS_HCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_HS_Init 0 */

  /* USER CODE END USB_OTG_HS_Init 0 */

  /* USER CODE BEGIN USB_OTG_HS_Init 1 */

  /* USER CODE END USB_OTG_HS_Init 1 */
  hhcd_USB_OTG_HS.Instance = USB_OTG_HS;
  hhcd_USB_OTG_HS.Init.Host_channels = 12;
  hhcd_USB_OTG_HS.Init.speed = HCD_SPEED_FULL;
  hhcd_USB_OTG_HS.Init.dma_enable = DISABLE;
  hhcd_USB_OTG_HS.Init.phy_itface = USB_OTG_EMBEDDED_PHY;
  hhcd_USB_OTG_HS.Init.Sof_enable = DISABLE;
  hhcd_USB_OTG_HS.Init.low_power_enable = DISABLE;
  hhcd_USB_OTG_HS.Init.vbus_sensing_enable = DISABLE;
  hhcd_USB_OTG_HS.Init.use_external_vbus = DISABLE;
  if (HAL_HCD_Init(&hhcd_USB_OTG_HS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_HS_Init 2 */

  /* USER CODE END USB_OTG_HS_Init 2 */

}

/* FMC initialization function */
static void MX_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK2;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_DISABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 7;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 7;
  SdramTiming.WriteRecoveryTime = 3;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */

  /* USER CODE END FMC_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, NCS_MEMS_SPI_Pin|CSX_Pin|OTG_FS_PSO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ACP_RST_GPIO_Port, ACP_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, RDX_Pin|WRX_DCX_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, LD3_Pin|LD4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : NCS_MEMS_SPI_Pin CSX_Pin OTG_FS_PSO_Pin */
  GPIO_InitStruct.Pin = NCS_MEMS_SPI_Pin|CSX_Pin|OTG_FS_PSO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : B1_Pin MEMS_INT1_Pin MEMS_INT2_Pin TP_INT1_Pin */
  GPIO_InitStruct.Pin = B1_Pin|MEMS_INT1_Pin|MEMS_INT2_Pin|TP_INT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ACP_RST_Pin */
  GPIO_InitStruct.Pin = ACP_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ACP_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OC_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TE_Pin */
  GPIO_InitStruct.Pin = TE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RDX_Pin WRX_DCX_Pin */
  GPIO_InitStruct.Pin = RDX_Pin|WRX_DCX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin LD4_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

