/*
 * Copyright (c) 2016 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <misc/printk.h>
#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/storage.h>

#include <soc.h>
#include <tc_util.h>

#include "device.h"

/* Any by default, can change depending on the hardware implementation */
static bt_addr_le_t bt_addr;

static ssize_t storage_read(const bt_addr_le_t *addr, uint16_t key, void *data,
			       size_t length)
{
	if (addr) {
		return -ENOENT;
	}

	if (key == BT_STORAGE_ID_ADDR && length == sizeof(bt_addr)) {
		bt_addr_le_copy(data, &bt_addr);
		return sizeof(bt_addr);
	}

	return -EIO;
}

static ssize_t storage_write(const bt_addr_le_t *addr, uint16_t key,
				const void *data, size_t length)
{
	return -ENOSYS;
}

static ssize_t storage_clear(const bt_addr_le_t *addr)
{
	return -ENOSYS;
}

static void set_own_bt_addr(void)
{
	int i;
	uint8_t tmp;

	/*
	 * Generate a static BT addr using the unique
	 * product number generated in set_device_id()
	 */
	for (i = 0; i < 4; i++) {
		tmp = (product_id.number >> i * 8) & 0xff;
		bt_addr.a.val[i] = tmp;
	}

	bt_addr.a.val[4] = 0xe7;
	bt_addr.a.val[5] = 0xd6;
}

int bt_storage_init(void)
{
	static const struct bt_storage storage = {
		.read = storage_read,
		.write = storage_write,
		.clear = storage_clear,
	};

	bt_addr.type = BT_ADDR_LE_RANDOM;

	set_own_bt_addr();

	bt_storage_register(&storage);

	printk("Bluetooth storage driver registered\n");

	TC_END_RESULT(TC_PASS);

	return 0;
}
