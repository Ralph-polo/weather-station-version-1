# Power Requirements Analysis

## Overview
This document provides a detailed power consumption analysis for the ESP32 Weather Station running once every hour for 24 hours.

---

## Component Power Specifications

### Operating Power Consumption

| Component | Voltage | Typical Current | Power |
|-----------|---------|-----------------|-------|
| **ESP32** | 3.3V | 80-100 mA | ~0.3W |
| **BME280** (I2C mode) | 3.3V | 0.5-1 mA | ~0.003W |
| **APDS-9960** (I2C mode) | 3.3V | 2-3 mA | ~0.01W |
| **ILI9341 TFT Display** (backlight on) | 3.3V | 200-300 mA | ~1W |
| **WiFi (AP mode, active)** | 3.3V | +80-120 mA extra | ~0.3-0.4W |

### Power Modes

| Mode | Consumption | Description |
|------|-------------|-------------|
| **Active Operation** | 1.6-1.8W | Full system: ESP32 + WiFi + TFT + Sensors |
| **Reduced Power** | 1.3-1.5W | Without WiFi running |
| **Sleep/Idle** | 0.1-0.2W | Minimal active components |
| **Deep Sleep** | ~12 µW | ESP32 in deep sleep mode |

---

## 24-Hour Operating Cycle (One Reading Per Hour)

### Scenario A: Full Power Operation
**Configuration**: TFT display active, WiFi enabled, continuous operation

```
Per measurement cycle (2 seconds):
  Power: 1.8W × (2 sec / 3600 sec) = 0.001W per cycle

Per 24-hour period:
  Active time: 24 cycles × 2 sec = 48 seconds total
  Energy: 1.8W × (48 sec / 3600 sec) = 0.024 Wh
  
Sleep time: 23 hours 58 minutes 12 seconds
  Energy: 0.15W × 23.97 hrs = 3.6 Wh

Total per 24 hours: ≈ 3.6 Wh
Equivalent: ~13 mAh at 3.3V
```

### Scenario B: Optimized Operation
**Configuration**: TFT dimmed, WiFi sleep mode, sensors only active during measurement

```
Per measurement cycle (2 seconds):
  Power: 0.5W × (2 sec / 3600 sec) = 0.00028W per cycle

Per 24-hour period:
  Active time: 24 cycles × 2 sec = 48 seconds total
  Energy: 0.5W × (48 sec / 3600 sec) = 0.0067 Wh
  
Sleep time: 23 hours 58 minutes 12 seconds
  Energy: 0.05W × 23.97 hrs = 1.2 Wh

Total per 24 hours: ≈ 1.2 Wh
Equivalent: ~3.6 mAh at 3.3V
```

### Scenario C: Deep Sleep Mode
**Configuration**: Minimize active power, use ESP32 deep sleep between readings

```
Per measurement cycle (5 seconds - includes wake + measure + sleep):
  Power: 1.8W × (5 sec / 3600 sec) = 0.0025W per cycle

Per 24-hour period:
  Active time: 24 cycles × 5 sec = 120 seconds total
  Energy: 1.8W × (120 sec / 3600 sec) = 0.06 Wh
  
Deep sleep time: 23 hours 58 minutes
  Energy: 0.000012W × 23.97 hrs ≈ 0.0003 Wh

Total per 24 hours: ≈ 0.06 Wh
Equivalent: ~0.22 mAh at 3.3V (theoretical best case)
```

---

## Battery Options & Runtime

### For 24-Hour Operation

| Battery Type | Capacity | Typical Runtime | Notes |
|--------------|----------|-----------------|-------|
| **Single 18650 Li-ion** | 2600-3500 mAh | 20-35 days | Reliable, rechargeable |
| **USB Power Bank** | 10,000 mAh | 60-100 days | Convenient, rechargeable |
| **4x AA Alkaline** | 8000-12000 mAh | 40-60 days | Simple, disposable |
| **2x AA Alkaline** | 4000-6000 mAh | 20-30 days | Compact option |
| **Single AA Alkaline** | 2000-3000 mAh | 10-15 days | Minimal setup |

**Calculation basis**: Scenario A (3.6 Wh/day = ~1100 mAh/day at 3.3V)

---

## Recommendations for Power Optimization

### 1. **Implement Deep Sleep Mode**
- Use ESP32's deep sleep to reduce idle consumption from 150 mW to 12 µW
- Potential savings: **95%+ power reduction during sleep**
- Use a real-time clock (RTC) to wake periodically

```cpp
// Example: Deep sleep for 1 hour between readings
esp_sleep_enable_timer_wakeup(3600 * 1000000); // 1 hour in microseconds
esp_deep_sleep_start();
```

### 2. **TFT Display Management**
- Turn off backlight during idle periods
- Use lower brightness during operation
- Potential savings: **50% of total power**

### 3. **WiFi Optimization**
- Disable WiFi during sleep periods
- Enable only when dashboard access is needed
- Potential savings: **20-30% of total power**

### 4. **Sensor Wake Scheduling**
- Keep sensors powered only during active measurements
- Put BME280 and APDS-9960 into sleep modes between readings
- Potential savings: **Minimal** (already low power)

### 5. **Power Supply Selection**
- Use a voltage regulator with low quiescent current
- Select one with soft-start to reduce inrush current
- Consider solar charging with small solar panel (battery + solar combo)

---

## Power Budget Summary

| Use Case | Daily Consumption | Weekly Consumption | Recommended Battery |
|----------|-------------------|-------------------|---------------------|
| **Full Power (Scenario A)** | 3.6 Wh | 25.2 Wh | 10,000 mAh Power Bank |
| **Optimized (Scenario B)** | 1.2 Wh | 8.4 Wh | 2x AA Alkaline |
| **Deep Sleep (Scenario C)** | 0.06 Wh | 0.42 Wh | Single AA Alkaline |

---

## Component Details

### ESP32
- **Operating Voltage**: 3.3V
- **Operating Current**: 80-160 mA (varies with WiFi activity)
- **Sleep Current**: ~10 mA
- **Deep Sleep Current**: ~12 µA

### BME280 Environmental Sensor
- **Operating Voltage**: 3.3V
- **Operating Current**: 0.5-1 mA
- **Sleep Mode Current**: < 0.1 mA
- **Interface**: I2C

### APDS-9960 RGB Light Sensor
- **Operating Voltage**: 3.3V (or 2.4-3.6V)
- **Operating Current**: 2-3 mA
- **Sleep Mode Current**: < 0.5 mA
- **Interface**: I2C

### ILI9341 TFT Display
- **Operating Voltage**: 3.3V
- **Backlight Current**: 200-300 mA (main power consumer)
- **Logic Current**: 10-20 mA
- **Interface**: SPI

---

## Hardware Wiring Diagram Reference

For the complete wiring diagram and hardware setup, see:
[Hardware/full-wiring diagram.png](https://github.com/Ralph-polo/weather-station-version-1/blob/main/Hardware/full-wiring%20diagram.png)

---

## Conclusion

The ESP32 Weather Station consuming approximately **3.6 Wh per day** in full operation or **0.06 Wh per day** with deep sleep optimization. 

**For reliable 24+ hour operation**, we recommend:
- A **2600 mAh 18650 Li-ion battery** (20+ days runtime)
- Or a **10,000 mAh USB Power Bank** (60+ days runtime)
- With **deep sleep mode** enabled for maximum battery life

For extended deployment without frequent charging, consider adding a small solar charging panel to the system.
