
1. SMP patch V1.0
/***************************************************************  
*@brief    for TLSR chips
* @date     Nov. 12, 2019
***************************************************************/

Fix EMI Test demo must do this below:
1. Replace file "../../telink_kite_ble_sdk_v3.4.0_20190816/ble_sdk_multimode/proj_lib/liblt_8258.a" with "./liblt_8258.a"

/***************************************************************  
*@brief    This patch fix the bug below 
***************************************************************/
1.	Buffer overflow and crashing the device
	1)	Device under risk：only BLE Slave device which use BLE Host SMP protocol of Telink. 82xx_hci project is OK, cause it not use SMP of Telink.
	2)	Software trigger condition：BLE slave application which set Security Level to LE_Security_Mode_1_Level_2(Unauthenticated_Paring_with_Encryption) or above.
	3)	Attack method: 
		a)	Attacker use a BLE master force a connection with device when it’s advertising.
		b)	This BLE master send a special paring request(not standard data format in BLE Specification) to slave device. 
	4)	Attack Results: Slave device Sram buffer overflow, will crash and stop working.  
	5)	Device recover condition: For crashed device, power off and then power on it again. 

2.	Secure connection pairing bypass
	1)	Device under risk：only BLE Slave device which use BLE Host SMP protocol of Telink. 82xx_hci project is OK, cause it not use SMP of Telink.
	2)	Software trigger condition：BLE slave application which set Security Level to LE_Security_Mode_1_Level_4(Authenticated_LE_Secure_Connection_Paring_with_Encryption) .
	3)	Attack method: 
		a)	Attacker use a BLE master force a connection with device when it’s advertising.
		b)	This BLE master send a Encryption request command to slave device directly without any pairing process, then use a uncertified zero STK for data encryption and decryption.
	4)	Attack Results: Attacker can access GATT service data freely, may cause device crash, and even control device function.
	5)	Device recover condition: For crashed device, power off and then power on it again.

