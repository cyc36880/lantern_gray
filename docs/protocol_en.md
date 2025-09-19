## 1. Overview

This module is a composite sensor integrating **ultrasonic distance measurement** and **RGB color control** functions, supporting communication via the I²C interface. It is compatible with M5 series development boards (basic functions such as distance reading can be directly invoked, but **color control requires custom I²C commands**). Strict adherence to the protocol specifications is required to ensure communication stability and data accuracy.

------

## 2. Core Features and Key Considerations

### 1. Compatibility and Limitations

- **M5 Compatibility**:

  Basic functions (e.g., distance reading) can be directly called on M5 devices. However, **the built-in M5 modules do not support direct probe color configuration**. Users must write custom I²C commands to control color parameters.

- **Read Frequency Limitation**:

  The interval between consecutive read operations must be **≥50 ms**. Excessively high read frequencies will cause abnormal returned data due to internal sensor processing delays.

- **Power-On Default State**:

  When the module is powered on and no write operations have been executed, it defaults to displaying **blue** (initial value for RGB color control).

------

## 3. Device Basic Information

### 1. I²C Device Address

Fixed at **0x57** (7-bit address format; no additional bit-shifting is required).

------

## 4. Functional Operation Protocols

### 1. RGB Color Setting (Command ID: 0x02)

#### Function Description

Dynamically adjusts the RGB color and brightness of the integrated LED probe on the module via I²C commands.

#### Operation Procedure

1. **Send Command Byte**:

   Write **1 byte of command** `0x02`(identifies the color setting instruction).

2. **Send Color Parameters**:

   Immediately follow with **4 bytes of parameters** in the following order:

| Byte Sequence |   Parameter Name    | Value Range |                         Description                          |
| ------------: | :-----------------: | :---------: | :----------------------------------------------------------: |
|        Byte 1 |     Brightness      |    0–255    | Controls overall RGB brightness (0 = off, 255 = maximum brightness) |
|        Byte 2 |  Red Component (R)  |    0–255    |                 Intensity of the red channel                 |
|        Byte 3 | Green Component (G) |    0–255    |                Intensity of the green channel                |
|        Byte 4 | Blue Component (B)  |    0–255    |                Intensity of the blue channel                 |

> **Parameter Order**: Brightness → R → G → B (4 contiguous bytes must be written sequentially).

------

### 2. Distance Reading (Command ID: 0x03)

#### Function Description

Retrieves the current target distance calculated by the module’s ultrasonic ranging function (unit: centimeters).

#### Operation Procedure

1. **Send Command Byte** (Compatibility Requirement):

   Write **1 byte of command** `0x03`(identifies the distance reading instruction).

   *Note: This step is designed solely for M5 compatibility and can be omitted in standalone applications.*

2. **Read Returned Data**:

   Read **3 bytes of raw data** from the module (sequentially: Byte0, Byte1, Byte2).

3. **Calculate Actual Distance**:

   Convert the raw data to centimeters using the following formula:

   ```
   distance (cm) = ( (Byte0 << 16) | (Byte1 << 8) | Byte2 ) / 10000.0
   ```

   - **Calculation Logic**: Combine the 3 bytes into a 24-bit integer value, then divide by 10,000 to obtain the distance in centimeters.

> **Critical Requirement**: The time interval between two consecutive distance read operations must be **>50 ms**. Otherwise, incomplete sensor processing may result in data errors.

------

## 5. Protocol Usage Constraints

1. **Write Specifications**:

   All instructions (color setting/distance read triggers) are initiated via **single-byte commands**. Each I²C write operation is limited to **1 byte only**. When multiple bytes are required (e.g., color setting: command + 4 parameters), they must be written **strictly in the defined sequence**.

2. **Read Specifications**:

   Distance readings must follow the exact sequence: **write command → read 3 bytes**. Users are responsible for merging the 24-bit raw data and performing unit conversion.

3. **Stability Requirements**:

   Due to internal sensor state transition delays, a **software delay (e.g., 50–100 ms)** is recommended after sending a command before executing a read operation. This ensures the module has completed mode switching and avoids data anomalies caused by incomplete initialization.

------

*This protocol balances compatibility with the M5 ecosystem and flexibility for standalone development. Users may adjust implementation details based on their hardware platform, but strict compliance with communication timing and data format requirements is mandatory to ensure system reliability and measurement accuracy.*