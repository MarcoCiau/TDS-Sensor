#include "TDSLib.h"
#include <stdio.h> 
#include <string.h> 

Serial pc(PA_2, PA_3);
AnalogIn anEC(PA_0);

TDSLib::TDSLib(/* args */)
{
    // this->pin = A1;
    this->temperature = 25.0;
    // this->aref = 5.0;
    // this->adcRange = 1024.0;
    // this->kValueAddress = 8;
    this->kValue = 1.00;
    this->debugTDSTimer = 0;
}

TDSLib::~TDSLib()
{

}

void TDSLib::setKValue(float kCell)
{
    this->kValue = kCell;
}

void TDSLib::setTemperature(float temp)
{
    this->temperature = temp;
}

void TDSLib::begin()
{
    // readKValues();
}

void TDSLib::update()
{
    this->voltage = vAVG() / 1000.00;
	this->ecValue=(133.42*this->voltage*this->voltage*this->voltage - 255.86*this->voltage*this->voltage + 857.39*this->voltage)*this->kValue;
	this->ecValue25  =  this->ecValue / (1.0+0.02*(this->temperature-25.0));  //temperature compensation
	this->tdsValue = ecValue25 * (float)TDS_FACTOR;

    if(cmdSerialDataAvailable() > 0)
    {
        calibration(cmdParse());  // if received serial cmd from the serial monitor, enter into the calibration mode
    }

    if (HAL_GetTick() - debugTDSTimer > TDS_DEBUG_SR_MS)
    {
        debugTDSTimer = HAL_GetTick();
        pc.printf("TDS Value : %d ppm \n", (int)getTDSValue());
    }
}

float TDSLib::getKValue()
{
    return this->kValue;
}

float TDSLib::getECValue()
{
    return this->ecValue25;
}

float TDSLib::getTDSValue()
{
    return this->tdsValue;
}

bool TDSLib::cmdSerialDataAvailable()
{
    char cmd;
    static unsigned long cmdRxTimeout = HAL_GetTick();
    while (pc.readable())
    {
        if(HAL_GetTick() - cmdRxTimeout > 500U)
        {
            cmdReceivedBufferIndex = 0;
            memset(cmdReceivedBuffer, 0, (ReceiveBufferLength + 1));
        }
        cmdRxTimeout = HAL_GetTick();
        cmd = pc.getc();

        if (cmd == '\n' || cmdReceivedBufferIndex == ReceiveBufferLength)
        {
            cmdReceivedBufferIndex = 0;
            strupr(cmdReceivedBuffer);
            return true;
        }
        else
        {
            cmdReceivedBuffer[cmdReceivedBufferIndex] = cmd;
            cmdReceivedBufferIndex++;
        }
    }
    return false;
}

uint8_t TDSLib::cmdParse()
{
    uint8_t modeIndex = 0;
    if(strstr(cmdReceivedBuffer, "ENTER") != NULL) modeIndex = 1;
    else if(strstr(cmdReceivedBuffer, "EXIT") != NULL) modeIndex = 3;
    else if(strstr(cmdReceivedBuffer, "CAL:") != NULL) modeIndex = 2;
    return modeIndex;
}


void TDSLib::calibration(uint8_t mode)
{
    char *cmdReceivedBufferPtr;
    static bool ecCalibrationFinish = 0;
    static bool enterCalibrationFlag = 0;
    float KValueTemp,rawECsolution;

    switch(mode)
    {
      case 0:
      if(enterCalibrationFlag)pc.printf("Command Error\n");
      break;
      
      case 1:
      enterCalibrationFlag = 1;
      ecCalibrationFinish = 0;
      pc.printf("\n>>>Enter Calibration Mode<<<\n");
      pc.printf(">>>Please put the probe into the standard buffer solution<<<\n\n");
      break;
     
      case 2:
      cmdReceivedBufferPtr=strstr(cmdReceivedBuffer, "CAL:");
      cmdReceivedBufferPtr+=strlen("CAL:");
      rawECsolution = strtod(cmdReceivedBufferPtr,NULL)/(float)(TDS_FACTOR);
      rawECsolution = rawECsolution*(1.0+0.02*(temperature-25.0));
      if(enterCalibrationFlag)
      {
          KValueTemp = rawECsolution/(133.42*voltage*voltage*voltage - 255.86*voltage*voltage + 857.39*voltage);  //calibrate in the  buffer solution, such as 707ppm(1413us/cm)@25^c
          if((rawECsolution>0) && (rawECsolution<2000) && (KValueTemp>0.25) && (KValueTemp<4.0))
          {
            pc.printf("\n>>>Confrim Successful,K: %d,Send EXIT to Save and Exit<<<\n", (int)(KValueTemp * 100.00));
            kValue =  KValueTemp;
            ecCalibrationFinish = 1;
          }
          else{
            pc.printf("\n>>>Confirm Failed,Try Again<<<\n");
            ecCalibrationFinish = 0;
          }        
      }
      break;

        case 3:
        if(enterCalibrationFlag)
        {
            if(ecCalibrationFinish)
            {
            //    EEPROM_write(kValueAddress, kValue);
               pc.printf("\n>>>Calibration Successful,K Value Saved");
            }
            else pc.printf(">>>Calibration Failed");       
            pc.printf("\n,Exit Calibration Mode<<<\n");
            ecCalibrationFinish = 0;
            enterCalibrationFlag = 0;
        }
        break;
    }
}

float TDSLib::vAVG()
{
    uint32_t sumV = 0;
    for (uint8_t i = 0; i < 100; i++)
    {
        sumV += anEC.read() * 3300;
    }
    float avgV = 0;
    avgV = (float)(sumV / 100.00);
    return avgV;
}
TDSLib probeTest;
