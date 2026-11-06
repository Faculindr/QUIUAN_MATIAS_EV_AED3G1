#include "ds18b20.h"

void dq_output(){
    GPIOA->CRL&=~(0xF<<DQ);
    GPIOA->CRL|=(0x5<<DQ);
}
void dq_input(){
    GPIOA->CRL&=~(0xF<<DQ);
    GPIOA->CRL|=(0x4<<DQ);
}

void dq_low(){
    GPIOA->BSRR|=(1<<DQ+16);
}
void dq_high(){
    GPIOA->BSRR|=(1<<DQ);
}
uint8_t dq_read(){
    return GPIOA->IDR&(1<<DQ);
}

void ds18b20_init(){
    RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;

}

uint8_t ds18b20_reset(){//debera generar la secuencia de inicializacion y detectar el precence pulse
    dq_output();//ponemos el dq como salida para enviar datos
    dq_low();//ponemos dq como 0
    delay_us(490);//esperamos 480us y 10us de stroll
    dq_high();//lo liberamos (poner en 1)
    dq_input();//lo ponemos como entrada para recibir
    delay_us(70);//esperamos 70us para saber si hay PRESENCE
    int presence=!dq_read(); //leemos dq para saber si hay PRESENCE (dq en low)
    delay(419);
    return presence; 

}

void ds18b20_write_bit(uint8_t bit){//transmitir un unico bit al ds18b20
    if(bit){//si el bit que se quiere enviar es 1 
        dq_output();//ponemos dq como salida
        dq_low();//lo ponemos en 0
        delay_us(15);//esperamos los 15us 
        dq_high();//lo liberamos
    }
    else{
        dq_output();
        dq_low();
        delay_us(60);
    }
    delay_us(1);//recovery
}

uint8_t ds18b20_read_bit(){//generar un time slot de lectura y devolver el bit entregado
    uint8_t bit=1;
    dq_output();//ponemos dq como salida para enviar datos
    dq_low();
    delay_us(1);
    dq_high();//lo liberamos
    dq_input();
    if(dq_read()){//si lo leido en dq es 1
        bit=1;
        delay_us(15);//la informacion es valida 15us despues de inciar el time slot
    }
    else{//si es un 0
        bit=0;
        delay_us(15);//la informacion es valida 15us despues de inciar el time slot
        dq_high();//liberamos dq
    }
    return bit;
}

void ds18b20_write_byte(uint8_t data){
    for(int i=0;i<8;i++){
        ds18b20_write_bit((data>>i)&1);//escribo el byte desplazado i y lo multiplico por 1 para quedarme con el ultimo
        //asi voy pasando bit por bit 
    }
}

uint8_t ds18b20_read_byte(){
    uint8_t byte=0;
    for(int i=0;i<8;i++){
        byte=(ds18b20_read_bit()<<i);//el byte va a ser el bit leido desplazado i
    }
    return byte;
}

void ds18b20_start_conversion(){//funcion que enviara la secuencias de cmd para realizar una medicion de temp
    if(ds18b20_reset()==0) return; //si PRESENCE es 0 no se puede realizar la conversion
    ds18b20_write_byte(0xCC);//enviamos el cmd de rom skip
    ds18b20_write_byte(0x44);//enviamos el cmd de convert t y de funcion 
}

float ds18b20_get_temperature(){
    int temp;
    ds18b20_reset();
    ds18b20_write_byte(0xCC);//enviamos el cmd de rom skip
    ds18b20_write_byte(0xBE);//enviamos el cmd de read scratchpad
    int lsb = ds18b20_read_byte();
    int msb = ds18b20_read_byte();
    return (msb<<8)|lsb;
}