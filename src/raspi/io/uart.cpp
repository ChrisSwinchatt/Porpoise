#include <stdint.h>

#include <porpoise/io/mmio.hpp>
#include <porpoise/io/uart.hpp>
#include <porpoise/time/delay.hpp>

static constexpr uint32_t UART_CLOCK_RATE = 3000000;

static volatile uint32_t __attribute__((aligned(16))) mailbox[9] = {
    9*4,
    0,
    0x38002,
    12,
    8,
    2,
    UART_CLOCK_RATE,
    0,
    0
};

namespace porpoise { namespace io {
    using namespace porpoise::time;

    uart uart::_instance;

    uart* uart::init(baud_rate baud)
    {
        if (_instance._baud == baud)
        {
            return &_instance;
        }

        // Disable UART0.
        mmio::put(mmio::reg::UART0_CR,     0);

        // Set up pins 14 & 15.
        mmio::put(mmio::reg::GPIO_UP_DOWN, 0);
        delay::cycles(150);
        mmio::put(mmio::reg::GPIO_CLOCK0, (1 << 14) | (1 << 15));
        delay::cycles(150);
        mmio::put(mmio::reg::GPIO_CLOCK0, 0);

        // Clear pending interrupts.
        mmio::put(mmio::reg::UART0_ICR,   0x7FF);

        // Ensure 3 MHz clock rate on Raspberry Pi 3+.
        if (__raspi__ >= 3)
        {
            auto r = (reinterpret_cast<uintptr_t>(&mailbox) & ~0xF) | 8;
            while (mmio::get(mmio::reg::MAILBOX_STATUS) & 0x80000000)
                ;
            mmio::put(mmio::reg::MAILBOX_WRITE, r);
            while (mmio::get(mmio::reg::MAILBOX_STATUS) & 0x40000000 || mmio::get(mmio::reg::MAILBOX_READ) != r)
                ;
        }

        // Send divisor.
        switch (baud)
        {
            case baud_rate::bd9600:
                mmio::put(mmio::reg::UART0_IBRD, 19);
                mmio::put(mmio::reg::UART0_FBRD, 35);
                break;
            case baud_rate::bd115200:
                mmio::put(mmio::reg::UART0_IBRD, 1);
                mmio::put(mmio::reg::UART0_FBRD, 40);
                break;
            default:
                return nullptr;
        }

        // Set 8-bit FIFO mode with 1 stop bit & no parity.
        mmio::put(mmio::reg::UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

        // Mask interrupts.
        mmio::put(
            mmio::reg::UART0_IMSC,
            (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10)
        );

        mmio::put(mmio::reg::UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));

        return &_instance;
    }

    void uart::put(uint8_t value)
    {
        while (mmio::get(mmio::reg::UART0_FR) & (1 << 5))
            ;
        mmio::put(mmio::reg::UART0_DR, value);
    }

    uint8_t uart::get()
    {
        while (mmio::get(mmio::reg::UART0_FR) & (1 << 5))
            ;
        return mmio::get(mmio::reg::UART0_DR);
    }
}} // porpoise::io
