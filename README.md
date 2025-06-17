# SquishyCat

SquishyCat is a fun RGB LED project featuring a squishy silicone cat with vibrant RGB lighting effects.

## Features

- Soft silicone cat shell
- Animated RGB lighting with tab and squish detection
- Meshing between cats for synchronous RGB glory
- Controlled via a custom-designed PCB

## Hardware Requirements

- Squishy silicone cat silicone
- Custom PCB (will be uploaded when final)
- Lipo or 3 AAA battery 

## Software Build Instructions (PlatformIO)

This project uses PlatformIO for managing dependencies and building the firmware.

1.  **Install PlatformIO Core:** If you don't have PlatformIO installed, follow the instructions on the [PlatformIO website](https://platformio.org/install). It's recommended to install it as a VS Code extension for an integrated development experience.
2.  **Open the Project:** Open the `SquishyCat` project folder in your IDE (e.g., VS Code with PlatformIO extension).
3.  **Build Firmware:** Use the PlatformIO "Build" task or run `platformio run` in your terminal from the project root. This will compile the firmware.
4.  **Upload Firmware:** Connect your custom PCB to your computer and use the PlatformIO "Upload" task or run `platformio run --target upload` to flash the compiled firmware to the microcontroller.

## License

This project is licensed under the GNU General Public License v3.0. See the `LICENSE` file for more details.
