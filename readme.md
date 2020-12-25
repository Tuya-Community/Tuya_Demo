# USER GUIDE

## 支持芯片平台

- tlsr8253

## Flash 布局

![](https://images.tuyacn.com/fe-static/docs/img/1f0ea62d-9ecf-41ad-96e1-2bd5452443db.png)

- ## 说明

  - 说明文档：

  ​       [tuya ble sdk](https://docs.tuya.com/zh/iot/device-development/access-mode-link/ble-chip-sdk/tuya-ble-sdk-user-guide?id=K9h5zc4e5djd9)

  ​       [tuya ble sdk demo](https://docs.tuya.com/zh/iot/device-development/access-mode-link/ble-chip-sdk/tuya-ble-sdk-demo-instruction-manual?id=K9gq09szmvy2o)

  - eclipse导入 ./telink_kite_ble_sdk_v3.4.0_20190816./ble_sdk_multimode 工程，选择编译825x_modlue。
  - sdk中没有授权码，所以不能连接到APP，需要首先向涂鸦申请授权码。
  - 调试输出：输出引脚GPIO_PC2 ，输出波特率：230400。调试信息默认打开，可在tuya_ble_app_init()函数中调用elog_set_output_enabled(flase)关闭调试信息。