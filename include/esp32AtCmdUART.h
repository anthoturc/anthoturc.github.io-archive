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
#define UART_PRE_IDLE_NUM 0xffffff

/* 
 * In UART_AT_CMD_POSTCNT_REG register:
 * mask of the bits used to configure the duration between the last at_cmd
 * and the next data. When the duration is less than what this register indicates, 
 * it will not take the previous data as an at_cmd char.
 * 
 * Default: 0x0186A00
 */
#define UART_POST_IDLE_NUM 0xffffff

/* 
 * In UART_AT_CMD_GAPTOUT_REG register:
 * mask of the bits used to configure the interval between the at_cmd chars. 
 * When the interval is greater than the value of this register, 
 * it will not take the data as continuous at_cmd chars. 
 * The register should be configured to more than half of the baud rate.
 * 
 * Default: 0x0001E00
 */
#define UART_RX_GAP_TOUT 0xffffff

/* 
 * In UART_AT_CMD_CHAR_REG register:
 * mask of the bits used to configure the
 * number of continuous at_cmd chars received to trigger 
 * UART_AT_CMD_CHAR_DET_INT interrupt
 * 
 * Default: 0x003
 */
#define UART_CHAR_NUM 0xff00

/* 
 * In UART_AT_CMD_CHAR_REG register:
 * mask of the bits used to configure the at_cmd chars,
 * which triggers the UART_AT_CMD_CHAR_DET_INT interrupt
 * after UART_CHAR_NUM continuous occurances
 * 
 * Default: 0x02B
 */
#define UART_AT_CMD_CHAR 0xff

/* 
 * In UART_INT_ST_REG register:
 * masked interrupt status bit for the UART_AT_CMD_CHAR_DET_INT interrupt
 * 
 * Default: 0
 */
#define UART_AT_CMD_CHAR_DET_INT_ST 18

/* 
 * In UART_INT_ENA_REG register:
 * interrupt enable bit for the UART_AT_CMD_CHAR_DET_INT interrupt
 * 
 * Default: 0
 */
#define UART_AT_CMD_CHAR_DET_INT_ENA 18

/* 
 * In UART_INT_CLR_REG register:
 * bit to clear the UART_AT_CMD_CHAR_DET_INT interrupt
 * 
 * Default: 0
 */
#define UART_AT_CMD_CHAR_DET_INT_CLR 18


namespace atCmdUART {
    /* AT escape sequence detection configuration */
    enum registers {
        UART_AT_CMD_PRECNT_REG  = 0x3FF40048,
        UART_AT_CMD_POSTCNT_REG = 0x3FF4004C,
        UART_AT_CMD_GAPTOUT_REG = 0x3FF40050,
        UART_AT_CMD_CHAR_REG    = 0X3FF50054,
        UART_INT_ST_REG         = 0x3FF40008,
        UART_INT_ENA_REG        = 0x3FF4000C,
        UART_INT_CLR_REG        = 0x3FF40010,
    };
}
