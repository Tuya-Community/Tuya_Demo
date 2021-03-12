# Tuya IoTOS Embeded Demo BLE Temperature and humidity sensor

[English](./README.md) | [中文](./README_zh.md)

## Introduction  

This Demo is based on Toodle Smart Cloud Platform, Tuya Smart APP, IoTOS Embeded Ble SDK, using Tuya BLE series modules to quickly build a temperature and humidity sensor to achieve remote monitoring of temperature and humidity data from cell phones.

## Quick start

### Compile & Burn
+ Download [Tuya IoTOS Embeded Demo BLE Temperature and humidity sensor](https://github.com/Tuya-Community/tuya-iotos-embeded-demo-ble-temperature-humidity) 

+ Download the demo to the apps directory of the SDK directory 

  ```bash
  $ cd apps
  $ git clone https://github.com/Weeeendi/tuya-iotos-embeded-demo-ble-temperature-humidity.git
  ```

  ·  [tuya ble sdk](https://docs.tuya.com/zh/iot/device-development/access-mode-link/ble-chip-sdk/tuya-ble-sdk-user-guide?id=K9h5zc4e5djd9)

  ·  [tuya ble sdk demo](https://docs.tuya.com/zh/iot/device-development/access-mode-link/ble-chip-sdk/tuya-ble-sdk-demo-instruction-manual?id=K9gq09szmvy2o)

  - eclipse Import `./telink_kite_ble_sdk_v3.4.0_20190816./ble_sdk_multimode` project，Select `Build 825x_modlue`.
  - Sdk has no Token，so you can't connect to Tuya APP，need first apply for Tuya Inc.
  - Debug：Output IO is GPIO_PC2 ，Baudrate：230400.Enable debugging information by default，You can use `tuya_ble_app_init()` function elog_set_output_enabled(flase) to disable debugging log.


### I/O list

|SHT21||
| --- | --- |
|`SDA`|PB4|
|`SCL`|PD2|

<br>


### dp point related

+ Send down dp point data stream:tuya_cb_handler()
+ Send up dp point data stream: tuya_ble_dp_data_report()

|Fun Name | tuya_ble_status_t tuya_ble_dp_data_report(uint8_t *p_data,uint32_t len)|
|	---|---|
|    p_data | dp data point|
|    Len |dp data length|
|    Return    |  TUYA_BLE_SUCCESS: succese  Other: failure |

<br>

## Flash map
![](https://images.tuyacn.com/fe-static/docs/img/1f0ea62d-9ecf-41ad-96e1-2bd5452443db.png)


<br>


## Related documents

Tuya Demo center：https://developer.tuya.com/demo


<br>

## Technical support

You can get support from Tuya through the following methods:

- Developer Center：https://developer.tuya.com
- Help center: https://support.tuya.com/help
- Technical Support Ticket Center: [https://service.console.tuya.com](https://service.console.tuya.com/) 


<br>
