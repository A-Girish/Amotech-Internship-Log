# 8051 Modbus RTU Slave (UART) — README

## Overview

This project implements a **minimal Modbus RTU slave** on an 8051 microcontroller using UART.
It supports:

* Function `0x03` → Read Holding Register
* Function `0x06` → Write Single Holding Register

The device exposes one holding register and responds over serial using Modbus RTU framing with CRC.

---

## Memory Map

| Register Address | Type             | Variable       | Default Value  |
| ---------------- | ---------------- | -------------- | -------------- |
| 0                | Holding Register | `holding_reg0` | 123            |
| 0                | Input Register   | `input_reg0`   | 456 *(unused)* |

Only holding register 0 is actually used by Modbus functions.

---

## Function Descriptions

### `uart_init()`

Initializes UART for serial communication.

* Timer1 used in auto-reload mode
* Sets baud rate (~9600 for 11.0592 MHz crystal)
* Enables serial transmission & reception

---

### `uart_tx(unsigned char c)`

Transmits one byte via UART.

1. Loads byte into serial buffer
2. Waits for transmission complete flag
3. Clears flag for next byte

Used to send Modbus responses.

---

### `uart_rx()`

Receives one byte from UART.

1. Waits until a byte arrives
2. Clears receive flag
3. Returns received byte

Used to collect incoming Modbus frames.

---

### `modbus_crc(unsigned char *buf, unsigned char len)`

Computes Modbus RTU CRC16 checksum.

* Initial value = `0xFFFF`
* Polynomial = `0xA001`
* Processes each bit of each byte

Returned CRC is appended to response frames.

---

### `main()`

Core Modbus slave loop:

1. Initializes UART
2. Waits for 8-byte Modbus request
3. Checks slave ID (`1`)
4. Executes function based on request code

Supported operations:

#### Function 03 — Read Holding Register

* Extracts register address
* Reads stored value
* Builds Modbus response frame
* Sends data + CRC

#### Function 06 — Write Holding Register

* Extracts register address and value
* Stores value into memory
* Echoes request back as response (Modbus rule)
* Sends CRC

---

## Using with Modscan (PC Master Tool)

### Step 1 — Hardware Connection

Use a **USB-to-TTL converter**.

| USB-TTL Pin | Connect to 8051        |
| ----------- | ---------------------- |
| TXD         | RXD (P3.0)             |
| RXD         | TXD (P3.1)             |
| GND         | GND                    |
| VCC         | 5V (if powering board) |

Important:

* TX always goes to RX
* RX always goes to TX
* Ground must be common

---

### Step 2 — Modscan Configuration

1. Open Modscan

2. Select **RTU mode**

3. Choose COM port of USB-TTL adapter

4. Set:

   * Baud: 9600
   * Data bits: 8
   * Parity: None
   * Stop bits: 1

5. Set Slave ID = `1`

---

### Step 3 — Reading Register

In Modscan:

* Function = 03
* Address = 0
* Length = 1

You should receive value `123`.

---

### Step 4 — Writing Register

In Modscan:

* Function = 06
* Address = 0
* Value = any number (e.g. 555)

Send command, then read again using function 03.
Returned value will match what you wrote.

---

## Limitations

This is a learning/demo implementation:

* Supports only one register
* No timeout handling
* No frame validation
* No CRC checking on incoming frames
* No multi-register support

---

## Possible Improvements

To make it closer to real industrial Modbus devices:

* Add CRC verification for incoming frames
* Support multiple registers
* Implement timeout-based frame detection
* Add exception responses for invalid addresses
* Support more function codes (04, 16, etc.)

---

## Summary

This code demonstrates:

* UART communication on 8051
* Modbus RTU frame structure
* CRC calculation
* Register read/write handling

It forms a solid base for building a full Modbus slave device.
