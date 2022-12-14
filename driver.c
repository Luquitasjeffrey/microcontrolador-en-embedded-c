//ATmega328P el microcontroller para el arduino nano
/*
avr-gcc -Os -mmcu=atmega328p driver.c -o driver
*/

//pulsador 1: pause
//pulsador 2: reset
#include <avr/io.h>
#define F_CPU 1600000000
#include <avr/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define INPUT_MODE 0
#define OUTPUT_MODE 1
#define ON 1
#define OFF 0

#define PAUSE      0b00100000
#define VENTILADOR 0b10000000
#define QUEMADOR   0b10000000
#define BUZZER     0b00001000
#define SYS_PAUSE  0b00000100
#define RESET      0b00010000
#define VALV_VAPOR 0b00000010
#define ADD        0b00000001
#define SUB        0b01000000


void DWrite(uint8_t pin, bool mode){
    if(mode){
        PORTB|=pin;//PORTB es el register del output en el puerto b
        return;
    }
    PORTB&=(~pin);
}

bool DRead(uint8_t pin){
    return (PINB&pin)!=0;//PINB es el register del input en el puerto b
}

void Pmode(uint8_t pin, bool mode){
    if(mode){
        DDRB|=pin;
        return;
    }
    DDRB&=(~pin);
}

void InitDisplay(){
    DDRD=0b01111111;
}

void display(uint32_t tiempo){
    #define C PORTD //me convenia usar puerto D jaja

    /*
    El puerto D se divide en 8 bits, a saber
    0b"<puerto 0><puerto 1><puerrto2><puerto 3><puerto 4><puerrto5><puerto 6><puerto 7>"
    ej:
    Si C=0b01010101;
    <Puerto 0 = OFF> <puerto 1 = ON> <puerto 2 = OFF> <puerto 3 = on> ...
    asi siguiendo con el numero binario que le asigne a C que representa el puerto D
    1 significa ON
    0 significa OFF
    */

    switch(tiempo){
        case 0:
            C=0b01111110;
            break;
        case 1:
            C=0b00000110;
            break;
        case 2:
            C=0b01101101;
            break;
        case 3:
            C=0b01111001;
            break;
        case 4:
            C=0b00110011;
            break;
        case 5:
            C=0b01011011;
            break;
        case 6:
            C=0b01011111;
            break;
        case 7:
            C=0b01110001;
            break;
        case 8:
            C=0b01111111;
            break;
        case 9:
            C=0b01111011;
            break;
        case 10:
            C=0b01110111;
            break;
        case 11:
            C=0b01111111;
            break;
        case 12:
            C=0b01001110;
            break;
        case 13:
            C=0b01111110;
            break;
        case 14:
            C=0b01001111;
            break;
        case 15:
            C=0b01000111;
            break;
        default:
            return;
    }
    #undef C
}

#define EXIT_SUCCESS 0
#define EXIT_FALUIRE 1

bool __main(uint32_t tiempo){

    //DWrite(QUEMADOR, ON);//son el mismo pin
    DWrite(VENTILADOR, ON);
    uint32_t i=0;
    bool doitagain=1;
task:
    for(; i<((tiempo*3)/4); i++){//75% del tiempo
        _delay_ms(1000);
        if(DRead(SYS_PAUSE)){
            _delay_ms(50); //por si llegaba a quedar algo de corriente para que se desactive el puerto b
            while(!DRead(SYS_PAUSE)){
                _delay_ms(50);
                if(DRead(ADD)){
                    tiempo++;
                    display(tiempo);
                    continue;
                }
                if(DRead(SUB)){
                    tiempo--;
                    display(tiempo);
                    continue;
                }
                if(DRead(RESET)){
                    return EXIT_FALUIRE;
                }
            }
        }
        if(DRead(RESET)){
            return EXIT_FALUIRE;
        }
        display(i);
    }
    if(doitagain){
        DWrite(VALV_VAPOR, ON);
        tiempo*=4;
        tiempo/=3;
        doitagain=0;
        goto task;
    }
    return EXIT_SUCCESS;//si no lo reseteaste tetrmina
}

void __init__(){
    //pooner el modo de los pines de input/output
    Pmode(VALV_VAPOR, OUTPUT_MODE);
    Pmode(QUEMADOR, OUTPUT_MODE);
    Pmode(BUZZER, OUTPUT_MODE);
    Pmode(SYS_PAUSE, INPUT_MODE);
    Pmode(VENTILADOR, OUTPUT_MODE);
    Pmode(ADD, INPUT_MODE);
    Pmode(SUB, INPUT_MODE);
    Pmode(RESET, INPUT_MODE);
    //poner todos los registros que voy a usar en cero
    DDRB=0;
    //inicializar el display
    InitDisplay();
}

int main(void){
    __init__();
    uint32_t tiempo=10;
    while(!DRead(SYS_PAUSE));
    while(!__main(tiempo)){
        DDRB=0;
    }
    DDRB=0;//solo para resetear todo en cero y apagar todo
    DWrite(BUZZER, ON);
    _delay_ms(10000);//10000 ms=10 sec
    DWrite(BUZZER, OFF);//apago el buzzer
    return 0;//fin
}
