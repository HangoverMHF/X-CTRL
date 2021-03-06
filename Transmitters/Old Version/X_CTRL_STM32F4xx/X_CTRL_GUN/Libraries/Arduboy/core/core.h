#ifndef ArduboyCore_h
#define ArduboyCore_h

#include "Arduino.h"
#include "EEPROM.h"
#include "SysConfig.h"

#define PIN_NULL PD0

#define RED_LED PIN_NULL
#define GREEN_LED PIN_NULL
#define BLUE_LED PIN_NULL

// pin values for buttons, probably shouldn't use these

// bit values for button states
#define LEFT_BUTTON _BV(5)
#define RIGHT_BUTTON _BV(6)
#define UP_BUTTON _BV(7)
#define DOWN_BUTTON _BV(4)
#define A_BUTTON _BV(3)
#define B_BUTTON _BV(2)

#define PIN_SPEAKER_1 Buzz_Pin
#define PIN_SPEAKER_2 PIN_NULL

#define WIDTH 128
#define HEIGHT 64

#define INVERT 2 //< lit/unlit pixel
#define WHITE 1 //< lit pixel
#define BLACK 0 //< unlit pixel

class ArduboyCore //: public TFT_SSD1283A
{
public:
    ArduboyCore();

    /// allows the CPU to idle between frames
    /**
     * This puts the CPU in "Idle" sleep mode.  You should call this as often
     * as you can for the best power savings.  The timer 0 overflow interrupt
     * will wake up the chip every 1ms - so even at 60 FPS a well written
     * app should be able to sleep maybe half the time in between rendering
     * it's own frames.
     * 
     * See the Arduboy class nextFrame() for an example of how to use idle()
     * in a frame loop.
     */
    void idle();

    void LCDDataMode(); //< put the display in data mode

    /// put the display in command mode
    /**
     * See SSD1306 documents for available commands and command sequences.
     * 
     * Links:
     * - https://www.adafruit.com/datasheets/SSD1306.pdf
     * - http://www.eimodule.com/download/SSD1306-OLED-Controller.pdf
     */
    void LCDCommandMode();

    uint8_t width();    //< return display width
    uint8_t height();   // < return display height

    /// get current state of all buttons (bitmask)
    /**
     * Bit mask that is returned:
     *
     *           Hi   Low   
     *  DevKit   00000000    - reserved                         
     *           -DLU-RAB    D down
     *                       U up       
     *  1.0      00000000    L left
     *           URLDAB--    R right
     * 
     * Of course you shouldn't worry about bits (they may change with future
     * hardware revisions) and should instead use the button defines:
     * LEFT_BUTTON, A_BUTTON, UP_BUTTON, etc.
     */

    uint8_t getInput(); 
	uint8_t buttonsState();

    // paints 8 pixels (vertically) from a single byte
    //  - 1 is lit, 0 is unlit
    //
    // NOTE: You probably wouldn't actually use this, you'd build something
    // higher level that does it's own calls to SPI.transfer().  It's
    // included for completeness since it seems there should be some very
    // rudimentary low-level draw function in the core that supports the
    // minimum unit that the hardware allows (which is a strip of 8 pixels)
    //
    // This routine starts in the top left and then across the screen.
    // After each "page" (row) of 8 pixels is drawn it will shift down
    // to start drawing the next page.  To paint the full screen you call
    // this function 1,024 times.
    //
    // Example:
    //
    // X = painted pixels, . = unpainted
    //
    // blank()                      paint8Pixels() 0xFF, 0, 0x0F, 0, 0xF0
    // v TOP LEFT corner (8x9)      v TOP LEFT corner
    // ........ (page 1)            X...X... (page 1)
    // ........                     X...X...
    // ........                     X...X...
    // ........                     X...X...
    // ........                     X.X.....
    // ........                     X.X.....
    // ........                     X.X.....
    // ........ (end of page 1)     X.X..... (end of page 1)
    // ........ (page 2)            ........ (page 2)
    void paint8Pixels(uint8_t pixels);

    /// paints an entire image directly to hardware (from PROGMEM)
    /*
     * Each byte will be 8 vertical pixels, painted in the same order as
     * explained above in paint8Pixels.
     */
    void paintScreen(const unsigned char *image);

    /// paints an entire image directly to hardware (from RAM)
    /*
     * Each byte will be 8 vertical pixels, painted in the same order as
     * explained above in paint8Pixels.
     */
    void paintScreen(unsigned char image[]);

    /// paints a blank (black) screen to hardware
    void blank();

    /// invert the display or set to normal
    /**
     * when inverted, a pixel set to 0 will be on
     */
    void invert(boolean inverse);

    /// turn all display pixels on, or display the buffer contents
    /**
     * when set to all pixels on, the display buffer will be
     * ignored but not altered
     */
    void static allPixelsOn(boolean on);

    /// flip the display vertically or set to normal
    void static flipVertical(boolean flipped);

    /// flip the display horizontally or set to normal
    void static flipHorizontal(boolean flipped);

    /// send a single byte command to the OLED
    void static sendLCDCommand(uint8_t command);

    /// set the light output of the RGB LEB
    void setRGBled(uint8_t red, uint8_t green, uint8_t blue);

protected:
    /// boots the hardware
    /**
     * - sets input/output/pullup mode for pins
     * - powers up the OLED screen and initializes it properly
     * - sets up power saving
     * - kicks CPU down to 8Mhz if needed
     * - allows Safe mode to be entered
     */
    void boot();

    /// Safe mode
    /**
     * Safe Mode is engaged by holding down both the LEFT button and UP button
     * when plugging the device into USB.  It puts your device into a tight
     * loop and allows it to be reprogrammed even if you have uploaded a very
     * broken sketch that interferes with the normal USB triggered auto-reboot
     * functionality of the device.
     * 
     * This is most useful on Devkits because they lack a built-in reset
     * button.
     */
    void static inline safeMode();

    // internals
    void bootLCD();
    void static inline bootPins();
    void static inline slowCPU();
    void static inline saveMuchPower();
};

#endif
