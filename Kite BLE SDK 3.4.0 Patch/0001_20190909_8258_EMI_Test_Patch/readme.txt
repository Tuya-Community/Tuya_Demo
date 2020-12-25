
1. EMI demo patch
/***************************************************************  
*@brief    for TLSR chips
* @date     Sep. 9, 2019
***************************************************************/

Fix EMI Test demo must do this below:
1. Replace file "../../telink_kite_ble_sdk_v3.4.0_20190816/ble_sdk_multimode/vendor/8258_driver_test/main.c" with "./main.c"
2. Replace file "../../telink_kite_ble_sdk_v3.4.0_20190816/ble_sdk_multimode/vendor/8258_driver_test/app_emi.c" with "./app_emi.c"
3. Replace file "../../telink_kite_ble_sdk_v3.4.0_20190816/ble_sdk_multimode/drivers/8258/emi.c" with "./emi.c"