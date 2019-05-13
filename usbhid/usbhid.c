#include "stdint.h"
#include "usbhid.h"

#define USBHID_DEV_NR 1

static struct usbhid_dev uhid_devs[USBHID_DEV_NR];
static struct usbhid_dev *hid_act = NULL;

static int usbhid_init(uint8_t output_report_length, uint8_t input_report_length,
				uint16_t vendor_id, uint16_t product_id, uint16_t product_release, bool connect)
{
	return 0;
}

static void usbhid_wait_connected()
{
}

static bool usbhid_send(const HID_REPORT *report)
{
    return false;
}

static bool usbhid_send_nb(const HID_REPORT *report)
{
    return false;
}

static bool usbhid_read(HID_REPORT *report)
{
    return false;
}

static bool usbhid_read_nb(HID_REPORT *report)
{
    return false;
}

static uint8_t *usbhid_get_report_desc(uint16_t *desc_len)
{
    return 0;
}

static void usbhid_callback_state_change(int new_state)
{
}

//
//  Route callbacks from lower layers to class(es)
//

// Called in ISR context
// Called by USBDevice on Endpoint0 request
// This is used to handle extensions to standard requests
// and class specific requests
// Return true if class handles this request
static void usbhid_callback_request(const setup_packet_t *setup)
{
    uint8_t *hidDescriptor;
    RequestResult result = PassThrough;
    uint8_t *data = NULL;
    uint32_t size = 0;

    // Process additional standard requests

    if ((setup->bmRequestType.Type == STANDARD_TYPE)) {
        switch (setup->bRequest) {
            case GET_DESCRIPTOR:
                switch (DESCRIPTOR_TYPE(setup->wValue)) {
                    case REPORT_DESCRIPTOR:
                        if ((report_desc() != NULL) \
                                && (report_desc_length() != 0)) {
                            size = report_desc_length();
                            data = (uint8_t *)report_desc();
                            result = Send;
                        }
                        break;
                    case HID_DESCRIPTOR:
                        // Find the HID descriptor, after the configuration descriptor
                        hidDescriptor = find_descriptor(HID_DESCRIPTOR);
                        if (hidDescriptor != NULL) {
                            size = HID_DESCRIPTOR_LENGTH;
                            data = hidDescriptor;
                            result = Send;
                        }
                        break;

                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    // Process class-specific requests

    if (setup->bmRequestType.Type == CLASS_TYPE) {
        switch (setup->bRequest) {
            case SET_REPORT:
                // First byte will be used for report ID
                _output_report.data[0] = setup->wValue & 0xff;
                _output_report.length = setup->wLength + 1;

                size = sizeof(_output_report.data) - 1;
                data = &_output_report.data[1];
                result = Send;
                break;
            default:
                break;
        }
    }

    complete_request(result, data, size);
}

static void usbhid_callback_request_xfer_done(const setup_packet_t *setup, bool aborted)
{
}

#define DEFAULT_CONFIGURATION (1)

// Called in ISR context
// Set configuration. Return false if the
// configuration is not supported
static void usbhid_callback_set_configuration(uint8_t configuration)
{

}

static void usbhid_callback_set_interface(uint16_t interface, uint8_t alternate)
{
}

static const uint8_t *usbhid_get_string_interface_desc(void)
{
    static const uint8_t string_iinterface_descriptor[] = {
        0x08,               //bLength
        STRING_DESCRIPTOR,  //bDescriptorType 0x03
        'H', 0, 'I', 0, 'D', 0, //bString iInterface - HID
    };
    return string_iinterface_descriptor;
}

static const uint8_t *usbhid_get_string_iproduct_desc(void)
{
    static const uint8_t string_iproduct_descriptor[] = {
        0x16,                                                       //bLength
        STRING_DESCRIPTOR,                                          //bDescriptorType 0x03
        'H', 0, 'I', 0, 'D', 0, ' ', 0, 'D', 0, 'E', 0, 'V', 0, 'I', 0, 'C', 0, 'E', 0 //bString iProduct - HID device
    };
    return string_iproduct_descriptor;
}

static const uint8_t *usbhid_get_report_desc(uint16_t *desc_len)
{
    static uint8_t report_descriptor[] = {
        USAGE_PAGE(2), LSB(0xFFAB), MSB(0xFFAB),
        USAGE(2), LSB(0x0200), MSB(0x0200),
        COLLECTION(1), 0x01, // Collection (Application)

        REPORT_SIZE(1), 0x08, // 8 bits
        LOGICAL_MINIMUM(1), 0x00,
        LOGICAL_MAXIMUM(1), 0xFF,

        REPORT_COUNT(1), _input_length,
        USAGE(1), 0x01,
        INPUT(1), 0x02, // Data, Var, Abs

        REPORT_COUNT(1), _output_length,
        USAGE(1), 0x02,
        OUTPUT(1), 0x02, // Data, Var, Abs

        END_COLLECTION(0),
    };

	if (desc_len)
    	*desc_len = sizeof(report_descriptor);

    return report_descriptor;
}

#define DEFAULT_CONFIGURATION (1)
#define TOTAL_DESCRIPTOR_LENGTH ((1 * CONFIGURATION_DESCRIPTOR_LENGTH) \
                               + (1 * INTERFACE_DESCRIPTOR_LENGTH) \
                               + (1 * HID_DESCRIPTOR_LENGTH) \
                               + (2 * ENDPOINT_DESCRIPTOR_LENGTH))

static const uint8_t *usbhid_get_config_desc(uint16_t *desc_len)
{
    static uint8_t configuration_descriptor[] = {
        CONFIGURATION_DESCRIPTOR_LENGTH,    // bLength
        CONFIGURATION_DESCRIPTOR,           // bDescriptorType
        LSB(TOTAL_DESCRIPTOR_LENGTH),       // wTotalLength (LSB)
        MSB(TOTAL_DESCRIPTOR_LENGTH),       // wTotalLength (MSB)
        0x01,                               // bNumInterfaces
        DEFAULT_CONFIGURATION,              // bConfigurationValue
        0x00,                               // iConfiguration
        C_RESERVED | C_SELF_POWERED,        // bmAttributes
        C_POWER(0),                         // bMaxPower

        INTERFACE_DESCRIPTOR_LENGTH,        // bLength
        INTERFACE_DESCRIPTOR,               // bDescriptorType
        0x00,                               // bInterfaceNumber
        0x00,                               // bAlternateSetting
        0x02,                               // bNumEndpoints
        HID_CLASS,                          // bInterfaceClass
        HID_SUBCLASS_NONE,                  // bInterfaceSubClass
        HID_PROTOCOL_NONE,                  // bInterfaceProtocol
        0x00,                               // iInterface

        HID_DESCRIPTOR_LENGTH,              // bLength
        HID_DESCRIPTOR,                     // bDescriptorType
        LSB(HID_VERSION_1_11),              // bcdHID (LSB)
        MSB(HID_VERSION_1_11),              // bcdHID (MSB)
        0x00,                               // bCountryCode
        0x01,                               // bNumDescriptors
        REPORT_DESCRIPTOR,                  // bDescriptorType
        (uint8_t)(LSB(report_desc_length())),   // wDescriptorLength (LSB)
        (uint8_t)(MSB(report_desc_length())),   // wDescriptorLength (MSB)

        ENDPOINT_DESCRIPTOR_LENGTH,         // bLength
        ENDPOINT_DESCRIPTOR,                // bDescriptorType
        _int_in,                            // bEndpointAddress
        E_INTERRUPT,                        // bmAttributes
        LSB(MAX_HID_REPORT_SIZE),           // wMaxPacketSize (LSB)
        MSB(MAX_HID_REPORT_SIZE),           // wMaxPacketSize (MSB)
        1,                                  // bInterval (milliseconds)

        ENDPOINT_DESCRIPTOR_LENGTH,         // bLength
        ENDPOINT_DESCRIPTOR,                // bDescriptorType
        _int_out,                           // bEndpointAddress
        E_INTERRUPT,                        // bmAttributes
        LSB(MAX_HID_REPORT_SIZE),           // wMaxPacketSize (LSB)
        MSB(MAX_HID_REPORT_SIZE),           // wMaxPacketSize (MSB)
        1,                                  // bInterval (milliseconds)
    };

	if (desc_len)
    	*desc_len = sizeof(configuration_descriptor);

    return configuration_descriptor;
}

static ushbid_dev *usbhid_dev_get_one(void)
{
	int i;

	for(i = 0;i < ARRAY_SIZE(uhid_devs); i++) {
		if (!uhid_devs[i].init) {
			return uhid_devs[i];
		}
	}
	return NULL;
}

static void usbhid_dev_set_active(struct usbhid_dev *h)
{
	hid_act = h;
}

struct usbhid_dev *usbhid_dev_alloc(void)
{
	struct usbhid_dev *h;

	h = usbhid_dev_get_one();
	if (!h) {
		return NULL;
	}

	if (!h->inited) {
		h->inited = 1;
		h->state = 0;
		h->init = usbhid_init;
		h->usbhid_wait_connected;
		h->send = usbhid_send;
		h->send_nb = usbhid_send_nb;
		h->read = usbhid_read;
		h->read_nb = usbhid_read_nb;
		h->get_report_desc = usbhid_get_report_desc;
		h->get_config_desc = usbhid_get_config_desc;
		h->get_string_iproduct_desc = usbhid_get_string_iproduc_desc;
		h->get_string_interface_desc = usbhid_get_string_interface_desc;
		h->callback_set_report = usbhid_callback_set_report;
		h->callback_state_change = usbhid_callback_state_change;
		h->callback_request = usbhid_callback_request;
		h->callback_request_xfer_done = usbhid_callback_request_xfer_done;
		h->callback_set_configuration = usbhid_callback_set_configuration;
		h->callback_set_interface = usbhid_callback_set_interface;
	}

	usbhid_dev_set_active(h);

	return h;
}

void usbhid_dev_free(struct usbhid_dev *h)
{
	if (!h)
		return;

	if (h->inited) {
		//TODO:  check h->state
		h->inited = 0;
	}
}
