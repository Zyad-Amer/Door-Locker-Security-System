# Door Locker Security System

## Overview

This project implements a door locking mechanism controlled by a password using two ATmega32 Microcontrollers. The system is designed and implemented based on a layered architecture model, comprising two main components:

- **HMI_ECU (Human Machine Interface):** Responsible for user interaction via a 2x16 LCD and a 4x4 keypad.
- **Control_ECU:** Manages system operations, including password checking, door unlocking, and activating the alarm, utilizing EEPROM, a Buzzer, and a DC Motor.

## System Requirements

### Hardware Specifications:

- Two ATmega32 Microcontrollers with a frequency of 8MHz.
- 2x16 LCD and 4x4 keypad for the Human Machine Interface.
- EEPROM, Buzzer, and DC Motor for the Control_ECU.

### System Sequence:

1. **Create a System Password:**
   - User inputs a 5-digit password twice for confirmation.
   - The Control_ECU verifies the passwords and saves them in EEPROM.

2. **Main Options:**
   - Displays the main system options on the LCD.

3. **Open Door:**
   - User inputs the password to unlock the door.
   - Control_ECU compares the entered password with the saved one.
   - If matched, the door unlocks and relocks after a specified time.

4. **Change Password:**
   - User inputs the current password to change it.
   - Control_ECU compares the entered password with the saved one.
   - If matched, the system proceeds to create a new password.

5. **Error Handling:**
   - If passwords do not match after three consecutive attempts:
     - Buzzer activates for 1 minute.
     - Error message displayed on LCD.
     - System locks for 1 minute.

## Usage

### Setup:

- Connect the hardware components as per the schematic.
- Upload the respective firmware to each ATmega32 Microcontroller.

### Operation:

- Follow the on-screen instructions on the LCD for password creation, door opening, and password changing.
- Ensure correct input of passwords to prevent system lockout.
