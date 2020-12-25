
1. 0004_20200402_8258_Fix_Error_Handing_Issues
/***************************************************************  
*@brief    for TLSR chips
* @date     Apr. 2, 2020
***************************************************************/

Fix error handing issues must do this below:
1. Replace file "../../telink_kite_ble_sdk_v3.4.0_20190816/ble_sdk_multimode/proj_lib/liblt_8258.a" with "./liblt_8258.a"

/***************************************************************  
*@brief    This patch fix the bug below 
1. Fix error handling issues via adding strict data check when receiving unexpected request from peer device.

	1)fix "response to VERSION_IND more than once"
	2)fix "response to data channel PDUs during encryption procedure"
	3)fix "sends unkunwn ll contorl PDU opcode"
	4)fix "accepts CONNECT_IND with hopincrement outside 5-16 range"

