#include<reg51.h>
#include<stdio.h>

sbit LED1=P1^0; sbit LED2=P1^1; 
unsigned char temp=25, hum=60;
unsigned int counter=0, sec_count=0;

void uart_init(void) {
    TMOD=0x20;       // Timer1 Mode 2 (Baud rate)
    TH1=0xFD;        // 9600 baud @11.0592MHz
    SCON=0x50;       // Mode 1, REN=1
    TR1=1;           // Start Timer1
    TI=1;            // Transmit interrupt clear
    ES=1; EA=1;      // Serial + Global interrupt
}

void delay_ms(unsigned int t) {
    unsigned int i,j; for(i=0;i<t;i++) for(j=0;j<120;j++);
}

void display_data(void) {
    printf("T:%02dC H:%02d%% CNT:%04d SEC:%02d\r\n", 
           temp, hum, counter++, sec_count);
    LED1=~LED1;  // Heartbeat LED
}

void timer0_isr() interrupt 1 using 1 {
    TH0=(65536-50000)/256; TL0=65536-50000; // 50ms
    sec_count+=50;
    if(sec_count>=1000) {
        sec_count=0;
        temp++; if(temp>35) temp=25;
        hum+=3; if(hum>85) hum=50;
        display_data();  // Send to PC
    }
}

void main(void) {
    P1=0xFF;
    uart_init();
    TMOD|=0x02;  // Timer0 Mode 2 (baud rate)
    ET0=1;       // Timer0 interrupt
    printf("\r\n=== UART Data Logger Started ===\r\n");
    printf("Format: Temp(C) Humidity(%%) Counter Sec\r\n");
    while(1) {
        delay_ms(100);  // Main loop
    }
}
