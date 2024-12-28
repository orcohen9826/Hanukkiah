# Hanukkah LED Menorah (Hanukiah)



This project is a Wi-Fi-controlled LED Hanukiah (Menorah) for Chanukah, featuring a custom flickering flame effect. The user can select the day via a web interface, and the corresponding number of "candles" (LED segments) light up. The project uses the NeoPixel library and includes STL files for 3D printing all parts needed for assembly.
![Menorah Overview](images/pic.jpg)
## Features

- **Wi-Fi Access Point**: A built-in web server allows you to connect and select the day of Hanukkah to light.
- **Day Selection**: Choose any day from 1–8. Each day lights up that many LED "candles," plus the shamash (helper lamp).
- **Fire/Flame Effect**: Each LED flickers with a random red/orange glow to mimic real flames.
- **Adafruit NeoPixel**: Uses the NeoPixel library to control WS2812/WS2812B LED strips.

## Hardware

- **Controller**: Wemos D1 mini (ESP8266-based).
- **LED Strip**: WS2812B or compatible, controlled via NeoPixel.
- **3D-Printed Parts**: Includes STL files for all components required to build the Menorah. Some parts are inspired by [Thingiverse Thing #604228](https://www.thingiverse.com/thing:604228) and modified for this project.

## Platform & Tools

- **PlatformIO** for building and uploading the code.
- **Arduino Framework**.
- **Adafruit NeoPixel** library for LED animations.

## Instructions for Assembly

### LED Wiring

1. **Prepare the LEDs**:
   - Assign **3 LEDs for the shamash (main candle)**.
   - Assign **2 LEDs for each of the other candles** (1–8).

2. **Connect Power**:
   - Connect the **ground (GND)** wires of all LEDs together.
   - Connect the **5V wires** of all LEDs together.
   - Create a single connection point from these 5V and GND wires to a **separate external power source** (do not power the LEDs directly from the controller, as it may burn out).
   - Also connect the external power source’s GND and 5V lines to the corresponding pins on the controller.

3. **Connect Data Lines**:
   - Connect the **data-in pin** of the first LED in the first candle to the **D1 pin** on the Wemos D1 mini.
   - Chain the LEDs by connecting the **data-out pin** of one LED to the **data-in pin** of the next LED, continuing for all candles in sequence.

### Code Upload

1. **Install PlatformIO**:
   - Set up the PlatformIO environment with the Arduino framework.
   - Install the Adafruit NeoPixel library.

2. **Upload the Code**:
   - Load the provided code into PlatformIO.
   - Configure Wi-Fi credentials and LED settings if necessary.
   - Upload the code to the Wemos D1 mini.


## How It Works

1. **Power on** the Wemos D1 mini.
2. The device creates a **Wi-Fi Access Point** (default SSID and password can be configured in the code).
3. Connect to the AP and navigate to the IP address (e.g., `192.168.4.1`) in your browser.
4. **Select a day** (1–8) to light up that many LED candles, plus the shamash. Selecting "Off" (0) turns all LEDs off.
5. Watch the flickering flame effect as the LEDs light up in sequence.

## Notes

- This project leverages the **Adafruit NeoPixel** library for precise LED control.
- The **flickering flame effect** is achieved by generating random red/orange color values for each LED.
- Includes STL files for all 3D-printed parts required for the physical Menorah.
- All code and instructions are provided as-is, and you can customize them for your setup.

Enjoy your new **LED Hanukiah** and Chag Sameach!

