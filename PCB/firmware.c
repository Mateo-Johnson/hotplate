#include <stdint.h>
#include <math.h>
#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_timer.h"

// ---------------------------
// CONFIG
// ---------------------------
#define SPI_PORT        SPI1
#define TC_CS_PIN       GPIO_PIN_5
#define TRIAC_PIN       GPIO_PIN_10

#define LOOP_HZ         10.0f
#define DT              (1.0f / LOOP_HZ)

// PID constants (tune later)
static float Kp = 8.0f;
static float Ki = 0.6f;
static float Kd = 2.0f;

float pid_integral = 0.0f;
float pid_prev_err = 0.0f;

// ---------------------------
// READ THERMOCOUPLE
// MAX31855-style 32-bit word
// ---------------------------
float read_temp_c(void) {
    uint8_t rx[4] = {0};
    hal_gpio_write(TC_CS_PIN, 0);
    hal_spi_rx(SPI_PORT, rx, 4);
    hal_gpio_write(TC_CS_PIN, 1);

    uint32_t v = (rx[0] << 24) | (rx[1] << 16) | (rx[2] << 8) | rx[3];

    if (v & 0x00000007) {
        return NAN;  // Fault bit
    }

    int16_t temp_raw = (v >> 18) & 0x3FFF;
    if (temp_raw & 0x2000) {
        temp_raw |= 0xC000;
    }

    return temp_raw * 0.25f;  // °C
}

// ---------------------------
// SIMPLE PID
// ---------------------------
float pid_step(float set, float meas) {
    float error = set - meas;

    pid_integral += error * DT;
    float deriv = (error - pid_prev_err) / DT;

    pid_prev_err = error;

    float out = Kp * error + Ki * pid_integral + Kd * deriv;
    if (out < 0) out = 0;
    if (out > 1) out = 1;

    return out;
}

// ---------------------------
// TRIAC FIRING (phase angle)
// 0–1 PID output → phase delay
// Lower output = longer delay before firing
// ---------------------------
void triac_fire(float level) {
    float delay_ms = (1.0f - level) * 8.3f; // 8.3 ms → half-cycle at 60 Hz
    hal_timer_delay_ms(delay_ms);
    hal_gpio_write(TRIAC_PIN, 1);
    hal_timer_delay_us(100);    // 100 µs gate pulse
    hal_gpio_write(TRIAC_PIN, 0);
}

// ---------------------------
// MAIN LOOP
// ---------------------------
int main(void) {
    hal_spi_init(SPI_PORT, 1000000);
    hal_gpio_mode(TC_CS_PIN, GPIO_OUTPUT);
    hal_gpio_mode(TRIAC_PIN, GPIO_OUTPUT);
    hal_gpio_write(TRIAC_PIN, 0);

    float setpoint = 180.0f; // test target temp °C

    hal_timer_init();

    while (1) {
        float temp = read_temp_c();
        if (!isnan(temp)) {
            float duty = pid_step(setpoint, temp);
            triac_fire(duty);
        }

        hal_timer_delay_ms((int)(DT * 1000));  // maintain loop rate
    }
}
