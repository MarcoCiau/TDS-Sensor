#ifndef TDS_LIB_H
#define TDS_LIB_H
#include "mbed.h"

#define BUFFER_LENGTH 15
#define TDS_FACTOR 0.5 /* TDS = Ec / 2 */
#define TDS_DEBUG_SR_MS 1000

class TDSLib
{
private:
    AnalogIn _adcPin;
    uint32_t debugTDSTimer;
    float temperature; /*Water Temperature*/
    char cmdReceivedBuffer[BUFFER_LENGTH+1];/*Serial Buffer*/
    uint8_t rxBufferIndex;
    float kValue;    /* K-Cell constant value of the probe. k = 1 (default) */
    float voltage;   /* Raw ADC To Voltage Conversion*/
    float ecValue;   /* non-temperature compensation EC value */
    float ecValue25; /* temperature compensation EC value */
    float tdsValue;  /* TDS Value based on 0.5 TDS Factor*/

    bool SerialDataAvailable();
    uint8_t payloadParser();
    void calibration(uint8_t mode); /* TDS Probe Calibration */
    float vAVG();/* Averaging ADC Readings */
public:
    TDSLib(PinName pin);
    ~TDSLib();
    void begin();
    void update();
    /*Set Water Temperature for EC compensation*/
    void setTemperature(float temp);
    /*Set K-Cell Constant of probe*/
    void setKValue(float kCell);
    
    float getKValue();
    float getTDSValue();
    float getECValue();
};

#endif