/*
 * MCP6S91 PGA.c
 *
 * Created: 22/05/2021 11:53:34
 * Author : Leonardo Lisa
 *
 */ 

#define F_CPU 16000000

#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>

#define BAUD  9600
#define BRC   ((F_CPU/16/BAUD) - 1)

#define SHUT_DOWN_C 0b00100000 // PGA enters Shutdown mode as soon as a full 16-bit word is sent and CS is raised.
#define WRITE_C     0b01000000 // Write to register.

#define DATA PORTB0
#define CLK  PORTB1
#define CS   PORTB2
#define D1   PORTB2
#define D2   PORTB2
#define D3   PORTB2

/* Gain table:
000 = Gain of +1
001 = Gain of +2
010 = Gain of +4
011 = Gain of +5
100 = Gain of +8
101 = Gain of +10
110 = Gain of +16
111 = Gain of +32
*/

void setGain(uint8_t _gain);
void sleepMode(void);
void shiftOut(uint8_t _data);
void USART_Begin(unsigned int brc);
void USART_Transmit(uint8_t _data);
uint8_t USART_Recive(void);
void USART_Transmit_Buffer(char *buffer, uint8_t endl);
int USART_Transmit_Int(int _data, uint8_t _endl);
void USART_Flush(void);
int USART_Get_int(uint8_t endl);


int main(void)
{
	int gain;
	const uint8_t gain_table[8][2] = {{1, 0}, {2, 1}, {4, 2}, {5, 3}, {8, 4}, {10, 5}, {16, 6}, {32, 7}};
	uint8_t valid = 0;
	
	USART_Begin(BRC);
	USART_Transmit_Buffer("Serial programmable gain amplifier", '\n');
	
    DDRB = (1 << DATA)|(1 << CLK)|(1 << CS);
	PORTB = (0 << DATA)|(0 << CLK)|(1 << CS);
	
    while (1)
    {
		gain = USART_Get_int('\n');
		
		if (gain == -1)
		{
			USART_Transmit_Buffer("Error", '\n');
		}
		else
		{
			for (int i = 0; i < 8;++i)
			{
				if (gain_table[i][0] == gain)
				{
					valid = 1;
					setGain(gain_table[i][1]);
				}
			}
			
			if (gain != 0 && valid != 1)
			{
				USART_Transmit_Buffer("Unable set a gain of: ", 0);
				USART_Transmit_Int(gain, '\n');
				USART_Transmit_Buffer("You must enter one of the following value",  '\n');
				USART_Transmit_Buffer("0 = Enter in high impedance state", '\n');
				USART_Transmit_Buffer("1 = Gain of +1", '\n');
				USART_Transmit_Buffer("2 = Gain of +2", '\n');
				USART_Transmit_Buffer("4 = Gain of +4", '\n');
				USART_Transmit_Buffer("5 = Gain of +5", '\n');
				USART_Transmit_Buffer("8 = Gain of +8", '\n');
				USART_Transmit_Buffer("10 = Gain of +10", '\n');
				USART_Transmit_Buffer("16 = Gain of +16", '\n');
				USART_Transmit_Buffer("32 = Gain of +32", '\n');
			}
			else
			{
				if (valid == 1)
				{
					USART_Transmit_Buffer("You set a gain of: ", 0);
					USART_Transmit_Int(gain, '\n');
					_delay_ms(500);
					valid = 0;
				}
				
				if (gain == 0)
				{
					USART_Transmit_Buffer("Enter in high impedance state, no gain", '\n');
					sleepMode();
				}
			}
		}
    }
}

// Set PGA gain.
void setGain(uint8_t _gain)
{
	PORTB &= ~(1 << CS);
	shiftOut(WRITE_C);
	shiftOut(_gain & 0b00000111);
	PORTB |= (1 << CS);
	
}

// Put the PGA in sleep mode.
void sleepMode(void)
{
	PORTB &= ~(1 << CS);
	shiftOut(SHUT_DOWN_C);
	shiftOut(0);
	PORTB |= (1 << CS);
}

// Shift-out an 8-bit value, LSB first, data on rising edge.
void shiftOut(uint8_t _data)
{
	for (int i = 7; i >= 0; --i)
	{
		PORTB &= ~(1 << CLK);
		_delay_us(10);
		if (!!(_data & (1 << i)))
		{
			PORTB |= (1 << DATA);
		}
		else
		{
			PORTB &= ~(1 << DATA);
		}
		PORTB |= (1 << CLK);
		_delay_us(10);
	}
	
	PORTB &= ~(1 << CLK);
}

// Initialize serial communication.
void USART_Begin(unsigned int brc)
{
	// Set baud rate.
	UBRR0H = (unsigned char) (brc >> 8);
	UBRR0L = (unsigned char) brc;
	// Enable receiver and transmitter.
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);
	// Set frame format: 8data, 1stop bit.
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Send one byte.
void USART_Transmit(uint8_t _data)
{
	// Wait for empty transmit buffer.
	while (!(UCSR0A & (1<<UDRE0)));
	// Put data into buffer, sends the data.
	UDR0 = _data;
}

// Receive one byte
uint8_t USART_Recive(void)
{
	// Wait for data to be received.
	while (!(UCSR0A & (1<<RXC0)));
	// Get and return received data from buffer.
	return UDR0;
}

// Send data terminate with '\0' if endl is set.
void USART_Transmit_Buffer(char *buffer, uint8_t endl)
{
	while (*buffer != '\0')
	{
		USART_Transmit(*buffer);
		buffer++;
	}
	
	if (endl)
	{
		USART_Transmit('\n');
	}
}

// Transmit int.
int USART_Transmit_Int(int _data, uint8_t _endl)
{
	char string[6];
	
	if (_data > 32767)
	{
		return -1;
	}
	
	itoa(_data, string, 10);
	
	USART_Transmit_Buffer(string, _endl);
	
	return 0;
}

void USART_Flush(void)
{
	unsigned char dummy;
	while (UCSR0A & (1<<RXC0)) dummy = UDR0;
}

// Receive int.
int USART_Get_int(uint8_t endl)
{
	char buffer[6] = {endl, endl, endl, endl, endl, 0};
	
	for (int i = 0; i < 6; ++i)
	{
		buffer[i] = USART_Recive();
		if (buffer[i] == endl)
		{
			buffer[5] = endl;
			break;
		}
		if (buffer[i] < 48 || buffer[i] > 57)
		{
			USART_Transmit_Buffer("Invalid data", endl);
			USART_Flush();
			return -1;
		}
	}
	
	if (buffer[5] != endl)
	{
		USART_Transmit_Buffer("String too long", endl);
		USART_Flush();
		return -1;
	}
	
	return atoi(buffer);
}