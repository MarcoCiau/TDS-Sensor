#ifndef TDS_LIB_H
#define TDS_LIB_H

#include "mbed.h"

#define ReceiveBufferLength 15
#define TDS_FACTOR 0.5 /* TDS = Ec / 2 */
#define TDS_DEBUG_SR_MS 1000
class TDSLib
{
private:
    /* data */
    uint32_t debugTDSTimer;
    float temperature;
    char cmdReceivedBuffer[ReceiveBufferLength+1];   // store the serial cmd from the serial monitor
    uint8_t cmdReceivedBufferIndex;
    float kValue;      // k value of the probe,you can calibrate in buffer solution ,such as 706.5ppm(1413us/cm)@25^C 
    float analogValue;
    float voltage;
    float ecValue;//non-temperature compensation EC value
    float ecValue25;//temperature compensation EC value
    float tdsValue;

    void readKValues();
    bool cmdSerialDataAvailable();
    uint8_t cmdParse();
    void calibration(uint8_t mode);

    float vAVG();
public:
    TDSLib(/* args */);
    ~TDSLib();
    void begin();
    void update();
    void setTemperature(float temp);
    void setKValue(float kCell);
    float getKValue();
    float getTDSValue();
    float getECValue();
};

extern TDSLib probeTest;
#endif