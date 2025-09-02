#ifndef __XMODEM_H__
#define __XMODEM_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @param timeout_s : Timeout in seconds
 */
int uXMODEM_Process(uint32_t timeout_s);

#ifdef __cplusplus
}
#endif


#endif /* __XMODEM_H__ */

