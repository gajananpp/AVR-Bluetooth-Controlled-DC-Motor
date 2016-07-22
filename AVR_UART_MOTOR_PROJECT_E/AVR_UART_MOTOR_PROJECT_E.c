
#define F_CPU 8000000UL

#define BAUD 9600
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)

#include<avr/io.h>
#include<util/delay.h>

void uart_init() {
	UBRRH = (BAUDRATE>>8);
	UBRRL = BAUDRATE;
	UCSRB = (1<<TXEN) | (1<<RXEN);
	UCSRC = (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1);
}

void pwm_init() {
	TCCR0 = (1<<WGM00) | (1<<COM01) | (1<<WGM01) | (1<<CS00);
	TCCR2 = (1<<WGM00) | (1<<COM01) | (1<<WGM01) | (1<<CS00);
	DDRB = (1<<DDRB3);
}

void uart_transmit_char(unsigned char data) {
	while (!(UCSRA & (1<<UDRE)));
	UDR = data;
}

void uart_transmit_str(unsigned char *str) {
	while (*str) {
		while (!(UCSRA & (1<<UDRE)));
		UDR = *str++;
	}
}

unsigned char uart_receive_char() {
	while (!(UCSRA & (1<<RXC)));
	return UDR;
}

unsigned char *uart_receive_str() {
	unsigned char string[200], x, i = 0;
	while ((x=uart_receive_char()) != 13) {
		string[i++] = x;
	}
	string[i] = '\0';
	return string;
}

void led_testing() {
	unsigned int i = 0;
	for (i=0; i<4; i++) {
		PORTC = (1<<PORTC2);
		_delay_ms(100);
		PORTC = (1<<PORTC3);
		_delay_ms(100);
		PORTC = (1<<PORTC4);
		_delay_ms(100);
		PORTC = (1<<PORTC5);
		_delay_ms(100);
		PORTD = (1<<PORTD7);
		_delay_ms(100);
		PORTD = (0<<PORTD7);
	}
}

int main() {
	unsigned char x, error="Error : First Turn ON The Motor !!!\r\n";
	unsigned int flag=0, initial_1=1, initial_2=0, i=0;
	MCUCSR = (1<<JTD);
	DDRC = (1<<DDRC0) | (1<<DDRC1) | (1<<DDRC2) | (1<<DDRC3) | (1<<DDRC4) | (1<<DDRC5) | (1<<DDRC6) | (1<<DDRC7);
	DDRD = (1<<DDRD7);

	led_testing();
	PORTC = (1<<PORTC3);
	uart_init();
	pwm_init();

	OCR0 = 255;
	OCR2 = 255;

	while (1) {
		x = uart_receive_char();
		switch (x) {
			case 'F' :
			uart_transmit_str("Everything Is OK !!!!!!\r\n");
			break;

			case 'A' :
			PORTC = (initial_1<<PORTC0) | (initial_2<<PORTC1) | (1<<PORTC2) | (0<<PORTC3);
			flag = 1;
			if (initial_1 == 1) {
				PORTC |= (1<<PORTC5);
			}
			else {
				PORTC |= (1<<PORTC4);
			}
			uart_transmit_str("Motor : ON\r\n");
			break;

			case 'B' :
			PORTC = (0<<PORTC0) | (0<<PORTC1) | (1<<PORTC3);
			flag = 0;
			uart_transmit_str("Motor : OFF\r\n");
			break;

			case 'C' :
			if (flag == 1) {
				PORTC = (0<<PORTC0) | (1<<PORTC1) | (1<<PORTC2) | (1<<PORTC4);
				initial_1 = 0, initial_2 = 1;
				uart_transmit_str("Motor Direction : Anti-Clockwise\r\n");
			}
			else {
				uart_transmit_str(error);
			}
			break;

			case 'D' :
			if (flag == 1) {
				PORTC = (1<<PORTC0) | (0<<PORTC1) | (1<<PORTC2) | (1<<PORTC5);
				initial_1 = 1, initial_2 = 0;
				uart_transmit_str("Motor Direction : Clockwise\r\n");
			}
			else {
				uart_transmit_str(error);
			}
			break;

			case 'H' :
			uart_transmit_str("\r\n|--------|--------------|\r\n|  Code  |    Action    |\r\n|--------|--------------|\r\n|   A    |     ON       |\r\n|   B    |     OFF      |\r\n|--------|--------------|\r\n|   C    |Anti-Clockwise|\r\n|   D    |  Clockwise   |\r\n|--------|--------------|\r\n|   F    |Check Connectn|\r\n|--------|--------------|\r\n");
			break;

			case 'P' :
			uart_transmit_str("Enter PWM Value(0-255) :\r\n");
			unsigned char string[10];
			i = 0;
			memset(string,0,strlen(string));
			while (strlen(string) != 3) {
				string[i++] = uart_receive_char();
			}
			if (atoi(string) < 256) {
				OCR0 = OCR2 = atoi(string);
			}
			break;

			case '1' :
			OCR0 = 150;
			OCR2 = 150;
			uart_transmit_str("Duty Cycle : 19.61%\r\nMotor Speed : 40 rpm\r\n");
			break;
			case '2' :
			OCR0 = 175;
			OCR2 = 175;
			uart_transmit_str("Duty Cycle : 39.21%\r\nMotor Speed : 78 rpm\r\n");
			break;
			case '3' :
			OCR0 = 200;
			OCR2 = 200;
			uart_transmit_str("Duty Cycle : 58.82%\r\nMotor Speed : 118 rpm\r\n");
			break;
			case '4' :
			OCR0 = 225;
			OCR2 = 225;
			uart_transmit_str("Duty Cycle : 78.43%\r\nMotor Speed : 157 rpm\r\n");
			break;
			case '5':
			OCR0 = 255;
			OCR2 = 255;
			uart_transmit_str("Duty Cycle : 100%\r\nMotor Speed : 200 rpm\r\n");
			break;
		}
		_delay_ms(100);
	}
	return 0;
}
