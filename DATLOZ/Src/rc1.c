// Mifare RC522 RFID Card reader 13.56 MHz
// STM32F103 RFID RC522 SPI1 / UART / USB / Keil HAL / 2017 vk.com/zz555

// PA0  - (OUT)	LED2
// PA1	- (IN)	BTN1
// PA4  - (OUT)	SPI1_NSS (Soft)
// PA5  - (OUT)	SPI1_SCK
// PA6  - (IN)	SPI1_MISO (Master In)
// PA7  - (OUT)	SPI1_MOSI (Master Out)
// PA9	- (OUT)	TX UART1 (RX-RS232)
// PA10	- (IN)	RX UART1 (TX-RS232)
// PA11 - (OUT) USB_DM
// PA12 - (OUT) USB_DP
// PA13 - (IN) 	SWDIO
// PA14 - (IN) 	SWDCLK
// PC13 - (OUT)	LED1

// MFRC522		STM32F103		DESCRIPTION
// CS (SDA)		PA4					SPI1_NSS	Chip select for SPI
// SCK				PA5					SPI1_SCK	Serial Clock for SPI
// MOSI				PA7 				SPI1_MOSI	Master In Slave Out for SPI
// MISO				PA6					SPI1_MISO	Master Out Slave In for SPI
// IRQ				-						Irq
// GND				GND					Ground
// RST				3.3V				Reset pin (3.3V)
// VCC				3.3V				3.3V power

#include "stm32f1xx_hal.h"
#include "rc1.h"

extern SPI_HandleTypeDef hspi1;

// RC522
extern uint8_t MFRC1_Check(uint8_t* id);
extern uint8_t MFRC1_Compare(uint8_t* CardID, uint8_t* CompareID);
extern void MFRC1_WriteRegister(uint8_t addr, uint8_t val);
extern uint8_t MFRC1_ReadRegister(uint8_t addr);
extern void MFRC1_SetBitMask(uint8_t reg, uint8_t mask);
extern void MFRC1_ClearBitMask(uint8_t reg, uint8_t mask);
extern uint8_t MFRC1_Request(uint8_t reqMode, uint8_t* TagType);
extern uint8_t MFRC1_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen);
extern uint8_t MFRC1_Anticoll(uint8_t* serNum);
extern void MFRC1_CalulateCRC(uint8_t* pIndata, uint8_t len, uint8_t* pOutData);
extern uint8_t MFRC1_SelectTag(uint8_t* serNum);
extern uint8_t MFRC1_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t* Sectorkey, uint8_t* serNum);
extern uint8_t MFRC1_Read(uint8_t blockAddr, uint8_t* recvData);
extern uint8_t MFRC1_Write(uint8_t blockAddr, uint8_t* writeData);
extern void MFRC1_Init(void);
extern void MFRC1_Reset(void);
extern void MFRC1_AntennaOn(void);
extern void MFRC1_AntennaOff(void);
extern void MFRC1_Halt(void);

uint8_t SPI1SendByte1(uint8_t data) {
	unsigned char writeCommand[1];
	unsigned char readValue[1];
	
	writeCommand[0] = data;
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&writeCommand, (uint8_t*)&readValue, 1, 10);
	return readValue[0];
	
	//while (SPI1->SR & SPI_SR_BSY);								// STM32F030 - ждем конец передачи
	//while (SPI1->SR & SPI_I2S_FLAG_BSY);					// STM32F103 - ждем конец передачи

	//while (!(SPI1->SR & SPI_SR_TXE));     				// убедиться, что предыдущая передача завершена (STM32F103)
	//SPI1->DR = data;															// вывод в SPI1
	//while (!(SPI1->SR & SPI_SR_RXNE));     				// ждем окончания обмена (STM32F103)
	//for (uint8_t i=0; i<50; i++) {};
	//data = SPI1->DR;															// читаем принятые данные
	//return data;
}

void SPI1_WriteReg1(uint8_t address, uint8_t value) {
	cs1_reset();
	SPI1SendByte1(address);
	SPI1SendByte1(value);
	cs1_set();
}

uint8_t SPI1_ReadReg1(uint8_t address) {
	uint8_t	val;

	cs1_reset();
	SPI1SendByte1(address);
	val = SPI1SendByte1(0x00);
	cs1_set();
	return val;
}

void MFRC1_WriteRegister(uint8_t addr, uint8_t val) {
	addr = (addr << 1) & 0x7E;															// Address format: 0XXXXXX0
  SPI1_WriteReg1(addr, val);
}

uint8_t MFRC1_ReadRegister(uint8_t addr) {
	uint8_t val;

	addr = ((addr << 1) & 0x7E) | 0x80;
	val = SPI1_ReadReg1(addr);
	return val;	
}

uint8_t MFRC1_Check(uint8_t* id) {
	uint8_t status;
	status = MFRC1_Request(PICC_REQIDL, id);							// Find cards, return card type
	if (status == MI_OK) status = MFRC1_Anticoll(id);			// Card detected. Anti-collision, return card serial number 4 bytes
	MFRC1_Halt();																					// Command card into hibernation 
	return status;
}

uint8_t MFRC1_Compare(uint8_t* CardID, uint8_t* CompareID) {
	uint8_t i;
	for (i = 0; i < 5; i++) {
		if (CardID[i] != CompareID[i]) return MI_ERR;
	}
	return MI_OK;
}

void MFRC1_SetBitMask(uint8_t reg, uint8_t mask) {
	MFRC1_WriteRegister(reg, MFRC1_ReadRegister(reg) | mask);
}

void MFRC1_ClearBitMask(uint8_t reg, uint8_t mask){
	MFRC1_WriteRegister(reg, MFRC1_ReadRegister(reg) & (~mask));
}

uint8_t MFRC1_Request(uint8_t reqMode, uint8_t* TagType) {
	uint8_t status;  
	uint16_t backBits;																			// The received data bits

	MFRC1_WriteRegister(MFRC1_REG_BIT_FRAMING, 0x07);		// TxLastBists = BitFramingReg[2..0]
	TagType[0] = reqMode;
	status = MFRC1_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
	if ((status != MI_OK) || (backBits != 0x10)) status = MI_ERR;
	return status;
}

uint8_t MFRC1_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen) {
	uint8_t status = MI_ERR;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;
	uint8_t lastBits;
	uint8_t n;
	uint16_t i;

	switch (command) {
		case PCD_AUTHENT: {
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case PCD_TRANSCEIVE: {
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default:
		break;
	}

	MFRC1_WriteRegister(MFRC1_REG_COMM_IE_N, irqEn | 0x80);
	MFRC1_ClearBitMask(MFRC1_REG_COMM_IRQ, 0x80);
	MFRC1_SetBitMask(MFRC1_REG_FIFO_LEVEL, 0x80);
	MFRC1_WriteRegister(MFRC1_REG_COMMAND, PCD_IDLE);

	// Writing data to the FIFO
	for (i = 0; i < sendLen; i++) MFRC1_WriteRegister(MFRC1_REG_FIFO_DATA, sendData[i]);

	// Execute the command
	MFRC1_WriteRegister(MFRC1_REG_COMMAND, command);
	if (command == PCD_TRANSCEIVE) MFRC1_SetBitMask(MFRC1_REG_BIT_FRAMING, 0x80);		// StartSend=1,transmission of data starts 

	// Waiting to receive data to complete
	i = 2000;	// i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms
	do {
		// CommIrqReg[7..0]
		// Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		n = MFRC1_ReadRegister(MFRC1_REG_COMM_IRQ);
		i--;
	} while ((i!=0) && !(n&0x01) && !(n&waitIRq));

	MFRC1_ClearBitMask(MFRC1_REG_BIT_FRAMING, 0x80);																// StartSend=0

	if (i != 0)  {
		if (!(MFRC1_ReadRegister(MFRC1_REG_ERROR) & 0x1B)) {
			status = MI_OK;
			if (n & irqEn & 0x01) status = MI_NOTAGERR;
			if (command == PCD_TRANSCEIVE) {
				n = MFRC1_ReadRegister(MFRC1_REG_FIFO_LEVEL);
				lastBits = MFRC1_ReadRegister(MFRC1_REG_CONTROL) & 0x07;
				if (lastBits) *backLen = (n-1)*8+lastBits; else *backLen = n*8;
				if (n == 0) n = 1;
				if (n > MFRC1_MAX_LEN) n = MFRC1_MAX_LEN;
				for (i = 0; i < n; i++) backData[i] = MFRC1_ReadRegister(MFRC1_REG_FIFO_DATA);		// Reading the received data in FIFO
			}
		} else status = MI_ERR;
	}
	return status;
}

uint8_t MFRC1_Anticoll(uint8_t* serNum) {
	uint8_t status;
	uint8_t i;
	uint8_t serNumCheck = 0;
	uint16_t unLen;

	MFRC1_WriteRegister(MFRC1_REG_BIT_FRAMING, 0x00);												// TxLastBists = BitFramingReg[2..0]
	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = MFRC1_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
	if (status == MI_OK) {
		// Check card serial number
		for (i = 0; i < 4; i++) serNumCheck ^= serNum[i];
		if (serNumCheck != serNum[i]) status = MI_ERR;
	}
	return status;
} 

void MFRC1_CalculateCRC(uint8_t*  pIndata, uint8_t len, uint8_t* pOutData) {
	uint8_t i, n;

	MFRC1_ClearBitMask(MFRC1_REG_DIV_IRQ, 0x04);													// CRCIrq = 0
	MFRC1_SetBitMask(MFRC1_REG_FIFO_LEVEL, 0x80);													// Clear the FIFO pointer
	// Write_MFRC522(CommandReg, PCD_IDLE);

	// Writing data to the FIFO	
	for (i = 0; i < len; i++) MFRC1_WriteRegister(MFRC1_REG_FIFO_DATA, *(pIndata+i));
	MFRC1_WriteRegister(MFRC1_REG_COMMAND, PCD_CALCCRC);

	// Wait CRC calculation is complete
	i = 0xFF;
	do {
		n = MFRC1_ReadRegister(MFRC1_REG_DIV_IRQ);
		i--;
	} while ((i!=0) && !(n&0x04));																						// CRCIrq = 1

	// Read CRC calculation result
	pOutData[0] = MFRC1_ReadRegister(MFRC1_REG_CRC_RESULT_L);
	pOutData[1] = MFRC1_ReadRegister(MFRC1_REG_CRC_RESULT_M);
}

uint8_t MFRC1_SelectTag(uint8_t* serNum) {
	uint8_t i;
	uint8_t status;
	uint8_t size;
	uint16_t recvBits;
	uint8_t buffer[9]; 

	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;
	for (i = 0; i < 5; i++) buffer[i+2] = *(serNum+i);
	MFRC1_CalculateCRC(buffer, 7, &buffer[7]);		//??
	status = MFRC1_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
	if ((status == MI_OK) && (recvBits == 0x18)) size = buffer[0]; else size = 0;
	return size;
}

uint8_t MFRC1_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t* Sectorkey, uint8_t* serNum) {
	uint8_t status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[12]; 

	// Verify the command block address + sector + password + card serial number
	buff[0] = authMode;
	buff[1] = BlockAddr;
	for (i = 0; i < 6; i++) buff[i+2] = *(Sectorkey+i);
	for (i=0; i<4; i++) buff[i+8] = *(serNum+i);
	status = MFRC1_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);
	if ((status != MI_OK) || (!(MFRC1_ReadRegister(MFRC1_REG_STATUS2) & 0x08))) status = MI_ERR;
	return status;
}

uint8_t MFRC1_Read(uint8_t blockAddr, uint8_t* recvData) {
	uint8_t status;
	uint16_t unLen;

	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	MFRC1_CalculateCRC(recvData,2, &recvData[2]);
	status = MFRC1_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
	if ((status != MI_OK) || (unLen != 0x90)) status = MI_ERR;
	return status;
}

uint8_t MFRC1_Write(uint8_t blockAddr, uint8_t* writeData) {
	uint8_t status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[18]; 

	buff[0] = PICC_WRITE;
	buff[1] = blockAddr;
	MFRC1_CalculateCRC(buff, 2, &buff[2]);
	status = MFRC1_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);
	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) status = MI_ERR;
	if (status == MI_OK) {
		// Data to the FIFO write 16Byte
		for (i = 0; i < 16; i++) buff[i] = *(writeData+i);
		MFRC1_CalculateCRC(buff, 16, &buff[16]);
		status = MFRC1_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) status = MI_ERR;
	}
	return status;
}

void MFRC1_Init(void) {
	MFRC1_Reset();
	MFRC1_WriteRegister(MFRC1_REG_T_MODE, 0x8D);
	MFRC1_WriteRegister(MFRC1_REG_T_PRESCALER, 0x3E);
	MFRC1_WriteRegister(MFRC1_REG_T_RELOAD_L, 30);           
	MFRC1_WriteRegister(MFRC1_REG_T_RELOAD_H, 0);
	MFRC1_WriteRegister(MFRC1_REG_RF_CFG, 0x70);				// 48dB gain	
	MFRC1_WriteRegister(MFRC1_REG_TX_AUTO, 0x40);
	MFRC1_WriteRegister(MFRC1_REG_MODE, 0x3D);
	MFRC1_AntennaOn();																		// Open the antenna
}

void MFRC1_Reset(void) {
	MFRC1_WriteRegister(MFRC1_REG_COMMAND, PCD_RESETPHASE);
}

void MFRC1_AntennaOn(void) {
	uint8_t temp;

	temp = MFRC1_ReadRegister(MFRC1_REG_TX_CONTROL);
	if (!(temp & 0x03)) MFRC1_SetBitMask(MFRC1_REG_TX_CONTROL, 0x03);
}

void MFRC1_AntennaOff(void) {
	MFRC1_ClearBitMask(MFRC1_REG_TX_CONTROL, 0x03);
}

void MFRC1_Halt(void) {
	uint16_t unLen;
	uint8_t buff[4]; 

	buff[0] = PICC_HALT;
	buff[1] = 0;
	MFRC1_CalculateCRC(buff, 2, &buff[2]);
	MFRC1_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}
