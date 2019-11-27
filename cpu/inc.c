#include "mos6502.h"

static int inc_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    cpu->a = mos6502_read8(cpu, (uint16_t)zp_address);
    int value = ++cpu->a;
    mos6502_write8(cpu, (uint16_t)zp_address, value);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 5;
}

static int inc_zero_page_x(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    uint8_t add_address = cpu->read(cpu, cpu->pc++);
    uint8_t final_address = zp_address + add_address;

    cpu->a = mos6502_read8(cpu, (uint16_t)final_address);
    int value = ++cpu->a;
    mos6502_write8(cpu, (uint16_t)final_address, value);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 6;
}

static int inc_absolute(mos6502_t *cpu)
{
    uint8_t low = cpu->read(cpu, cpu->pc++);
    uint8_t high = cpu->read(cpu, cpu->pc++);
    uint16_t final_address = (high << 8) | low;

    cpu->a = mos6502_read16(cpu, final_address);
    int value = ++cpu->a;
    mos6502_write8(cpu, (uint16_t)final_address, value);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 6;
}

static int inc_absolute_x(mos6502_t *cpu)
{
    uint8_t low = cpu->read(cpu, cpu->pc++);
    uint8_t high = cpu->read(cpu, cpu->pc++);
    uint8_t increment = cpu->read(cpu, cpu->pc++);
    uint16_t address = (high << 8) | low;
    uint16_t final_address = address + (uint16_t)increment;

    cpu->a = mos6502_read16(cpu, final_address);
    int value = ++cpu->a;

    mos6502_write8(cpu, (uint16_t)final_address, value);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 7;
}

void mos6502_register_inc(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0xE6, inc_zero_page);
    mos6502_register_opcode(cpu, 0xF6, inc_zero_page_x);
    mos6502_register_opcode(cpu, 0xEE, inc_absolute);
    mos6502_register_opcode(cpu, 0xFE, inc_absolute_x);
}

#ifdef _TEST
//REGULAR

static int test_inc_zero_page(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0055, 0x55);
    mos6502_write8(cpu, 0x8000, 0xE6);
    mos6502_write8(cpu, 0x8001, 0x55);
    int ticks = mos6502_tick(cpu);
    return ticks == 5 && cpu->a == 0x56 && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_inc_zero_page_x(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0056, 0x55);
    mos6502_write8(cpu, 0x8000, 0xF6);
    mos6502_write8(cpu, 0x8001, 0x55);
    mos6502_write8(cpu, 0x8002, 0x01);
    int ticks = mos6502_tick(cpu);
    return ticks == 6 && cpu->a == 0x56 && cpu->pc == 0x8003 && cpu->flags == 0;
}

static int test_inc_absolute(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x1234, 0x55);
    mos6502_write8(cpu, 0x8000, 0xEE);
    mos6502_write8(cpu, 0x8001, 0x34);
    mos6502_write8(cpu, 0x8002, 0x12);
    int ticks = mos6502_tick(cpu);

    return ticks = 6 && cpu->a == 0x56 && cpu->pc == 0x8003 && cpu->flags == 0;
}

static int test_inc_absolute_x(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x1333, 0x55);
    mos6502_write8(cpu, 0x8000, 0xFE);
    mos6502_write8(cpu, 0x8001, 0x34);
    mos6502_write8(cpu, 0x8002, 0x12);
    mos6502_write8(cpu, 0x8003, 0xff);
    int ticks = mos6502_tick(cpu);

    return ticks = 7 && cpu->a == 0x56 && cpu->pc == 0x8004 && cpu->flags == 0;
}

//ZERO

static int test_inc_zero_page_zero_flag(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0055, 0xFF);
    mos6502_write8(cpu, 0x8000, 0xE6);
    mos6502_write8(cpu, 0x8001, 0x55);
    int ticks = mos6502_tick(cpu);
    return ticks == 5 && cpu->a == 0x00 && cpu->pc == 0x8002 && cpu->flags == ZERO;
}

static int test_inc_zero_page_x_zero_flag(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0056, 0xFF);
    mos6502_write8(cpu, 0x8000, 0xF6);
    mos6502_write8(cpu, 0x8001, 0x55);
    mos6502_write8(cpu, 0x8002, 0x01);
    int ticks = mos6502_tick(cpu);
    return ticks == 6 && cpu->a == 0x00 && cpu->pc == 0x8003 && cpu->flags == ZERO;
}

static int test_inc_absolute_zero_flag(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x1234, 0xFF);
    mos6502_write8(cpu, 0x8000, 0xEE);
    mos6502_write8(cpu, 0x8001, 0x34);
    mos6502_write8(cpu, 0x8002, 0x12);
    int ticks = mos6502_tick(cpu);

    return ticks = 6 && cpu->a == 0x00 && cpu->pc == 0x8003 && cpu->flags == ZERO;
}

static int test_inc_absolute_x_zero_flag(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x1333, 0xFF);
    mos6502_write8(cpu, 0x8000, 0xFE);
    mos6502_write8(cpu, 0x8001, 0x34);
    mos6502_write8(cpu, 0x8002, 0x12);
    mos6502_write8(cpu, 0x8003, 0xff);
    int ticks = mos6502_tick(cpu);

    return ticks = 7 && cpu->a == 0x00 && cpu->pc == 0x8004 && cpu->flags == ZERO;
}

void test_mos6502_inc()
{
    RUN_TEST(test_inc_zero_page);
    RUN_TEST(test_inc_zero_page_x);
    RUN_TEST(test_inc_absolute);
    RUN_TEST(test_inc_absolute_x);

    RUN_TEST(test_inc_zero_page_zero_flag);
    RUN_TEST(test_inc_zero_page_x_zero_flag);
    RUN_TEST(test_inc_absolute_zero_flag);
    RUN_TEST(test_inc_absolute_x_zero_flag);
}
#endif