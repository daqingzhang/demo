#include <string.h>
#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <test2.h>

int add(int a, int c)
{
	return a+c;
}

int test_libusb(int argc, const char *argv[])
{
	libusb_device **devs;
	int r,i;
	ssize_t cnt;

	printf("%s\n", __func__);

	r = libusb_init(NULL);
	if (r) {
		printf("init libusb failed %d\n",r);
		return r;
	}

	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0) {
		printf("get libusb dev failed %d\n",r);
		return -1;
	}
	printf("usb devs cnt=%d\n", (int)cnt);

	struct libusb_device_descriptor desc;
	libusb_device_handle *handle;
	libusb_device *dev;
	char sbuf[256];

	for(i = 0; devs[i] != NULL; i++) {
		dev = devs[i];

		r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			printf("get dev desc failed %d", r);
			return -1;
		}
		printf("devs[%d]: vid=%04x, pid=%04x\n", i,
			(unsigned int)(desc.idVendor), (unsigned int)(desc.idProduct));

		handle = NULL;
		r = libusb_open(dev, &handle);
		if (r == LIBUSB_SUCCESS) {
			if (desc.iManufacturer) {
				r = libusb_get_string_descriptor_ascii(handle,
					desc.iManufacturer, sbuf, sizeof(sbuf));
				if (r > 0) {
					printf("Manu: %s\n", sbuf);
				}
			}

			if (desc.iProduct) {
				r = libusb_get_string_descriptor_ascii(handle,
					desc.iProduct, sbuf, sizeof(sbuf));
				if (r > 0) {
					printf("Product: %s\n", sbuf);
				}
			}
		} else {
			perror("libusb_open");
			printf("open usb dev failed %d\n", r);
		}
		if (handle) {
			libusb_close(handle);
		}
		printf("\n");
	}
	libusb_free_device_list(devs, 1);

	libusb_exit(NULL);

	return 0;
}

