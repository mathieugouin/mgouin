/*************************************************************************
* Compiler :  CCS C - 16F876
* Programmer :  Mathieu Gouin
* Description : A program to control model trains.
*
*************************************************************************/

/************************************************************************
* Description : Template
* Parameters :  void
* Return :      void
************************************************************************/

/************************************************************************/
/*                      PRE-PROCESSOR DIRECTIVES                        */
/************************************************************************/

#case // Case-sensitive compiler

#include <16F876.H>   // General library
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>

#include "common.h"
#include "eeprom.h"
#include "i2c_lcd.h"
#include "buffer.h"

//#include "dcc.h"
//#include "test.c"

/* The PCM/PCW compilers now allow for 16 bit pointers for parts with
   RAM above 255.  Use a #device statement like the following to use
   16 bit pointers.  When 8 bit pointers are used the compiler will no
   longer use RAM above 255 except with #byte or read/write _ bank.
*/
//#device *=16

/* Configures the read_adc return size. For example, using
a PIC with a 10 bit A/D you can use 8 or 10 for xx- 8 will
return the most significant uint8_t, 10 will return the full A/D
reading of 10 bits. */
#device ADC=10

// Fuses : High Speed, NO WatchDog Timer, PowerUp Timer,
// NO code PROTECTion, NO Low Voltage Programmation.
#fuses HS,NOWDT,PUT,NOPROTECT,NOLVP

#use delay(clock=20000000)    // 20 MHz Clock

/************************************************************************/
/*                          TYPE DEFINITIONS                            */
/************************************************************************/

/************************************************************************/
/*                         MACROS DEFINITIONS                           */
/************************************************************************/

#define LED_PAUSE               (uint8_t)100   // time in ms to wait ON and OFF
#define BOOT_NB_ADDRESS         (uint8_t)0x00  // where the boot number is stored in EEPROM
#define DELAY_MESSAGE           2000           // delay in ms for the lcd message

// I2C IO Expander PCF8574 #1 address pins: A2:0, A1:0, A0:0
#define I2C_IO_EXP_1_R          (uint8_t)0x41  // Read
#define I2C_IO_EXP_1_W          (uint8_t)0x40  // Write

// I2C IO Expander PCF8574 #2 address pins: A2:0, A1:0, A0:1
#define I2C_IO_EXP_2_R          (uint8_t)0x43  // Read
#define I2C_IO_EXP_2_W          (uint8_t)0x42  // Write

// Symbolic names
// I2C LCD connected to IO Expander #1
#define I2C_LCD_R               I2C_IO_EXP_1_R
#define I2C_LCD_W               I2C_IO_EXP_1_W

// General Purpose I/O connected to IO Expander #2
#define I2C_GPIO_R              I2C_IO_EXP_2_R
#define I2C_GPIO_W              I2C_IO_EXP_2_W

// Microchip: 24FC512 I2C 64K x 8bit EEPROM
// Address pins: A2:0, A1:0, A0:0
#define I2C_EEPROM_R            (uint8_t)0xA1 // Read
#define I2C_EEPROM_W            (uint8_t)0xA0 // Write

// See w/ DIRECTION_PIN
#define DIRECTION_FWD           1
#define DIRECTION_REV           0

// Speed command read from ADC input
#define ADC_SAMPLING_RATE       50  /* ms */

/************************************************************************/
/*                         PINS DEFINITIONS                             */
/************************************************************************/
#define LED_PIN     PIN_A5      // On-board test LED
#define BUTTON_PIN  PIN_B0      // Push button : normally open = HIGH
                                // pressed = low

// Serial 1 (S1) : Hardware UART
#define S1_RX       PIN_C7      // PIC <-- DB-9
#define S1_TX       PIN_C6      // PIC --> DB-9

// I2C Port
#define I2C_SCL     PIN_C3      // Serial Clock
#define I2C_SDA     PIN_C4      // Serial Data

// Motor Control
#define PWM_PIN         PIN_C2  // CCP1 PWM Output
#define DIRECTION_PIN   PIN_C0  // H-Bridge direction input

/************************************************************************/
/*                          PORTS DEFINITIONS                           */
/************************************************************************/

// Serial Port #1 using RS-232 to computer @ 9600 baud
#define SERIAL1_PORT    #use rs232(baud=9600, xmit=S1_TX, rcv=S1_RX, bits=8, parity=N)

// I2C port using hardware I2C.
#define I2C_PORT        #use i2c(MASTER, SCL=I2C_SCL, SDA=I2C_SDA, FAST)

/************************************************************************/
/*                          FUNCTION PROTOTYPES                         */
/************************************************************************/

void master_init(void);

int16_t low_pass(int16_t x);
int16_t normalizer_dead_band(int16_t x);
int16_t kick_start(int16_t x);

uint16_t convert_speed_step(uint8_t speed);

void toggle_led(void);
void flash_led(uint8_t x);

void start_adc_conversion(void);
int16_t read_adc_conversion(void);

uint8_t handle_boot_nb(void);

// Serial port 1
void put_serial1(uint8_t c);
uint8_t get_serial1(void);

void i2c_leds(uint8_t leds_on);

/************************************************************************/
/*                            GLOBAL VARIABLES                          */
/************************************************************************/
static bool g_new_adc = false;
static int16_t g_adc_value = 0;
static int16_t g_speed_command = 0;

static uint16_t g_timebase_counter_lsw = 0; // Least Significant Word
static uint16_t g_timebase_counter_msw = 0; // Most Significant Word

static uint8_t g_last_serial_byte = 0;
static bool g_new_serial_byte = false;

static bufferData_t   g_serial_buffer_memory[8];
static bufferStruct_t g_serial_buffer;

/************************************************************************/
/*                              ISR                                     */
/************************************************************************/

// Interrupt when received data is available
#INT_RDA
void rs232_rx_isr(void)
{
  g_last_serial_byte = get_serial1();
  g_new_serial_byte = true;
  //bufferPut(&g_serial_buffer, g_last_serial_byte);
  toggle_led();
}

// Interrupt when ADC conversion is complete
#INT_AD
void adc_done_isr(void)
{
#if 1
  g_adc_value = read_adc_conversion();                        // [0, 1023]
  g_speed_command = g_adc_value;
  // Processing
  g_speed_command = low_pass(g_speed_command);                // [0, 1023]
  g_speed_command = normalizer_dead_band(g_speed_command);    // [-1023, +1023]
  //g_speed_command = kick_start(g_speed_command);              // [-1023, +1023]

#else
  g_speed_command =
    kick_start(                     // [-1023, +1023]
      normalizer_dead_band(         // [-1023, +1023]
        low_pass(                   // [0, 1023]
          g_adc_value = read_adc_conversion()))); // [0, 1023]
#endif

  // CCP1
  set_pwm1_duty((uint16_t)ABS(g_speed_command));
  g_new_adc = true;
  //toggle_led();
}

// Interrupt when Timer0 overflows (also referd as RTCC)
#INT_RTCC
void timer0_isr(void)
{
  ++g_timebase_counter_lsw;
  if (!g_timebase_counter_lsw)
  {
    ++g_timebase_counter_msw;
  }
}

#if 0 // not used for now

#INT_TIMER1
void timer1_isr(void)
{
  toggle_led();
}

#INT_TIMER2
void timer2_isr(void)
{
  toggle_led();
}

#INT_CCP1
void ccp1_isr(void)
{
  toggle_led();
}

#INT_CCP2
void ccp2_isr(void)
{
  toggle_led();
}

#endif // 0

/************************************************************************/
/*                              MAIN                                    */
/************************************************************************/
void main(void)
{
  uint8_t leds = 0;
  uint8_t i = 0;
  uint8_t c = 0;
  
  master_init();

  //flash_led(5);   // reality check at boot

  // 0         1     
  // 0123456789012345
  // Boot:4
  // Date:19-FEB-15 
  
  i2c_lcd_init();
  printf(i2c_lcd_write_data, "Boot:%u", handle_boot_nb());
  i2c_lcd_position_cursor(1, 0);
  i2c_lcd_write_data("Date:");
  i2c_lcd_write_data(__date__);
  delay_ms(DELAY_MESSAGE);
  i2c_lcd_clear();

#if 1
  i2c_lcd_position_cursor(0, 0);
  i2c_lcd_write_data("A0");
  i2c_lcd_position_cursor(1, 0);
  i2c_lcd_write_data("SP");

  //convert_speed_step(0);
  
  while (input(BUTTON_PIN))
  {
    // Wait for new ADC reading flag
    while (!g_new_adc)
    {
    }
    g_new_adc = false;
    //toggle_led();

    if (g_speed_command > 0)
    {
      leds = 0b00000010; // Forward
      output_bit(DIRECTION_PIN, DIRECTION_FWD);
    }
    else if (g_speed_command < 0)
    {
      leds = 0b00000001; // Reverse
      output_bit(DIRECTION_PIN, DIRECTION_REV);
    }
    else
    {
      leds = 0b00000000; // Stop
      // No change: output_bit(DIRECTION_PIN, DIRECTION_REV);
    }

    i2c_lcd_position_cursor(0, 3);
    printf(i2c_lcd_write_data, "%04ld %02X %5lu", g_adc_value, g_last_serial_byte, g_timebase_counter_msw);

    i2c_lcd_position_cursor(1, 2);
    if (g_speed_command < 0)
    {
      i2c_lcd_write_data('-');
    }
    else
    {
      i2c_lcd_write_data(' ');
    }
    printf(i2c_lcd_write_data, "%4ld    %5lu", ABS(g_speed_command), g_timebase_counter_lsw);

    i2c_leds(leds);
  }
#else
  while (input(BUTTON_PIN))
  {
    // Wait for new serial data flag
  #if 1
    while (!g_new_serial_byte)
    {
    }
    c = g_last_serial_byte;
    g_new_serial_byte = false;
  #else
    while (bufferGet(&g_serial_buffer, &c) != BUFFER_OK)
    {
    }
  #endif
    
    i2c_lcd_position_cursor((i >> 3) % 2, (i % 8) << 1);
    printf(i2c_lcd_write_data, "%02X*", c);
    
    ++i;
  }
#endif

  // Shutdown sequence
  disable_interrupts(INT_ADC); // Interrupt on ADC complete
  delay_ms(100); // at least twice the original timer rate
  set_pwm1_duty(0);
  output_bit(DIRECTION_PIN, DIRECTION_FWD);
  i2c_lcd_clear();
  output_low(LED_PIN);
  i2c_leds(0b00000000);

  sleep();
} // main()

/************************************************************************/
/*                          FUNCTION DEFINITIONS                        */
/************************************************************************/

/**
 * Initialization of the PIC
 */
void master_init(void)
{
  output_low(PWM_PIN);
  output_bit(DIRECTION_PIN, DIRECTION_FWD);

  bufferInit(&g_serial_buffer, &(g_serial_buffer_memory[0]), ARRAY_SIZE(g_serial_buffer_memory));
  
  // ----------------------------------------------------------
  // PWM Setup uses CCP1 & Timer2
  setup_ccp1(CCP_PWM);   // Configure CCP1 as a PWM, must use Timer2 as PWM timebase
  // Ftimer = Fc / 4 / Divider / (period + 1)
  set_timer2(0);
  // Slowest interrupt is at 76.29 Hz
  // setup_timer_2(prescale, period, postscale)
  setup_timer_2(T2_DIV_BY_16, 255, 16); // 1.221 kHz
  //setup_timer_2(T2_DIV_BY_16, 19, 16); // 15.63 kHz
  //enable_interrupts(INT_TIMER2);
  // Start OFF
  set_pwm1_duty(0);

  // ----------------------------------------------------------
  // ADC Setup
  setup_adc_ports(ALL_ANALOG); // RA0_ANALOG
  setup_adc(ADC_CLOCK_INTERNAL);
  set_adc_channel(0);
  // TBD: read at least one sample  
  //read_adc();
  enable_interrupts(INT_ADC); // Interrupt on ADC complete

  // ----------------------------------------------------------
  // Timer1 Setup
  set_timer1(0);
  // Ftimer = Fc / 4 / Divider
  setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
  //enable_interrupts(INT_TIMER1);

  // ----------------------------------------------------------
  // CCP2 Setup (configured as comparator with Timer1)
  // Fccp = Ftimer / (CCP + 1)
  // Fccp = 20 Hz (50 ms)
  // Be sure to keep in sync #define ADC_SAMPLING_RATE
  CCP_2 = 31249;
  // Compare mode + special event: reset timer & start A/D conversion automatically
  setup_ccp2(CCP_COMPARE_RESET_TIMER);
  //enable_interrupts(INT_CCP2);
  
  // ----------------------------------------------------------
  // Timer0: timebase
  // Overflow frequency = Fosc/4 / Prescaler / 256
  // For Prescaler = 256, F = 76.3Hz ; T = 13.1ms
  set_timer0(0);
  setup_counters(RTCC_INTERNAL, RTCC_DIV_256);
  enable_interrupts(INT_RTCC);
  
  // ----------------------------------------------------------
  // RS232
  enable_interrupts(INT_RDA);

  enable_interrupts(GLOBAL);
}

/************************************************************************
* Description : First Order Low Pass Filter
*               k = Ts / (Ts + Tau)
*               Ts = 50ms
*               Tau = 0.6
* http://en.wikipedia.org/wiki/Low-pass_filter#Discrete-time_realization
* Parameters :  input unfiltered
* Return :      output filtered
************************************************************************/
#define LOW_PASS_CONSTANT 0.076923
int16_t low_pass(int16_t x)
{
#if 1
  static float old_data = 0.0;
  old_data = LOW_PASS_CONSTANT * x + (1 - LOW_PASS_CONSTANT) * old_data;
  return old_data;
#else
  static int16_t old_data = 0;
  old_data = (x * 3) / 11 + old_data * 10 / 11;
  return old_data / 3;
#endif
}

/************************************************************************
* Description : Convert 0-1023 ADC input to -1023 to 1023 with a
*               dead band in the center.
*               The equivalent output in volts at the start of the 
*               dead-band is: 2*DEAD_BAND / ADC_MAX_VALUE * VoltMax
* Parameters :  input
* Return :      output normalized
************************************************************************/
#define DEAD_BAND 80 // starts at about 1.875V for a full 12V
#define ADC_MAX_VALUE 1024
int16_t normalizer_dead_band(int16_t x)
{
  int16_t out = 0;
  if ((x < ADC_MAX_VALUE / 2 - DEAD_BAND) ||
      (x > ADC_MAX_VALUE / 2 + DEAD_BAND))
  {
    out = 2 * x - ADC_MAX_VALUE + 1;
  }
  return out;
}

/************************************************************************
* Description : Generates a kick start PWM value when starting from 0
*               to non-zero input value.
* Parameters :  input
* Return :      output with kick start
************************************************************************/
#define KICK_START_VALUE        800 // Max = 1023
#define KICK_START_DURATION     (100 / ADC_SAMPLING_RATE)  // 100 ms

#if (KICK_START_DURATION > 255)
  #error "kick_start counter variable must be uint16_t"
#endif

int16_t kick_start(int16_t x)
{
  int16_t out = 0;
  static int16_t old_data = 1;  // will not generate a kick start if ADC != 0 at power-up
  static uint8_t counter = 0;

  if (old_data == 0 && x != 0) // 0 to non-zero transition detection
  {
    // Initiate kick start
    counter = KICK_START_DURATION;
  }

  out = x; // default, assume not in kick start mode
  if (counter > 0)
  {
    counter--;

    //out = SIGN(x) * KICK_START_VALUE;
    out = x >= 0 ? KICK_START_VALUE : -KICK_START_VALUE;
  }

  old_data = x;
  return out;
}

/************************************************************************
* Description : Convert speed step curve.
* Parameters :  
* Return :      
************************************************************************/
#define SPEED_STEP_MAX 16
#define SPEED_CMD_MAX 1023
#define SPEED_CMD_MIN 256 // about 3V
uint16_t convert_speed_step(uint8_t speed)
{
  // a*x + b
  return ((SPEED_CMD_MAX - SPEED_CMD_MIN)/(float)SPEED_STEP_MAX) * speed + SPEED_CMD_MIN;
}


/************************************************************************
* Description : Read EEPROM, increment value, write to EEPROM.
*
* Parameters :  void
* Return :      bood_nb, the value of the boot number currently stored
*               in eeprom.
************************************************************************/
uint8_t handle_boot_nb(void)
{
    uint8_t boot_nb;
    boot_nb = i2c_read_eeprom(BOOT_NB_ADDRESS);

    i2c_write_eeprom(BOOT_NB_ADDRESS, boot_nb + 1);
    return boot_nb;
}

/************************************************************************
* Description : Toggle the LED.
* Parameters :  void
* Return :      void
************************************************************************/
void toggle_led(void)
{
  static bool led_toggle = 1;
  output_bit(LED_PIN, led_toggle);
  led_toggle = !led_toggle;
}

/************************************************************************
* Description : Flash LED x number of times.
* Parameters :  x, the number of times the led will flash
* Return :      void
************************************************************************/
void flash_led(uint8_t x)
{
  uint8_t i;
  for (i = 0; i < x; ++i)
  {
    output_high(LED_PIN);
    delay_ms(LED_PAUSE);
    output_low(LED_PIN);
    delay_ms(LED_PAUSE);
  }
}

/************************************************************************
* Description : This function starts the ADC conversion only.
*               It does not wait for the conversion to complete.
* Parameters :  void
* Return :      void
************************************************************************/
/*
#bit ADCON0_GO_DONE = 0x1F.2
void start_adc_conversion(void)
{
  ADCON0_GO_DONE = 1;
}
*/

/************************************************************************
* Description : This function reads the ADC conversion result immediately.
* Parameters :  void
* Return :      ADC value range [0, 1023].
************************************************************************/
#byte ADRESH = 0x1E
#byte ADRESL = 0x9E
int16_t read_adc_conversion(void)
{
  uint16_t result;
  result = (ADRESH & 0x03) << 8;
  result |= ADRESL;
  // Safe to cast 10bits unsigned to 16 bits signed
  return (int16_t)result;
}

/* -------------------- DCC -------------------- */
// Test only...
//#include "dcc.c"

/* -------------------- BUFFER -------------------- */

#include "buffer.c"

/* -------------------- I2C SERIAL PORT -------------------- */

// Definition of the I2C port settings.
I2C_PORT

#include "eeprom.c"
#include "i2c_lcd.c"

/************************************************************************
* Description : Turns ON or OFF the blue LED's on 1st PCF8574
* Parameters :  leds_on
* Return :      void
************************************************************************/
void i2c_leds(uint8_t leds_on)
{
  i2c_start();
  i2c_write(I2C_GPIO_W);
  i2c_write(~leds_on); // All pins high, low to ON a led.
  i2c_stop();
}

/* -------------------- SERIAL PORT 1 -------------------- */
// Functions using serial port 1 connected to the computer

#if 1

// define serial port 1 settings
SERIAL1_PORT

bool kbhit_serial1(void)
{
    return kbhit();
}

// Put a single char to serial port 1
void put_serial1(uint8_t c)
{
    putchar(c);
}

// Get a single char from serial port 1
uint8_t get_serial1(void)
{
    return getchar();
}

#endif // 1

