/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#include "plat_internal.h"
#include <sel4/sel4.h>
#include <platsupport/chardev.h>

static int
debug_putchar(ps_chardevice_t* d UNUSED, int c)
{
#ifdef SEL4_DEBUG_KERNEL
    seL4_DebugPutChar(c);
#else
    (void)c;
#endif
    return 0;
}

static struct ps_chardevice console_device = {
    .putchar = &debug_putchar
};
static struct ps_chardevice* console = &console_device;


void
register_console(struct ps_chardevice* user_console)
{
    console = user_console;
}

void
__plat_serial_input_init_IRQ(void)
{
}

int
__plat_serial_init(ps_io_ops_t* io_ops)
{
    struct ps_chardevice temp_device;
    if(ps_cdev_init(PS_SERIAL_DEFAULT, io_ops, &temp_device)){
        /* Apply the changes */
#if defined(CONFIG_LIB_SEL4_PLAT_SUPPORT_USE_SEL4_DEBUG_PUTCHAR)
        temp_device.putchar = &debug_putchar;
#endif
        console_device = temp_device;
        console = &console_device;
        return 0;
    }else{
        return -1;
    }
}

void
__plat_putchar(int c)
{
    if(console){
        ps_cdev_putchar(console, c);
    }
}

int
__plat_getchar(void)
{
    if(console){
        return ps_cdev_getchar(console);
    }else{
        return EOF;
    }
}

