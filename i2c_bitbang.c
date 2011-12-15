


//from http://codinglab.blogspot.com/2008/10/i2c-on-avr-using-bit-banging.html

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>

// Port for the I2C
#define I2C_DDR DDRB
#define I2C_PIN PINB
#define I2C_PORT PORTB

// Pins to be used in the bit banging
#define I2C_CLK 6
#define I2C_DAT 7

#define I2C_DATA_HI() I2C_DDR &= ~( 1 << I2C_DAT );
#define I2C_DATA_LO() I2C_DDR |= ( 1 << I2C_DAT );

#define I2C_CLOCK_HI() I2C_DDR &= ~( 1 << I2C_CLK );
#define I2C_CLOCK_LO() I2C_DDR |= ( 1 << I2C_CLK );

#define DELAY() (_delay_ms(1))

void I2C_WriteBit( unsigned char c )
{
	if ( c > 0 )
	{
		I2C_DATA_HI();
	}
	else
	{
		I2C_DATA_LO();
	}
	
	I2C_CLOCK_HI();
	DELAY();
	
	I2C_CLOCK_LO();
	DELAY();
	
	if ( c > 0 )
	{
		I2C_DATA_LO();
	}
	
	DELAY();
	
}

unsigned char I2C_ReadBit()
{
	I2C_DATA_HI();
	
	I2C_CLOCK_HI();
	DELAY();
	
	unsigned char c = I2C_PIN;
	
	I2C_CLOCK_LO();
	DELAY();
	
	return ( c >> I2C_DAT ) & 1;
}

// Inits bitbanging port, must be called before using the functions below
//
void I2C_Init()
{
	I2C_PORT &= ~( ( 1 << I2C_DAT ) | ( 1 << I2C_CLK ) );
	
	I2C_CLOCK_HI();
	I2C_DATA_HI();
	
	DELAY();
}

// Send a START Condition
//
void I2C_Start()
{
	// set both to high at the same time
	I2C_DDR &= ~( ( 1 << I2C_DAT ) | ( 1 << I2C_CLK ) );
	DELAY();
	
	I2C_DATA_LO();
	DELAY();
	
	I2C_CLOCK_LO();
	DELAY();
}

// Send a STOP Condition
//
void I2C_Stop()
{
	I2C_CLOCK_HI();
	DELAY();
	
	I2C_DATA_HI();
	DELAY();
}

// write a byte to the I2C slave device
//
unsigned char I2C_Write( unsigned char c )
{
	for ( char i=0;i<8;i++)
	{
		I2C_WriteBit( c & 128 );
		
		c<<=1;
	}
	
	return I2C_ReadBit();
}


// read a byte from the I2C slave device
//
unsigned char I2C_Read( unsigned char ack )
{
	unsigned char res = 0;
	
	for ( char i=0;i<8;i++)
	{
		res <<= 1;
		res |= I2C_ReadBit();
	}
	
	if ( ack > 0)
	{
		I2C_WriteBit( 0 );
	}
	else
	{
		I2C_WriteBit( 1 );
	}
	
	DELAY();
	
	return res;
}
