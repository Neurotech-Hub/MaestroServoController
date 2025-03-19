# Maestro Servo Controller

This Arduino project demonstrates how to smoothly control a servo using a Pololu Maestro servo controller.

## Hardware Requirements
- Arduino board
- Pololu Maestro servo controller
- RC hobby servo (connected to channel 0)

## Setup Instructions

1. **Maestro Control Center Settings**
   - Go to the Serial Settings tab and apply these settings:
     - Serial mode: UART, fixed baud rate
     - Baud rate: 9600
     - CRC disabled
   - Click "Apply Settings" after making changes

2. **Connections**
   - Connect your Arduino to your Maestro following the wiring guide at:
   - https://github.com/pololu/maestro-arduino

## Features
- Smooth servo control with adjustable speed and acceleration
- Example movements demonstrating different speed and acceleration settings
- Uses the PololuMaestro library for communication 