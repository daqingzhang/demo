#ifndef USB_HID_H
#define USB_HID_H

/* These headers are included for child class. */
#include "USBDescriptor.h"
#include "USBDevice.h"

#include "USBHID_Types.h"
#include "AsyncOp.h"
#include "LinkedList.h"

/**
 * USBHID example
 * @code
 * #include "mbed.h"
 * #include "USBHID.h"
 *
 * USBHID hid;
 * HID_REPORT recv;
 * BusOut leds(LED1,LED2,LED3,LED4);
 *
 * int main(void) {
 *    while (1) {
 *        hid.read(&recv);
 *        leds = recv.data[0];
 *    }
 * }
 * @endcode
 */

struct usbhid_dev {

	uint8_t inited;
	uint8_t state;

    uint8_t endp_in;
    uint8_t endp_out;
    uint8_t output_length;
    uint8_t input_length;
    uint16_t report_len;
    HID_REPORT input_report;
    HID_REPORT output_report;

    /**
    * Constructor
    *
    * @param output_report_length Maximum length of a sent report (up to 64 bytes) (default: 64 bytes)
    * @param input_report_length Maximum length of a received report (up to 64 bytes) (default: 64 bytes)
    * @param vendor_id Your vendor_id
    * @param product_id Your product_id
    * @param product_release Your preoduct_release
    * @param connect Connect the device
    */

	void (*init)(uint8_t output_report_length, uint8_t input_report_length,
			uint16_t vendor_id, uint16_t product_id, uint16_t product_release, bool connect);

    /**
     * Block until this HID device is configured
     */
	
    void (*wait_connected)(void);

    /**
    * Send a Report. warning: blocking
    *
    * @param report Report which will be sent (a report is defined by all data and the length)
    * @returns true if successful
    */
    bool (*send)(const HID_REPORT *report);

    /**
    * Send a Report. warning: non blocking
    *
    * @param report Report which will be sent (a report is defined by all data and the length)
    * @returns true if successful
    */
    bool (*send_nb)(const HID_REPORT *report);

    /**
    * Read a report: blocking
    *
    * @param report pointer to the report to fill
    * @returns true if successful
    */
    bool (*read)(HID_REPORT *report);

    /**
    * Read a report: non blocking
    *
    * @param report pointer to the report to fill
    * @returns true if successful
    */
    bool (*read_nb)(HID_REPORT *report);

    /*
    * Get the Report descriptor
    *
    * @returns pointer to the report descriptor
    */
    const uint8_t* (*get_report_desc)(uint16_t *desc_len);

    /*
    * Get configuration descriptor
    *
    * @returns pointer to the configuration descriptor
    */
    const uint8_t* (*get_config_desc)(uint16_t *desc_len);

       /*
    * Get string product descriptor
    *
    * @returns pointer to the string product descriptor
    */
    const uint8_t* (*get_string_iproduct_desc)(void);

    /*
    * Get string interface descriptor
    *
    * @returns pointer to the string interface descriptor
    */
    const uint8_t* (*get_string_interface_desc)(void);

    /*
    * HID Report received by SET_REPORT request. Warning: Called in ISR context
    * First byte of data will be the report ID
    *
    * @param report Data and length received
    */
    void (*callback_set_report)(HID_REPORT *report);

    /**
    * Called when USB changes state
    *
    * @param new_state The new state of the USBDevice
    *
    * Warning: Called in ISR context
    */
    void (*callback_state_change)(int new_state);

    /*
    * This is used to handle extensions to standard requests
    * and class specific requests
    */
    void (*callback_request)(const setup_packet_t *setup);

    /*
    * This is used to handle extensions to standard requests
    * and class specific requests with a data phase
    */
    void (*callback_request_xfer_done)(const setup_packet_t *setup, bool aborted);

    /*
    * Called by USBDevice layer. Set configuration of the device.
    * For instance, you can add all endpoints that you need on this function.
    *
    * @param configuration Number of the configuration
    * @returns true if class handles this request
    */
    void (*callback_set_configuration)(uint8_t configuration);

    /*
    * Called by USBDevice layer in response to set_interface.
    *
    * Upon reception of this command endpoints of any previous interface
    * if any must be removed with endpoint_remove and new endpoint added with
    * endpoint_add.
    *
    * @param configuration Number of the configuration
    *
    * Warning: Called in ISR context
    */
    void (*callback_set_interface)(uint16_t interface, uint8_t alternate);
};

struct usbhid_dev *usbhid_dev_alloc(void);
void usbhid_dev_free(struct usbhid_dev *h);

#endif
