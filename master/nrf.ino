#include "nrf.h"

//***************************************NRF24L01*******************************************************
#define READ_REG        0x00    //
#define WRITE_REG       0x20  //
#define RD_RX_PLOAD     0x61    //
#define WR_TX_PLOAD     0xA0    //
#define FLUSH_TX        0xE1  //
#define FLUSH_RX        0xE2    //
#define REUSE_TX_PL     0xE3    //
#define NOP             0xFF    //
//*************************************SPI(nRF24L01)·****************************************************
#define CONFIG          0x00  //
#define EN_AA           0x01  //
#define EN_RXADDR       0x02  //
#define SETUP_AW        0x03  //
#define SETUP_RETR      0x04  //
#define RF_CH           0x05  //
#define RF_SETUP        0x06  //
#define STATUS          0x07  //
#define OBSERVE_TX      0x08  // ·
#define CD              0x09  //
#define RX_ADDR_P0      0x0A  //
#define RX_ADDR_P1      0x0B  //
#define RX_ADDR_P2      0x0C  //
#define RX_ADDR_P3      0x0D  //
#define RX_ADDR_P4      0x0E  //
#define RX_ADDR_P5      0x0F  //
#define TX_ADDR         0x10  //
#define RX_PW_P0        0x11  //
#define RX_PW_P1        0x12  //
#define RX_PW_P2        0x13  //
#define RX_PW_P3        0x14  //
#define RX_PW_P4        0x15  //
#define RX_PW_P5        0x16  //
#define FIFO_STATUS     0x17  //
//**************************************************************************************

unsigned char const TX_ADDRESS[NRF_ADDRESS_WIDTH]= NRF_TX_ADDRESS;
unsigned char const RX_ADDRESS[NRF_ADDRESS_WIDTH]= NRF_RX_ADDRESS;

inline void nrf_chip_enable(unsigned char value) { digitalWrite(NRF_CE_PIN, value); }
inline void nrf_spi_enable(unsigned char value) { digitalWrite(NRF_CSN_PIN, value); }
unsigned char nrf_spi_rw(unsigned char buff);
unsigned char nrf_spi_read(unsigned char reg);
unsigned char nrf_spi_rw_reg(unsigned char reg, unsigned char value);
unsigned char nrf_spi_read_buf(unsigned char reg, unsigned char *pBuf, unsigned char uchars);
unsigned char nrf_spi_write_buf(unsigned char reg, const unsigned char *pBuf, unsigned char uchars);

void nrf_init()
{
	pinMode(NRF_CE_PIN, OUTPUT);
	pinMode(NRF_CSN_PIN, OUTPUT);
	pinMode(NRF_SCK_PIN, OUTPUT);
	pinMode(NRF_MOSI_PIN, OUTPUT);
	pinMode(NRF_MISO_PIN, INPUT_PULLUP);
	nrf_delay_us(100);
 	nrf_chip_enable(LOW);	// chip enable
 	nrf_spi_enable(HIGH);	// Spi disable
 	digitalWrite(NRF_SCK_PIN, LOW); // Spi clock line init high
	nrf_spi_write_buf(WRITE_REG + TX_ADDR, TX_ADDRESS, NRF_ADDRESS_WIDTH);
	nrf_spi_write_buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, NRF_ADDRESS_WIDTH);
	nrf_spi_rw_reg(WRITE_REG + EN_AA, 0x01);      // EN P0, 2-->P1
	nrf_spi_rw_reg(WRITE_REG + EN_RXADDR, 0x01);  //Enable data P0
	nrf_spi_rw_reg(WRITE_REG + RF_CH, 0);        // Chanel 0 RF = 2400 + RF_CH* (1or 2 M)
	nrf_spi_rw_reg(WRITE_REG + RX_PW_P0, NRF_PAYLOAD_WIDTH);
	nrf_spi_rw_reg(WRITE_REG + RF_SETUP, 0x07);   		// 1M, 0dbm
	nrf_spi_rw_reg(WRITE_REG + CONFIG, 0x0e);   		 // Enable CRC, 2 byte CRC, Send
}

void nrf_set_rx_mode()
{
	nrf_chip_enable(LOW);
	nrf_spi_rw_reg(WRITE_REG + CONFIG, 0x0f);	// Enable CRC, 2 byte CRC, Recive
	nrf_chip_enable(HIGH);
	nrf_delay_us(130);
}

void nrf_set_tx_mode()
{
	nrf_chip_enable(LOW);
	nrf_spi_rw_reg(WRITE_REG + CONFIG, 0x0e);	// Enable CRC, 2 byte CRC, Send
	nrf_chip_enable(HIGH);
	nrf_delay_us(130);
}

unsigned char nrf_rx_packet(unsigned char* rx_buf)
{
	unsigned char revale=0;
    unsigned char sta = nrf_spi_read(STATUS);
    if((sta & 0x40)!=0)		// Data in RX FIFO
	{
		nrf_chip_enable(LOW);
		nrf_spi_read_buf(RD_RX_PLOAD, rx_buf, NRF_PAYLOAD_WIDTH);// read receive payload from RX_FIFO buffer
		revale =1;	
	}
	nrf_spi_rw_reg(WRITE_REG+STATUS, sta);   
	return revale;
}

void nrf_tx_packet(unsigned char* tx_buf)
{
	nrf_chip_enable(LOW);
	nrf_spi_write_buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, NRF_ADDRESS_WIDTH); // Send Address
	nrf_spi_write_buf(WR_TX_PLOAD, tx_buf, NRF_PAYLOAD_WIDTH); 			 //send data
	nrf_spi_rw_reg(WRITE_REG + CONFIG, 0x0e);   		 // Send Out
	nrf_chip_enable(HIGH);
}

unsigned char nrf_spi_rw(unsigned char buff)
{
	for (unsigned char i=0; i < 8; i++)
	{
		digitalWrite(NRF_MOSI_PIN, buff >> 7);	// output MSB to MOSI (or buff & 0x80)
		nrf_delay_us(NRF_SPI_DELAY_US);
		buff = buff << 1;           		// shift next bit into MSB
		digitalWrite(NRF_SCK_PIN, HIGH);        // set SCK high
		nrf_delay_us(NRF_SPI_DELAY_US);
		buff |= digitalRead(NRF_MISO_PIN) & 1;	// capture current MISO bit
		digitalWrite(NRF_SCK_PIN, LOW);			// ... then set SCK low again
	}
	return buff;
}

unsigned char nrf_spi_read(unsigned char reg)
{
	unsigned char reg_val;
	nrf_spi_enable(LOW);		// CSN low, initialize SPI communication
	nrf_spi_rw(reg);            // select register to read
	reg_val = nrf_spi_rw(0);    // ... then read registervalue
	nrf_spi_enable(HIGH);		// CSN high, terminate SPI communication
	return reg_val;				// return register value
}

unsigned char nrf_spi_rw_reg(unsigned char reg, unsigned char value)
{
	unsigned char status;
	nrf_spi_enable(LOW);		// CSN low, init SPI transaction
	status = nrf_spi_rw(reg);	// select register
	nrf_spi_rw(value);			// ... and write value to it
	nrf_spi_enable(HIGH);		// CSN high again
	return status;				// return nRF24L01 status uchar
}

unsigned char nrf_spi_read_buf(unsigned char reg, unsigned char *pBuf, unsigned char uchars)
{
	unsigned char status, uchar_ctr;
	nrf_spi_enable(LOW);
	status = nrf_spi_rw(reg);
	for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++)
		pBuf[uchar_ctr] = nrf_spi_rw(0);
	nrf_spi_enable(HIGH);
	return status;
}

unsigned char nrf_spi_write_buf(unsigned char reg, const unsigned char *pBuf, unsigned char uchars)
{
	unsigned char status,uchar_ctr;
	nrf_spi_enable(LOW);
	status = nrf_spi_rw(reg);
	for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++)
		nrf_spi_rw(*pBuf++);
	nrf_spi_enable(HIGH);
	return(status);
}
