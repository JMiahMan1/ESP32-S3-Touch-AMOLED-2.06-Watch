This Project is for the Waveshare watch https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-2.06 and is based on cade created by https://github.com/RealDeco

So far I have changed the idle page to include date and time. The voice images have also been changed, apologies to whoever created these picture as I can't remember where I got them from to post a link

![watch](https://github.com/user-attachments/assets/9229cd6f-fe95-4adf-ae9c-a5d63fd10b0e)

## Features

### Hardware Support
- **Display:** AXS15231 AMOLED (410x502 resolution)
- **Touch:** FT6336 Capacitive Touch
- **Sensors:**
  - **QMI8658:** IMU used for Step Counting and Tilt-to-Wake.
  - **AXP2101:** Power Management Unit (Battery voltage, percentage, charging status).
- **Audio:**
  - **Speaker:** I2S Amplified output (ES8311 DAC).
  - **Microphone:** I2S Input (ES7210 ADC) for Voice Assistant.
- **Time:** PCF85063 Real-Time Clock (RTC) for offline timekeeping.

### Software & Interface (ESPHome + LVGL)
- **Clock Face:** Displays current time, date, daily step count, battery level, and WiFi connection status.
- **App Launcher:** Quick access to integrated applications.
- **Applications:**
  - **Timer:** Custom duration timer with visual countdown and audio alert.
  - **Music Player:** Controls Home Assistant media players (Play/Pause, Next/Prev, Metadata).
  - **Voice Assistant:** integrated "Push-to-Talk" voice assistant for Home Assistant.
  - **Flashlight:** Max brightness white screen.
- **Settings:**
  - **Connectivity:** Toggle WiFi radio on/off to save power. View IP address.
  - **Display:** Adjust screen timeout.
  - **Date & Time:** Manually set time/date (syncs to RTC).
  - **System:** Reboot and reset options.
  - **About:** Hardware and build information.

## File Structure

| File / Directory | Description | Status |
| :--- | :--- | :--- |
| **`watch.yaml`** | **Main ESPHome configuration file.** This is the active file used to build and flash the watch. | **Active** |
| `wifi_fix.h` | C header file included by `watch.yaml`. Contains WiFi fix implementation. | Active |
| `images/` | Directory containing image assets used by the display configuration. | Active |
| `material_icons.ttf` | TrueType font file used for icons in the UI. | Active |
| `secrets.yaml` | Contains sensitive WiFi credentials (SSID/Password). | Active |
| `timer_finished.flac` | Audio file for timer alert. | Active |
