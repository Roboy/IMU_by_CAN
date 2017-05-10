#include <stdio.h>
#include <stdint.h> // defines uintN_t types
#include <inttypes.h> // defines PRIx macros
#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))
#include "CurieIMU.h"
#include <mcp_can.h>
#include <SPI.h>

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);  


uint64_t pack754(long double f, unsigned bits, unsigned expbits)
{
    long double fnorm;
    int shift;
    long long sign, exp, significand;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

    if (f == 0.0) return 0; // get this special case out of the way

    // check sign and begin normalization
    if (f < 0) { sign = 1; fnorm = -f; }
    else { sign = 0; fnorm = f; }

    // get the normalized form of f and track the exponent
    shift = 0;
    while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
    while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
    fnorm = fnorm - 1.0;

    // calculate the binary form (non-float) of the significand data
    significand = fnorm * ((1LL<<significandbits) + 0.5f);

    // get the biased exponent
    exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

    // return the final answer
    return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

long double unpack754(uint64_t i, unsigned bits, unsigned expbits)
{
    long double result;
    long long shift;
    unsigned bias;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

    if (i == 0) return 0.0;

    // pull the significand
    result = (i&((1LL<<significandbits)-1)); // mask
    result /= (1LL<<significandbits); // convert back to float
    result += 1.0f; // add the one back on

    // deal with the exponent
    bias = (1<<(expbits-1)) - 1;
    shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
    while(shift > 0) { result *= 2.0; shift--; }
    while(shift < 0) { result /= 2.0; shift++; }

    // sign it
    result *= (i>>(bits-1))&1? -1.0: 1.0;

    return result;
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");
  
  while (!Serial);    // wait for the serial port to open

  // initialize device
  Serial.println("Initializing IMU device...");
  CurieIMU.begin();

  // Set the accelerometer range to 250 degrees/second
  CurieIMU.setGyroRange(250);

  // Set the accelerometer range to 2G
  CurieIMU.setAccelerometerRange(2);
}

 float gx, gy, gz; //scaled Gyro values
 float gx1, gy1, gz1;
 uint32_t gxi, gyi, gzi;
 float ax, ay, az; //scaled acceleration values
 float ax1, ay1, az1;
 uint32_t axi, ayi, azi;
 
void loop() {
 
 // float ax, ay, az;   //scaled accelerometer values

  // read gyro measurements from device, scaled to the configured range
  CurieIMU.readGyroScaled(gx, gy, gz);
 // read accelerometer measurements from device, scaled to the configured range
 CurieIMU.readAccelerometerScaled(ax, ay, az);

  // put your main code here, to run repeatedly:


    gxi = pack754_32(gx);
//    gx1 = unpack754_32(gxi);
    gyi = pack754_32(gy);
//    gy1 = unpack754_32(gyi);
    gzi = pack754_32(gz);
//    gz1 = unpack754_32(gzi);
   axi = pack754_32(ax);
//    ax1 = unpack754_32(axi);
    ayi = pack754_32(ay);
//    ay1 = unpack754_32(ayi);
    azi = pack754_32(az);
//    az1 = unpack754_32(azi);


//    Serial.println( gx);
//     Serial.print("\t");
//    Serial.println( gxi);
//     Serial.print("\t");
//    Serial.println( gy);
//     Serial.print("\t");
//    Serial.println( gy1);
//     Serial.print("\t");
//    Serial.println( gz);
//     Serial.print("\t");
//    Serial.println( gz1);

uint32_t stmp[6] = {gxi, gyi, gzi, axi, ayi, azi};
uint32_t frame[2];

 for (int i=0; i < 6; i++)
  {
    Serial.println(i);
    Serial.println(stmp[i]);
    frame[0]=i;
    frame[1]=stmp[i];
     CAN.sendMsgBuf(0x00, 1, sizeof(float)*2, (byte*)&frame);
     delay(100);  
  }

    // send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf
   
                         // send data per 100ms
}

