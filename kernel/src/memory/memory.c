#include "../util/panic.h"
#include "../bootservices/bootservices.h"
#include "../util/string.h"
#include "memory.h"
#include "memory_cb.h"

#define IS_ALIGNED(x) ((((uint64_t)(x) & (PAGE_SIZE - 1))) == 0)
#define ALIGN_ADDR(x) (((PAGE_SIZE - 1) & (x)) ? ((x + PAGE_SIZE) & ~(PAGE_SIZE - 1)) : (x))
#define ADDR_TO_PAGE(x) ((uint64_t)(x) >> 12)
#define SET_PAGE_BIT(x) ((memory->bitfield[(x) >> 3] |= ((x) % 8)))
#define CLEAR_PAGE_BIT(x) ((memory->bitfield[(x) >> 3] &= ~(1 << (x) % 8)))
#define GET_PAGE_BIT(x) ((memory->bitfield[(x) >> 3] & (1 << (x) % 8)) >> ((x) % 8))

struct system_memory *memory;

void set_page_bit(void *address)
{
    if (ADDR_TO_PAGE(address) > memory->total_available_pages)
    {
        panic("set_page_bit: address out of bounds");
    }
    SET_PAGE_BIT(ADDR_TO_PAGE(address));
    if (GET_PAGE_BIT(ADDR_TO_PAGE(address)) == 0)
    {
        panic("set_page_bit: SET_PAGE_BIT failed");
    }
}

void clear_page_bit(void *address)
{
    if (ADDR_TO_PAGE(address) > memory->total_available_pages)
    {
        panic("clear_page_bit: address out of bounds");
    }
    CLEAR_PAGE_BIT(ADDR_TO_PAGE(address));
}

uint8_t get_page_bit(void *address)
{
    if (ADDR_TO_PAGE(address) > memory->total_available_pages)
    {
        panic("get_page_bit: address out of bounds");
    }
    return (uint8_t)(ADDR_TO_PAGE(address));
}

void reserve_page(void *address)
{
    if (get_page_bit(address) == 1)
        panic("reserve_page: page already reserved");
    set_page_bit(address);
    memory->free_memory -= PAGE_SIZE;
    memory->reserved_memory += PAGE_SIZE;
}

void unreserve_page(void *address)
{
    if (get_page_bit(address) == 0)
        panic("unreserve_page: page not reserved");
    clear_page_bit(address);
    memory->free_memory += PAGE_SIZE;
    memory->reserved_memory -= PAGE_SIZE;
}

void lock_page(void *address)
{
    if (get_page_bit(address) == 1)
        panic("lock_page: page already locked");
    set_page_bit(address);
    memory->free_memory -= PAGE_SIZE;
    memory->locked_memory += PAGE_SIZE;
}
void *unlock_page(void *address)
{
    if (get_page_bit(address) == 0)
        panic("unlock_page: page not locked");
    clear_page_bit(address);
    memory->free_memory += PAGE_SIZE;
    memory->locked_memory -= PAGE_SIZE;
}
void *request_page(void *address)
{
    static uint64_t last_requested;
    static uint64_t statistics_count;
    uint64_t first = last_requested - 1;

    while (GET_PAGE_BIT(last_requested) == 1)
    {
        if (last_requested >= memory->total_available_pages)
            last_requested = 0;
        if (last_requested == first)
            panic("request_page: no free pages");
        last_requested++;
    }
    lock_page((void *)(last_requested << 12));

    if (GET_PAGE_BIT(last_requested) == 0)
        panic("request_page: lock_page failed");

    statistics_count++;

    return (void *)(memory->first_available_page_addr + (last_requested << 12));
}

void free_page(void *address)
{
    uint64_t addr_uint = (uint64_t)address;
    uint64_t normalized_addr = addr_uint - memory->first_available_page_addr;

    if (addr_uint < memory->first_available_page_addr)
        panic("free_page: address out of bounds");
    if (GET_PAGE_BIT(normalized_addr >> 12) == 0)
        panic("free_page: page not locked");

    unlock_page(address);

    if (GET_PAGE_BIT(normalized_addr >> 12) == 1)
        panic("free_page: unlock_page failed");
}

void *request_contiguous_pages(uint64_t pages)
{
    static uint64_t last_requested;
    static uint64_t statistics_count;
    uint64_t first = last_requested - 1;
    uint64_t last = last_requested - 1;

    while (GET_PAGE_BIT(last_requested) == 1)
    {
        if (last_requested >= memory->total_available_pages)
            last_requested = 0;
        if (last_requested == first)
            panic("request_contiguous_pages: no free pages");
        last_requested++;
    }

    for (uint64_t i = 0; i < pages; i++)
    {
        if (GET_PAGE_BIT(last_requested) == 1)
        {
            panic("request_contiguous_pages: page already locked");
        }
        lock_page((void *)(last_requested << 12));
        last_requested++;
    }

    statistics_count++;
    return (void *)(memory->first_available_page_addr + (last << 12));
}
void free_contiguous_pages(void *address, uint64_t pages)
{
    uint64_t addr_uint = (uint64_t)address;
    uint64_t normalized_addr = addr_uint - memory->first_available_page_addr;

    if (addr_uint < memory->first_available_page_addr)
        panic("free_contiguous_pages: address out of bounds");
    if (GET_PAGE_BIT(normalized_addr >> 12) == 0)
        panic("free_contiguous_pages: page not locked");

    for (uint64_t i = 0; i < pages; i++)
    {
        if (GET_PAGE_BIT((normalized_addr >> 12) + i) == 0)
            panic("free_contiguous_pages: page not locked");
        unlock_page((void *)(normalized_addr + (i << 12)));
    }
}

void reserve_pages(void *address, uint64_t pages)
{
    if ((uint64_t)address < 0)
        panic("reserve_pages: address out of bounds");
    for (uint64_t i = 0; i < pages; i++)
    {
        reserve_page((void *)((uint64_t)address + (i << 12)));
    }
}

void unreserve_pages(void *address, uint64_t pages)
{
    if ((uint64_t)address < 0)
        panic("unreserve_pages: address out of bounds");
    for (uint64_t i = 0; i < pages; i++)
    {
        unreseve_page((void *)((uint64_t)address + (i << 12)));
    }
}

void lock_pages(void *address, uint64_t pages)
{
    if ((uint64_t)address < 0)
        panic("lock_pages: address out of bounds");
    for (uint64_t i = 0; i < pages; i++)
    {
        lock_page((void *)((uint64_t)address + (i << 12)));
    }
}

void unlock_pages(void *address, uint64_t pages)
{
    if ((uint64_t)address < 0)
        panic("unlock_pages: address out of bounds");
    for (uint64_t i = 0; i < pages; i++)
    {
        unlock_page((void *)((uint64_t)address + (i << 12)));
    }
}

void iterate_memory(void (*callback)(void *, uint64_t, uint64_t, uint64_t), void *global_override)
{
    uint64_t memory_entries = get_memory_map_entries();

    for (uint64_t index = 0; index < memory_entries; index++)
    {
        uint64_t base = get_memory_map_base(index);
        uint64_t length = get_memory_map_length(index);
        uint64_t type = get_memory_map_type(index);
        callback(global_override, base, length, type);
    }
}

struct system_memory *test_get_memory()
{
    return memory;
}

uint64_t get_total_memory()
{
    static uint64_t total_memory;
    if (total_memory > 0)
        return total_memory;
    iterate_memory(get_total_memory_cb, (void *)&total_memory);
    return total_memory;
}

int init_memory()
{
    static struct chunk_data biggest_avail_chunk;
    if (biggest_avail_chunk.size > 0)
        return 0;

    uint64_t total_memory = get_total_memory();
    uint64_t total_pages = total_memory >> 12;

    iterate_memory(init_memory_cb, (void *)&biggest_avail_chunk);

    uint64_t first_availiable_address = ALIGN_ADDR(biggest_avail_chunk.addr);
    uint64_t last_availiable_address = ALIGN_ADDR(biggest_avail_chunk.addr + biggest_avail_chunk.size);
    uint64_t availiable_pages = (last_availiable_address - first_availiable_address) >> 12;
    uint64_t bitfield_entries = ((availiable_pages % 8) == 0 ? (availiable_pages >> 3) : ((availiable_pages >> 3) + 1));

    if (biggest_avail_chunk.size < (bitfield_entries + sizeof(struct system_memory) + (3 << 12)))
    {
        panic("Not enough memory to initialize memory manager\n");
    }

    memory = (struct system_memory *)first_availiable_address;
    memory->bitfield = (uint8_t *)ALIGN_ADDR(first_availiable_address + sizeof(struct system_memory *));

    memset(memory->bitfield, 0x55, 100);

    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (j % 2 == 1)
            {
                if (GET_PAGE_BIT(i * 8 + j) == 0)
                    panic("init memory: GET_PAGE_BIT failed");
            }
            else
            {
                if (GET_PAGE_BIT(i * 8 + j) == 1)
                    panic("init memory: GET_PAGE_BIT failed");
            }
        }
    }

    memset(memory->bitfield, 0x0, bitfield_entries);

    memory->total_available_pages = availiable_pages;
    memory->total_pages = total_pages;
    memory->total_memory = total_memory;
    memory->free_memory = last_availiable_address - first_availiable_address;
    memory->locked_memory = 0;
    memory->reserved_memory = 0;

    memory->first_available_page_addr = first_availiable_address;
    memory->last_available_page_addr = last_availiable_address;

    if (!IS_ALIGNED(memory) || !IS_ALIGNED(memory->bitfield) || !IS_ALIGNED(memory->first_available_page_addr))
        panic("Memory alignment error!");

    uint64_t used_space = (bitfield_entries + sizeof(struct system_memory) + (3 << 12)) >> 12;

    reserve_pages((void *)0, used_space);

    return 1;
}