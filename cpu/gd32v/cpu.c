/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_gd32v
 * @{
 *
 * @file
 * @brief           GD32V CPU initialization
 *
 * @author          Koen Zandberg <koen@bergzand.net>
 */
#include "stdio_uart.h"
#include "periph/init.h"
#include "irq_arch.h"
#include "periph_cpu.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void periph_clk_en(bus_t bus, uint32_t mask)
{
    switch (bus) {
    case AHB:
        cpu_reg_enable_bits(&RCU->AHBEN, mask);
        break;
    case APB1:
        cpu_reg_enable_bits(&RCU->APB1EN, mask);
        break;
    case APB2:
        cpu_reg_enable_bits(&RCU->APB2EN, mask);
        break;
    default:
        DEBUG("unsupported bus %d\n", (int)bus);
        break;
    }
}

void periph_clk_dis(bus_t bus, uint32_t mask)
{
    switch (bus) {
    case AHB:
        cpu_reg_disable_bits(&RCU->AHBEN, mask);
        break;
    case APB1:
        cpu_reg_disable_bits(&RCU->APB1EN, mask);
        break;
    case APB2:
        cpu_reg_disable_bits(&RCU->APB2EN, mask);
        break;
    default:
        DEBUG("unsupported bus %d\n", (int)bus);
        break;
    }
}

uint32_t periph_apb_clk(bus_t bus)
{
    switch (bus) {
    case AHB:
        return CLOCK_AHB;
    case APB1:
        return CLOCK_APB1;
    case APB2:
        return CLOCK_APB2;
    }
    return 0;
}

extern void __libc_init_array(void);

void cpu_init(void)
{
    gd32vf103_clock_init();
    /* Common RISC-V initialization */
    riscv_init();
    stdio_init();
    periph_init();
}
