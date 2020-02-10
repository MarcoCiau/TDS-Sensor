#include <mbed.h>
#include <TDSLib.h>
TDSLib probeTest(PA_0);
int main() {

  // put your setup code here, to run once:
  probeTest.begin();
  probeTest.setKValue(1);

  while(1) {
    // put your main code here, to run repeatedly:
    probeTest.setTemperature(26.00);
    probeTest.update();
  }
}