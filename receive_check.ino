// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// loovee, 2014-6-13


#include <SPI.h>
#include "mcp_can.h"
#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

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


// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
    Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");
}

//float unbuff;

void loop()
{
    unsigned char len = 0;
    unsigned char buf[8];
    float gx, gy, gz, ax, ay, az;

    if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        uint32_t val1 = (uint32_t)((buf[7]<<24)|(buf[6]<<16)|(buf[5]<<8)|(buf[4]));
        uint32_t val0 = (uint32_t)((buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|(buf[0]));
        float floatval1 = unpack754_32(val1);
        
        switch(val0){
          case 0:
            gx = floatval1;
            break;
          case 1:
            gy = floatval1;
            break;
          case 2:
            gz = floatval1;
            break;
          case 3:
            ax = floatval1;
            break;
          case 4:
            ay = floatval1;
            break;
          case 5:
            az = floatval1;
            break;
        }
        
                                  

        unsigned int canId = CAN.getCanId();

        
        Serial.println(len);        
        Serial.println(val0);
        Serial.println(floatval1);
        
        
        Serial.println("-----------------------------");
        Serial.print("Get data from ID: ");
        Serial.println(canId, HEX);
        Serial.println();
    }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
