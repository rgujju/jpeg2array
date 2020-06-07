# jpeg2array

Convert jpeg image to C/C++ array for use with microcontrollers.

This tool creates a header file which needs to be included in your program.

This can be used with the tft library by bodmer
https://github.com/Bodmer/TFT_eSPI

## Usage

```cmd
./jpeg2array jpeg_filename [options]
Options:
--stm32         Generate array for stm32 (default)
--arduino       Generate array for arduino
```
