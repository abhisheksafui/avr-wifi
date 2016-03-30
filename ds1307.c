#include<i2cmaster.h>


/**
 * @brief 
 *
 * @return 
 */
uint8_t rtc_enable(void)
{

  return 1;
}


/**
 * @brief Set the format of time 12 or 24 Hours
 *
 * @param format
 *
 * @return 
 */
uint8_t rtc_set_time_format(TIME_FORMAT format)
{
  return rc;
}
