#ifndef MYUSB_H
#define MYUSB_H

/* Defines */
#define USBD1_DATA_REQUEST_EP 1
#define USBD1_DATA_AVAILABLE_EP 1
#define USBD1_INTERRUPT_REQUEST_EP 2

/* Typedefs */

/* Global variables */
extern SerialUSBDriver SDU1;
extern const USBConfig usbcfg;
extern const SerialUSBConfig serusbcfg;

/* Macros */
#define USBClaim() chMtxLock(&USB_write_lock)
#define USBRelease() chMtxUnlock(&USB_write_lock)

/* Inline functions */

/* Global functions */
uint32_t USBSendData(uint8_t *data, uint32_t size, systime_t timeout);
uint32_t USBReadByte(systime_t timeout);

#endif
