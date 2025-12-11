#include "stm32f4xx.h"

//
/* SHARED RESOURCES                               */
//

// --- Global Variables & Constants ---
int adcData; // For LDR sensor reading
const int LDR_THRESHOLD = 100; // Threshold for darkness. Adjust based on your LDR's sensitivity (e.g., 1000-3000).

// --- Delay Functions (Standardized) ---
/**
 * @brief Provides a simple microsecond delay.
 * @note This is approximate and depends on the system clock frequency.
 * @param us Microseconds to delay.
 */
void delay_us(uint32_t us) {
    // Loop iterations calibrated roughly for the default STM32F4 clock speed.
    for (uint32_t i = 0; i < us * 8; i++) {
        __NOP(); // No operation, just wastes a clock cycle.
    }
}

/**
 * @brief Provides a simple millisecond delay.
 * @param ms Milliseconds to delay.
 */
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        delay_us(1000); // 1000 microseconds = 1 millisecond
    }
}

// --- Buzzer Function (Shared by Button and Ultrasonic modules) ---
/**
 * @brief Generates a tone on the buzzer pin (PA5).
 * @param frequency The frequency of the tone in Hz.
 * @param duration_ms The duration of the tone in milliseconds.
 */
void Buzzer_Beep(uint32_t frequency, uint32_t duration_ms) {
    if (frequency == 0) return;
    uint32_t period_us = 1000000 / frequency; // Time for one full wave
    uint32_t half_period_us = period_us / 2;   // Time for high/low state
    uint32_t cycles = (frequency * duration_ms) / 1000; // Total number of beeps

    for (uint32_t i = 0; i < cycles; i++) {
        GPIOA->ODR |= (1 << 5);     // Set PA5 (Buzzer) HIGH
        delay_us(half_period_us);
        GPIOA->ODR &= ~(1 << 5);    // Set PA5 (Buzzer) LOW
        delay_us(half_period_us);
    }
}


//
/* ULTRASONIC SENSOR MODULE                           */
//

/**
 * @brief Sends a 10us trigger pulse to the ultrasonic sensor on PA6.
 */
void Ultrasonic_Send_Trigger(void) {
    GPIOA->ODR |= (1 << 6);     // Set PA6 HIGH
    delay_us(10);
    GPIOA->ODR &= ~(1 << 6);    // Set PA6 LOW
}

/**
 * @brief Measures the echo pulse and calculates the distance in centimeters.
 * @return Distance in cm.
 */
uint32_t Ultrasonic_Get_Distance(void) {
    uint32_t time = 0;
    
    Ultrasonic_Send_Trigger();
    
    // Wait for the echo pin (PA1) to go HIGH
    while (!(GPIOA->IDR & (1 << 1)));
    
    // Measure the duration of the HIGH pulse
    while (GPIOA->IDR & (1 << 1)) {
        time++;
        delay_us(1);
        if (time > 30000) break; // Timeout to prevent infinite loop
    }
    
    // Distance (cm) = (time_in_us * speed_of_sound_cm_per_us) / 2
    // Speed of sound is ~0.0343 cm/us. The formula simplifies to time / 58.
    return time / 58;
}


//
/* SYSTEM INITIALIZATION                           */
//

void System_Init(void) {
    // --- 1. Enable Peripheral Clocks ---
    RCC->AHB1ENR |= (1U << 0); // Enable GPIOA clock
    RCC->APB2ENR |= (1U << 8); // Enable ADC1 clock
    
    // --- 2. Configure GPIO Pins ---
    // PA0: Emergency Button (Input with Pull-up, connected to on-board switch)
    GPIOA->MODER &= ~(3U << 0); // Set PA0 as input
    GPIOA->PUPDR |= (1U << 0);  // Enable pull-up resistor for PA0
    
    // PA1: Ultrasonic Echo (Input, default state is input)
    
    // PA2: Emergency LED (Output)
    GPIOA->MODER |= (1U << 4);
    
    // PA3: LDR LED (Output)
    GPIOA->MODER |= (1U << 6);
    
    // PA4: LDR Sensor (Analog Input)
    GPIOA->MODER |= (3U << 8);
    
    // PA5: Shared Buzzer (Output)
    GPIOA->MODER |= (1U << 10);
    
    // PA6: Ultrasonic Trigger (Output) -> NEW PIN ASSIGNMENT
    GPIOA->MODER |= (1U << 12);

    // --- 3. Configure ADC1 for LDR on Channel 4 (PA4) ---
    ADC1->SMPR2 |= (3U << 12);  // Set sampling time for channel 4
    ADC1->CR1 = 0;              // Reset CR1
    ADC1->CR2 = 0;              // Reset CR2
    ADC1->CR2 |= (1U << 1);     // Enable Continuous Conversion mode
    ADC1->SQR3 = 4;             // Select channel 4 as the first conversion
    ADC1->SQR1 = 0;             // Set sequence length to 1 conversion
    ADC1->CR2 |= (1U << 0);     // Enable ADC
    
    // Wait for ADC to stabilize
    delay_ms(10);
}


//
/* MAIN PROGRAM                                */
//

int main(void) {
    System_Init();
    
    // Start the ADC conversion process
    ADC1->CR2 |= (1U << 30);
    
    while(1) {
        //
        /* Module 1: LDR Light Sensor          */
        //
        // Wait for ADC conversion to complete
        while (!(ADC1->SR & (1U << 1)));
        adcData = ADC1->DR; // Read the value
        
        // If it's dark (LDR resistance is high, so voltage divider gives high ADC value)
        if (adcData > LDR_THRESHOLD) {
            // Blink the LDR indicator LED (PA3)
            GPIOA->ODR |= (1U << 3);
            delay_ms(100);
            GPIOA->ODR &= ~(1U << 3);
            delay_ms(100);
        } else {
            // Keep the LED off in light
            GPIOA->ODR &= ~(1U << 3);
        }
        
        //
        /* Module 2: Ultrasonic Obstacle         */
        //
        uint32_t distance = Ultrasonic_Get_Distance();
        
        // If an object is detected within 2cm to 20cm range
        if (!(distance < 5 && distance > 2)) {
            // Beep the buzzer to indicate proximity
            Buzzer_Beep(2000, 200);
            delay_ms(100); // Pause between beeps
        }
        
        //
        /* Module 3: Emergency Button         */
        //
        // Check if the button on PA0 is pressed (pin goes LOW)
        if (!(GPIOA->IDR & (1U << 0))) {
            // Turn on the emergency indicator LED (PA2)
            GPIOA->ODR |= (1U << 2);
            // Sound the emergency alarm
            Buzzer_Beep(2800, 100);
            delay_ms(100); // Pause between beeps
        } else {
            // Turn off the emergency LED when the button is released
            GPIOA->ODR &= ~(1U << 2);
        }
        
        // A small delay for overall system stability and button debouncing
        delay_ms(10);
    }
}