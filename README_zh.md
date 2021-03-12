# Tuya IoTOS Embeded Demo BLE Temperature and humidity sensor

[English](./README.md) | [中文](./README_zh.md)

## 简介 

本 Demo 基于涂鸦智能云平台、涂鸦智能APP、IoTOS Embeded WiFi &Ble SDK，使用涂鸦WiFi/WiFi+BLE系列模组快速组建一款温湿度传感器，实现手机远程监测温度和湿度数据。
## 快速上手

### 编译与烧录
+ 下载[tuya-iot-temp-humi-sensor-blesdk](https://github.com/Weeeendi/tuya-iotos-embeded-demo-wifi-ble-temperature-humidity) 

+ 下载Demo至SDK目录的apps目录下 

  ```bash
  $ cd apps
  $ git clone https://github.com/Weeeendi/tuya-iotos-embeded-demo-wifi-ble-temperature-humidity.git
  ```

  ·  [tuya ble sdk](https://docs.tuya.com/zh/iot/device-development/access-mode-link/ble-chip-sdk/tuya-ble-sdk-user-guide?id=K9h5zc4e5djd9)

  ·  [tuya ble sdk demo](https://docs.tuya.com/zh/iot/device-development/access-mode-link/ble-chip-sdk/tuya-ble-sdk-demo-instruction-manual?id=K9gq09szmvy2o)

  - eclipse导入 ./telink_kite_ble_sdk_v3.4.0_20190816./ble_sdk_multimode 工程，选择编译825x_modlue。
  - sdk中没有授权码，所以不能连接到APP，需要首先向涂鸦申请授权码。
  - 调试输出：输出引脚GPIO_PC2 ，输出波特率：230400。调试信息默认打开，可在tuya_ble_app_init()函数中调用elog_set_output_enabled(flase)关闭调试信息。



### I/O 列表

|SHT21||
| --- | --- |
|`SDA`|PB4|
|`SCL`|PD2|

<br>


### dp点相关

+ 下发dp点数据流：tuya_cb_handler()
+ 上报dp点接口: tuya_ble_dp_data_report()

|函数名 | tuya_ble_status_t tuya_ble_dp_data_report(uint8_t *p_data,uint32_t len)|
|	---|---|
|    p_data | dp结构体数组名|
|    Len |dp结构体数组长度|
|    Return    |  TUYA_BLE_SUCCESS: 成功  Other: 失败 |

<br>

## Flash 布局
![](https://images.tuyacn.com/fe-static/docs/img/1f0ea62d-9ecf-41ad-96e1-2bd5452443db.png)


<br>


## 相关文档

涂鸦Demo中心：https://developer.tuya.com/demo


<br>

## 技术支持

您可以通过以下方法获得涂鸦的支持:

- 开发者中心：https://developer.tuya.com
- 帮助中心: https://support.tuya.com/help
- 技术支持工单中心: [https://service.console.tuya.com](https://service.console.tuya.com/) 


<br>
