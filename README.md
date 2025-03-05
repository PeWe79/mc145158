# MC145158 PLL Controller Library

## Description

A library for Arduino that provides functions to configure and operate the MC145158 PLL integrated circuit. This library enables control over various aspects of the PLL's operation.

## Features

- **Arduino Compatibility**: Works with most Arduino boards.
- **PLL Control Functions**: Functions to set frequency shifts, data configuration, and more.
- **Configuration**: Easy-to-use API for setting up pins and operating modes.
- **DIP Switch Integration**: Support for configuring pins via DIP switches.

## Requirements

- **Arduino IDE**: To install the library.
- **Hardware**: MC145158 PLL circuit integrated into your project.
- **Board Compatibility**: Most Arduino-compatible boards supported.

## Installation

To install the library:

1. Download the repository from GitHub:
   [https://github.com/junon10/mc145158](https://github.com/junon10/mc145158)

2. In Arduino IDE, go to Sketch > Include Library > Add ZIP File...

3. Select the downloaded zip file.

## Usage

```c++
#include <MC145158.h>

// Declare an instance of the MC145158 PLL
MC145158 pll;

// Configure the PLL pins
pll.begin(clock_pin, data_pin, dlen_pin);

// DIP Switch configuration example (b7 to b0)
pll.setDipSwPinout(b7, b6, b5, b4, b3, b2, b1, b0);

// Configure the operating frequency
pll.setFrequency(108.0); // Frequency in MHz
// or
pll.setFrequencyByDipSw();

// Commit changes
pll.commitConfig();
```

## Limitations

- Warning: Code tested with logic analyzer, not yet tested in hardware!
- For detailed technical specifications, refer to the MC145158 datasheet.
- Function parameters and return values may vary based on hardware configuration.

## Author

- **Junon M.**  
  Contact: [junon10@tutamail.com](mailto:junon10@tutamail.com)

## Contributing

Contributions are welcome! Please fork the repository and send a pull request.

## Repository

- [https://github.com/junon10/mc145158](https://github.com/junon10/mc145158)

## References

- MC145158 Datasheet
- Arduino Documentation

## Changelog

- **v1.0.0 (2024/06/18)**: Initial commit.
- **v1.0.1 (2024/06/18)**: function name fix.
- **v1.0.2 (2024/06/22)**: Bug fix.
- **v1.0.3 (2025/02/07)**: DipSW support.

## License

- GPLv3


