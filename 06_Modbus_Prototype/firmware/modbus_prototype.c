#include <reg51.h>

/* ---- Dummy memory ---- */
unsigned int holding_reg0 = 123;
unsigned int input_reg0   = 456;

/* ---- UART ---- */
void uart_init() {
    TMOD = 0x20;
    TH1  = 0xFD;
    SCON = 0x50;
    TR1  = 1;
}

void uart_tx(unsigned char c) {
    SBUF = c;
    while(!TI);
    TI = 0;
}

unsigned char uart_rx() {
    while(!RI);
    RI = 0;
    return SBUF;
}

unsigned int modbus_crc(unsigned char *buf, unsigned char len)
{
    unsigned int crc = 0xFFFF;
    unsigned char i, j;

    for(i=0;i<len;i++)
    {
        crc ^= buf[i];
        for(j=0;j<8;j++)
        {
            if(crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

/* ---- Main ---- */
void main()
{
    unsigned char req[8];
    int i;

    uart_init();

    while(1)
    {
        /* waiting for first byte */
        req[0] = uart_rx();

        /* reading remaining bytes */
        for(i=1;i<8;i++)
            req[i] = uart_rx();

        /* check slave id */
        if(req[0] != 1)
            continue;

        /* only support function 03 */
				if(req[1] == 0x03)
				{
						unsigned char resp[7];
						unsigned int crc;
						unsigned int addr;
						unsigned int value;

						/* extract requested register address */
						addr = (req[2] << 8) | req[3];

						/* choose value based on address */
						if(addr == 0)
								value = holding_reg0;
						else
								value = 0;   // unknown register ? return 0

						/* build response */
						resp[0] = 1;
						resp[1] = 3;
						resp[2] = 2;
						resp[3] = value >> 8;
						resp[4] = value & 0xFF;

						/* add CRC */
						crc = modbus_crc(resp,5);
						resp[5] = crc & 0xFF;
						resp[6] = crc >> 8;

						/* send response */
						for(i=0;i<7;i++)
								uart_tx(resp[i]);
				}
				
				if(req[1] == 0x06)
				{
						unsigned int addr;
						unsigned int value;
						unsigned int crc;
						unsigned char resp[8];

						/* extract address */
						addr = (req[2] << 8) | req[3];

						/* extract value */
						value = (req[4] << 8) | req[5];

						/* store value in memory */
						if(addr == 0)
								holding_reg0 = value;

						/* build response = echo request */
						for(i=0;i<6;i++)
								resp[i] = req[i];

						/* compute CRC for first 6 bytes */
						crc = modbus_crc(resp,6);

						resp[6] = crc & 0xFF;
						resp[7] = crc >> 8;

						/* send response */
						for(i=0;i<8;i++)
								uart_tx(resp[i]);
				}
    }
}