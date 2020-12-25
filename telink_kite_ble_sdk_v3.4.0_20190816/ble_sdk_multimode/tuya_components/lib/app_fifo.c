//#include "app_fifo.h"
//#include "tuya_log.h"
//#define __INLINE inline
//#define  NRF_SUCCESS    0
//#define  NRF_ERROR_NULL 1
//#define  NRF_ERROR_INVALID_LENGTH  2
//#define  NRF_ERROR_NO_MEM 3
//#define  NRF_ERROR_NOT_FOUND 4
//
//
///**@brief Macro for checking if an integer is a power of two.
// *
// * @param[in]   A   Number to be tested.
// *
// * @return      true if value is power of two.
// * @return      false if value not power of two.
// */
//#define IS_POWER_OF_TWO(A) ( ((A) != 0) && ((((A) - 1) & (A)) == 0) )
///** Leaves the minimum of the two 32-bit arguments */
///*lint -emacro(506, MIN) */ /* Suppress "Constant value Boolean */
//#define MIN(a, b) ((a) < (b) ? (a) : (b))
///** Leaves the maximum of the two 32-bit arguments */
///*lint -emacro(506, MAX) */ /* Suppress "Constant value Boolean */
//#define MAX(a, b) ((a) < (b) ? (b) : (a))
//
//static __INLINE unsigned int fifo_length(app_fifo_t * p_fifo)
//{
//    unsigned int tmp = p_fifo->read_pos;
//    return p_fifo->write_pos - tmp;
//}
//
//
//#define FIFO_LENGTH() fifo_length(p_fifo)  /**< Macro for calculating the FIFO length. */
//
//
///**@brief Put one byte to the FIFO. */
//static __INLINE void fifo_put(app_fifo_t * p_fifo, unsigned char byte)
//{
//    p_fifo->p_buf[p_fifo->write_pos & p_fifo->buf_size_mask] = byte;
//    p_fifo->write_pos++;
//}
//
//
///**@brief Look at one byte in the FIFO. */
//static __INLINE void fifo_peek(app_fifo_t * p_fifo, uint16_t index, unsigned char * p_byte)
//{
//    *p_byte = p_fifo->p_buf[(p_fifo->read_pos + index) & p_fifo->buf_size_mask];
//}
//
//
///**@brief Get one byte from the FIFO. */
//static __INLINE void fifo_get(app_fifo_t * p_fifo, unsigned char * p_byte)
//{
//    fifo_peek(p_fifo, 0, p_byte);
//    p_fifo->read_pos++;
//}
//
//
//unsigned int app_fifo_init(app_fifo_t * p_fifo, unsigned char * p_buf, uint16_t buf_size)
//{
//    // Check buffer for null pointer.
//    if (p_buf == NULL)
//    {
//        return NRF_ERROR_NULL;
//    }
//
//    // Check that the buffer size is a power of two.
//    if (!IS_POWER_OF_TWO(buf_size))
//    {
//        return NRF_ERROR_INVALID_LENGTH;
//    }
//
//    p_fifo->p_buf         = p_buf;
//    p_fifo->buf_size_mask = buf_size - 1;
//    p_fifo->read_pos      = 0;
//    p_fifo->write_pos     = 0;
//
//    return NRF_SUCCESS;
//}
//
//
//unsigned int app_fifo_put(app_fifo_t * p_fifo, unsigned char byte)
//{
//    if (FIFO_LENGTH() <= p_fifo->buf_size_mask)
//    {
//        fifo_put(p_fifo, byte);
//        return NRF_SUCCESS;
//    }
//
//    return NRF_ERROR_NO_MEM;
//}
//
//
//unsigned int app_fifo_get(app_fifo_t * p_fifo, unsigned char * p_byte)
//{
//    if (FIFO_LENGTH() != 0)
//    {
//        fifo_get(p_fifo, p_byte);
//        return NRF_SUCCESS;
//    }
//
//    return NRF_ERROR_NOT_FOUND;
//
//}
//
//
//unsigned int app_fifo_peek(app_fifo_t * p_fifo, uint16_t index, unsigned char * p_byte)
//{
//    if (FIFO_LENGTH() > index)
//    {
//        fifo_peek(p_fifo, index, p_byte);
//        return NRF_SUCCESS;
//    }
//
//    return NRF_ERROR_NOT_FOUND;
//}
//
//
//unsigned int app_fifo_flush(app_fifo_t * p_fifo)
//{
//    p_fifo->read_pos = p_fifo->write_pos;
//    return NRF_SUCCESS;
//}
//
//
//unsigned int app_fifo_read(app_fifo_t * p_fifo, unsigned char * p_byte_array, unsigned int  *p_size)
//{
//    //VERIFY_PARAM_NOT_NULL(p_fifo);
//    //VERIFY_PARAM_NOT_NULL(p_size);
//    if(p_fifo==NULL||p_size==0||p_byte_array==NULL)
//    {
//    	return 0;
//    }
//    const unsigned int byte_count    = fifo_length(p_fifo);
//    const unsigned int requested_len = (*p_size);
//    unsigned int       index         = 0;
//    unsigned int       read_size     = MIN(requested_len, byte_count);
//
//    (*p_size) = byte_count;
//
//    // Check if the FIFO is empty.
//    if (byte_count == 0)
//    {
//        return 0;
//    }
//
//    // Check if application has requested only the size.
//    if (p_byte_array == NULL)
//    {
//        return 0;
//    }
//
//    // Fetch bytes from the FIFO.
//    while (index < read_size)
//    {
//        fifo_get(p_fifo, &p_byte_array[index++]);
//    }
//
//    (*p_size) = read_size;
//    if(p_fifo->read_pos> p_fifo->write_pos)
//    {
//    	p_fifo->read_pos=p_fifo->write_pos;
//    	tuya_log_d("system err!!!!!!!!!!!!!!!!!!!-%0x-%0x-%0x",p_fifo->read_pos,p_fifo->write_pos,read_size);
//    }
//    return read_size;
//}
//
//
//unsigned int app_fifo_write(app_fifo_t * p_fifo, unsigned char const * p_byte_array, unsigned int * p_size)
//{
//	 if(p_fifo==NULL||p_size==0)
//	{
//		return 1;
//	}
//
//    const unsigned int available_count = p_fifo->buf_size_mask - fifo_length(p_fifo) + 1;
//    const unsigned int requested_len   = (*p_size);
//    unsigned int       index           = 0;
//    unsigned int       write_size      = MIN(requested_len, available_count);
//
//    (*p_size) = available_count;
//
//    // Check if the FIFO is FULL.
//    if (available_count == 0)
//    {
//        return 2;
//    }
//
//    // Check if application has requested only the size.
//    if (p_byte_array == NULL)
//    {
//        return 3;
//    }
//
//    //Fetch bytes from the FIFO.
//    while (index < write_size)
//    {
//        fifo_put(p_fifo, p_byte_array[index++]);
//    }
//
//    (*p_size) = write_size;
//
//    return NRF_SUCCESS;
//}
//
//static app_fifo_t    tx_fifo;
//static unsigned char tx_buffer[1024];
//
//static app_fifo_t    rx_fifo;
//static unsigned char rx_buffer[128];
//
//
//
//unsigned int rx_fifo_write_bytes(unsigned char *data,unsigned int len)
//{
//
//	unsigned int errcode= app_fifo_write(&rx_fifo,data,&len);
//	return errcode;
//}
//unsigned int rx_fifo_write_byte(unsigned char data)
//{
//	unsigned int errcode= app_fifo_put(&rx_fifo,data);
//	return errcode;
//}
//unsigned int rx_fifo_read_bytes(unsigned char *data,unsigned int len)
//{
//	return app_fifo_read(&rx_fifo,data,&len);
//}
//
//unsigned int log_fifo_write_bytes(unsigned char *data,unsigned int len)
//{
//	return app_fifo_write(&tx_fifo,data,&len);
//}
//unsigned int log_fifo_read_bytes(unsigned char *data,unsigned int len)
//{
//	return app_fifo_read(&tx_fifo,data,&len);
//}
//
//void log_fifo_init()
//{
//	app_fifo_init(&tx_fifo, tx_buffer, sizeof(tx_buffer));
//	app_fifo_init(&rx_fifo,rx_buffer, sizeof(rx_buffer));
//}
//

