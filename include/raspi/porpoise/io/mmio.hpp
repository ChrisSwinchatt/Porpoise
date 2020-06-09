#pragma once

#include <stdint.h>

namespace porpoise { namespace io {
    struct mmio
    {
#if __raspi__ == 0 || __raspi__ == 1
        static constexpr uintptr_t MMIO_BASE = 0x20000000;
#elif __raspi__ == 2 || __raspi__ == 3
        static constexpr uintptr_t MMIO_BASE = 0x3F000000;
#elif __raspi__ == 4
        static constexpr uintptr_t MMIO_BASE = 0xFE000000;
#else
#error Unknown/undefined Raspberry Pi version
#endif
        static constexpr uintptr_t GPIO_BASE    = MMIO_BASE + 0x200000;
        static constexpr uintptr_t UART0_BASE   = GPIO_BASE + 0x1000;
        static constexpr uintptr_t MAILBOX_BASE = MMIO_BASE + 0xB880;
        enum class reg : uintptr_t
        {
            GPIO_UP_DOWN   = GPIO_BASE    + 0x94,
            GPIO_CLOCK0    = GPIO_BASE    + 0x98,
            UART0_DR       = UART0_BASE   + 0x00,
            UART0_RSRECR   = UART0_BASE   + 0x04,
            UART0_FR       = UART0_BASE   + 0x18,
            UART0_ILPR     = UART0_BASE   + 0x20,
            UART0_IBRD     = UART0_BASE   + 0x24,
            UART0_FBRD     = UART0_BASE   + 0x28,
            UART0_LCRH     = UART0_BASE   + 0x2C,
            UART0_CR       = UART0_BASE   + 0x30,
            UART0_IFLS     = UART0_BASE   + 0x34,
            UART0_IMSC     = UART0_BASE   + 0x38,
            UART0_RIS      = UART0_BASE   + 0x3C,
            UART0_MIS      = UART0_BASE   + 0x40,
            UART0_ICR      = UART0_BASE   + 0x44,
            UART0_DMACR    = UART0_BASE   + 0x48,
            UART0_ITCR     = UART0_BASE   + 0x80,
            UART0_ITIP     = UART0_BASE   + 0x84,
            UART0_ITOP     = UART0_BASE   + 0x88,
            UART0_TDR      = UART0_BASE   + 0x8C,
            MAILBOX_READ   = MAILBOX_BASE + 0x00,
            MAILBOX_STATUS = MAILBOX_BASE + 0x18,
            MAILBOX_WRITE  = MAILBOX_BASE + 0x20
        };

        static void put(reg r, uint32_t value);

        static uint32_t get(reg r);
    private:
        mmio() = delete;
    };
}} // porpose::io
