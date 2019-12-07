/*
 * Header file contains neccessary definitions and declarations to
 * configure the UART at_cmd interrupt on the ESP32 chip, triggered
 * when the receiver detects the configured at_cmd char.
 * 
 * See page 340 of the esp32 technical reference manual for more details.
 */

/* 
 * In UART_AT_CMD_PRECNT_REG register:
 * mask of the bits used to configure the idle-time duration before the first
 * at_cmd is received by the receiver. When the duration is less than what this 
 * register indicates, it will not take the next data received as an at_cmd char.
 * 
 * Default: 0x0186A00
 */
#define UART_PRE_IDLE_NUM_MASK 0xffffff

/* 
 * In UART_AT_CMD_POSTCNT_REG register:
 * mask of the bits used to configure the duration between the last at_cmd
 * and the next data. When the duration is less than what this register indicates, 
 * it will not take the previous data as an at_cmd char.
 * 
 * Default: 0x0186A00
 */
#define UART_POST_IDLE_NUM_MASK 0xffffff

/* 
 * In UART_AT_CMD_GAPTOUT_REG register:
 * mask of the bits used to configure the interval between the at_cmd chars. 
 * When the interval is greater than the value of this register, 
 * it will not take the data as continuous at_cmd chars. 
 * The register should be configured to more than half of the baud rate.
 * 
 * Default: 0x0001E00
 */
#define UART_RX_GAP_TOUT_MASK 0xffffff

/* 
 * In UART_AT_CMD_CHAR_REG register:
 * mask of the bits used to configure the
 * number of continuous at_cmd chars received to trigger 
 * UART_AT_CMD_CHAR_DET_INT interrupt
 * 
 * Default: 0x003
 */
#define UART_CHAR_NUM_MASK 0xff00
#define UART_CHAR_NUM_BIT 8 // for bit shifting new inputs

/* 
 * In UART_AT_CMD_CHAR_REG register:
 * mask of the bits used to configure the at_cmd chars,
 * which triggers the UART_AT_CMD_CHAR_DET_INT interrupt
 * after UART_CHAR_NUM continuous occurances
 * 
 * Default: 0x02B
 */
#define UART_AT_CMD_CHAR_MASK 0xff

/* 
 * In UART_INT_ST_REG register:
 * masked interrupt status bit for the UART_AT_CMD_CHAR_DET_INT interrupt
 * 
 * Default: 0
 */
#define UART_AT_CMD_CHAR_DET_INT_ST_BIT 18

/* 
 * In UART_INT_ENA_REG register:
 * interrupt enable bit for the UART_AT_CMD_CHAR_DET_INT interrupt
 * 
 * Default: 0
 */
#define UART_AT_CMD_CHAR_DET_INT_ENA_BIT 18

/* 
 * In UART_INT_CLR_REG register:
 * bit to clear the UART_AT_CMD_CHAR_DET_INT interrupt
 * 
 * Default: 0
 */
#define UART_AT_CMD_CHAR_DET_INT_CLR_BIT 18


/* pointers to important registers */
#define UART_AT_CMD_PRECNT_REG   *reinterpret_cast<uint32_t *>(0x3FF40048)
#define UART_AT_CMD_POSTCNT_REG  *reinterpret_cast<uint32_t *>(0x3FF4004C)
#define UART_AT_CMD_GAPTOUT_REG  *reinterpret_cast<uint32_t *>(0x3FF40050)
#define UART_AT_CMD_CHAR_REG     *reinterpret_cast<uint32_t *>(0X3FF40054)
#define UART_INT_RAW_REG         *reinterpret_cast<uint32_t *>(0x3FF40004)
#define UART_INT_ST_REG          *reinterpret_cast<uint32_t *>(0x3FF40008)
#define UART_INT_ENA_REG         *reinterpret_cast<uint32_t *>(0x3FF4000C)
#define UART_INT_CLR_REG         *reinterpret_cast<uint32_t *>(0x3FF40010)
