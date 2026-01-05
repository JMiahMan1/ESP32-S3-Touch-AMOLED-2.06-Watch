# ESP32-S3-Touch-AMOLED-2.06 Watch (ESPHome)

This project allows you to seamlessly run **ESPHome** on the Waveshare ESP32-S3-Touch-AMOLED-2.06 Watch, supporting all major hardware verify features.

## Features

### Hardware Support
- **Display:** AXS15231 AMOLED (410x502 resolution)
- **Touch:** FT6336 Capacitive Touch
- **Sensors:**
  - **QMI8658:** IMU used for Step Counting, Tilt-to-Wake.
  - **AXP2101:** Power Management Unit (Battery voltage, percentage, charging status).
- **Audio:**
  - **Speaker:** I2S Amplified output (ES8311 DAC).
  - **Microphone:** I2S Input (ES7210 ADC) for Voice Assistant.
- **Time:** PCF85063 Real-Time Clock (RTC) for offline timekeeping.
- **Storage (New):**
  - **MicroSD Card:** Supports logging and data storage (FAT32 & exFAT supported).
  - **Dynamic Power:** SD card power is managed via AXP2101 (ALDO4).

### Software & Interface (ESPHome + LVGL)
- **Clock Face:** Displays current time, date, daily step count, battery level, WiFi signal, and storage status.
- **App Launcher:** Quick access to integrated applications.
- **Applications:**
  - **Timer:** Custom duration timer with visual countdown and audio alert.
  - **Music Player:** Controls Home Assistant media players (Play/Pause, Next/Prev, Metadata).
  - **Voice Assistant:** Integrated "Push-to-Talk" voice assistant for Home Assistant.
  - **Flashlight:** Max brightness white screen.
- **Settings:**
  - **Connectivity:** Toggle WiFi radio on/off to save power. View IP address.
  - **Display:** Adjust screen timeout.
  - **Storage:** View SD Card status (Mounted/Total/Used) and **Safe Unmount** option.
  - **Date & Time:** Manually set time/date (syncs to RTC).
  - **System:** Reboot and reset options.
  - **About:** Hardware and build information.

### Data Logging
- **Battery Log:** Automatically logs battery voltage, percentage, WiFi signal strength, and system uptime to `/sd/battery_log.csv` every hour.

### Controls
- **Touchscreen:** Tap to interact, swipe to navigate tiles.
- **Side Button:**
  - **Click:** Wake / Sleep Display.
  - **Hold (3s):** Reboot Watch.
  - **Deep Sleep:** Press to wake from critical battery deep sleep.
