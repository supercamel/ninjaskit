#include "usart.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

UsartDriver::UsartDriver(uint32 usartn) : ringbuf(buffer, USART_RING_BUFFER_SIZE)
{
    usart = usartn;
}

void UsartDriver::begin(uint32 baud, uint32 bits, uint32 stopbits, uint32 parity)
{
    /* Setup GPIO pins */
    switch(usart)
    {
    case USART1:
    {
        rcc_periph_clock_enable(RCC_USART1);
        nvic_enable_irq(NVIC_USART1_IRQ);
        gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
        gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                      GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);
    }
    break;
    case USART2:
    {
        rcc_periph_clock_enable(RCC_USART2);
        nvic_enable_irq(NVIC_USART2_IRQ);
        gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);
        gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                      GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);
    }
    break;
    case USART3:
    {
        rcc_periph_clock_enable(RCC_USART3);
        nvic_enable_irq(NVIC_USART3_IRQ);
        gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART3_TX);
        gpio_set_mode(GPIOB, GPIO_MODE_INPUT,
                      GPIO_CNF_INPUT_FLOAT, GPIO_USART3_RX);
    }
    break;
    }

    /* Setup UART parameters. */
    usart_set_baudrate(usart, baud);
    usart_set_databits(usart, bits);
    usart_set_stopbits(usart, stopbits);
    usart_set_parity(usart, parity);
    usart_set_flow_control(usart, USART_FLOWCONTROL_NONE);
    usart_set_mode(usart, USART_MODE_TX_RX);

    USART_CR1(usart) |= USART_CR1_RXNEIE;

    /* Finally enable the USART. */
    usart_enable(usart);
}

void UsartDriver::end()
{
    usart_disable(usart);
}


void usart1_isr(void)
{
    /* Check if we were called because of RXNE. */
    if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) &&
            ((USART_SR(USART1) & USART_SR_RXNE) != 0))
    {
        Serial1.ringbuf.put(usart_recv(USART1));
    }
}

void usart2_isr(void)
{
    /* Check if we were called because of RXNE. */
    if (((USART_CR1(USART2) & USART_CR1_RXNEIE) != 0) &&
            ((USART_SR(USART2) & USART_SR_RXNE) != 0))
    {
        char c = usart_recv(USART2);
        Serial2.ringbuf.put(c);
    }
}

void usart3_isr(void)
{
    /* Check if we were called because of RXNE. */
    if (((USART_CR1(USART3) & USART_CR1_RXNEIE) != 0) &&
            ((USART_SR(USART3) & USART_SR_RXNE) != 0))
    {
        char c = usart_recv(USART3);
        Serial3.ringbuf.put(c);
    }
}


UsartDriver Serial1(USART1);
UsartDriver Serial2(USART2);
UsartDriver Serial3(USART3);


