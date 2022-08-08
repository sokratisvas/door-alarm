#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>

#define TRIG_DDR DDRB
#define ECHO_DDR DDRD
#define TRIG_PORT PORTB
#define ECHO_PORT PORTD
#define TRIG_PIN 5
#define ECHO_PIN 2

#define BUZZER_PORT PORTB
#define BUZZER_DDR DDRB
#define BUZZER_PIN 4

#define CLOSE_DISTANCE 30

volatile uint64_t pulse_width = 0;
volatile bool pulse_sent = 0;

void my_delay_ms(int ms){
    while (0 < ms){  
        _delay_ms(1);
        --ms;
    }
}

void generate_tone(float frequency, float duration){
    int total_cycles = duration * frequency / 1000;

    for(int i = 0; i < total_cycles; i++){
        my_delay_ms(1000 / (2 * frequency));
        BUZZER_PORT = BUZZER_PORT | (1 << BUZZER_PIN);
        my_delay_ms(1000 / (2 * frequency));
        BUZZER_PORT = BUZZER_PORT & ~(1 << BUZZER_PIN);
    }
}

void trigger_pulse() {
    TRIG_PORT |= (1 << TRIG_PIN);
    _delay_us(10);
    TRIG_PORT &= ~(1 << TRIG_PIN);
}

void HCSR04_init() {
    TRIG_DDR |= (1 << TRIG_PIN);
    ECHO_DDR &= ~(1 << ECHO_PIN);
}

ISR(INT0_vect) {
    if (!pulse_sent) {
        TCCR1B |= 1 << CS10;
        pulse_sent = 1;
    } else {
        TCCR1B = 0;
        pulse_width = TCNT1;
        TCNT1 = 0;
        pulse_sent = 0;
    }
}

int main() {
    double distance = 0;
    BUZZER_DDR = BUZZER_DDR | (1 << BUZZER_PIN);
    HCSR04_init();
    EIMSK |= (1 << INT0);
    EICRA |= (1 << ISC00);
    TCNT1 = 0;
    sei();

    while(true) {
        trigger_pulse();
        distance = pulse_width * (340.26 * 100 * 0.5) / F_CPU;
        if (distance < CLOSE_DISTANCE) {
            generate_tone(50, 20);
        }
    }

    return 0;
}















