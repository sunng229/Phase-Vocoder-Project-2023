/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : usb_host.c
  * @version         : v1.0_Cube
  * @brief           : This file implements the USB Host
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"

/* USER CODE BEGIN Includes */
#include "ff.h"
FATFS USBH_fatfs;
FIL MyFile;
FRESULT res;
uint32_t bytesWritten;
uint8_t rtext[4096];
uint8_t wtext[] = "USB Host Library : Mass Storage Example";
uint8_t name[10];//name of the file
uint16_t counter=0;
uint32_t i=0;
extern char USBHPath [];  /* USBH logical drive path */
extern int file_ready;
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostFS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

float stof(const char* s){ //code is taken from stackoverflow: https://stackoverflow.com/questions/4392665/converting-string-to-float-without-atof-in-c
  float rez = 0, fact = 1;
  if (*s == '-'){
    s++;
    fact = -1;
  };
  for (int point_seen = 0; *s; s++){
    if (*s == '.'){
      point_seen = 1;
      continue;
    };
    int d = *s - '0';
    if (d >= 0 && d <= 9){
      if (point_seen) fact /= 10.0f;
      rez = rez * 10.0f + (float)d;
    };
  };
  return rez * fact;
}


void userFunction(void) {
	uint16_t bytesread;
	if (Appli_state == APPLICATION_READY) {
			sprintf(name,"demo.txt");
			/*Create a file*/
			res = f_open(&MyFile,name,FA_CREATE_ALWAYS |FA_WRITE);
			if (res != FR_OK ) {
				/* Creation failed */
				Error_Handler();
			} else {
				/*write message to the file. Use variable wtext, bytesWritten*/
				res =f_write(&MyFile,wtext,sizeof(wtext),&bytesWritten);

				/*close the file*/
				f_close(&MyFile);
				/*check number of written bytes*/
				if ((bytesWritten == 0) || (res != FR_OK)) {
					Error_Handler();
				} else{
					// Read the message
					if (f_open(&MyFile,name,FA_READ) != FR_OK ) {
						Error_Handler();
					}else{
						res= f_read(&MyFile,rtext,sizeof(rtext), &bytesread);

						if ((bytesread == 0) || (res != FR_OK)) {
							Error_Handler();
						}
						if (f_close(&MyFile) != FR_OK) {
							Error_Handler();
						}
						while(1);
					}
				}
			}
	}
}

void writeFile(char filename[],  float *vec, int size){

	FIL fp;
	char buff[20];

	if (Appli_state == APPLICATION_READY) {
		if(f_mount(&USBH_fatfs, USBHPath, 1) != FR_OK) {
				 Error_Handler();
		 }

		if (f_open(&fp, filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK ) {
			Error_Handler();
		}
		else {
			for(int i=0; i<size; i++){
				// convert uint16 to string
				//utoa(vec[i], buff, 10); //questionable
				// format write
				//printf("%f\n", vec[i]);
				sprintf(buff,"%f", vec[i]);
				f_printf(&fp, "%s\n", buff);

			}

			f_close(&fp);
		}

		if(f_mount(NULL, USBHPath, 0) != FR_OK){
			  Error_Handler();
		}

	}
}

float * read_txt(uint8_t file_name[], int num_line, int count) { //have another input to tell it where to start reading from
	uint16_t bytesread;
	if (Appli_state == APPLICATION_READY) {
			if (f_open(&MyFile,file_name,FA_READ) != FR_OK ) {
				Error_Handler();
			}else{
				printf("p1\n");
//				FIL* fp;
				int offset;
				offset = 0+(6*count*num_line); //# bytes in a line + 2 bytes for newline = 6 then * num_lines
				f_lseek(&MyFile, offset);
				float * array_out = (float *) malloc(sizeof(float)*num_line);
				for(int i = 0; i < num_line; i++){
					memset(rtext,0,sizeof(rtext));
					f_gets(rtext, sizeof(rtext), &MyFile);
					array_out[i] = atof(rtext);
					int _ = 0;

				}
				if (f_close(&MyFile) != FR_OK) {
					Error_Handler();
				}
				return array_out;
				//return something that tells where it left off
			}
	}
}

void writeFloats (char filename[], char * arr, int num_floats) {

    FIL fp;
    int dummy;

    if (Appli_state == APPLICATION_READY) {

        if(f_mount(&USBH_fatfs, USBHPath, 1) != FR_OK) {
            Error_Handler();
        }

        if (f_open(&fp, filename, FA_OPEN_APPEND | FA_WRITE) != FR_OK ) {
            Error_Handler();
        }
        else {
            if (f_write(&fp, arr, num_floats * sizeof(float), &dummy) != FR_OK) {
                Error_Handler();
            }
            f_close(&fp);
        }

        if(f_mount(NULL, USBHPath, 0) != FR_OK){
              Error_Handler();
        }

    }

}


/* USER CODE END 1 */

/**
  * Init USB host library, add supported class and start the library
  * @retval None
  */
void MX_USB_HOST_Init(void)
{
  /* USER CODE BEGIN USB_HOST_Init_PreTreatment */

  /* USER CODE END USB_HOST_Init_PreTreatment */

  /* Init host Library, add supported class and start the library. */
  if (USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_RegisterClass(&hUsbHostFS, USBH_MSC_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_Start(&hUsbHostFS) != USBH_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */

  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * Background task
 */
void MX_USB_HOST_Process(void)
{
  /* USB Host Background task */
  USBH_Process(&hUsbHostFS);
}
/*
 * user callback definition
 */
static void USBH_UserProcess  (USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
  switch(id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
  break;

  case HOST_USER_DISCONNECTION:
  Appli_state = APPLICATION_DISCONNECT;
  break;

  case HOST_USER_CLASS_ACTIVE:
  Appli_state = APPLICATION_READY;
  break;

  case HOST_USER_CONNECTION:
  Appli_state = APPLICATION_START;
  if (f_mount(&USBH_fatfs,USBHPath,0) != FR_OK)
  {
	  Error_Handler();
  }
  break;

  default:
  break;
  }
  /* USER CODE END CALL_BACK_1 */
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
