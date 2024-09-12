#include <stdint.h>
#include <stdio.h>
#include "gb.h"
#define __not_in_flash_func(func_name) func_name
#include "peanut_gb.h"
struct priv_t
{
    /* Pointer to allocated memory holding GB file. */
    uint8_t *rom;
    /* Pointer to allocated memory holding save file. */
    uint8_t *cart_ram;

    /* Frame buffer */
};
static struct priv_t priv;
enum gb_init_error_e ret;
static struct gb_s gb;


/**
 * Returns a byte from the ROM file at the given address.
 */
uint8_t *address = (uint8_t *)GB_FILE_ADDR;
uint8_t __not_in_flash_func(gb_rom_read)(struct gb_s *gb, const uint_fast32_t addr)
{
    // const struct priv_t * const p = gb->direct.priv;
    // const struct priv_t *const p = static_cast<const struct priv_t *>(gb->direct.priv);
    return address[addr];
}

/**
 * Returns a byte from the cartridge RAM at the given address.
 */
uint8_t __not_in_flash_func(gb_cart_ram_read)(struct gb_s *gb, const uint_fast32_t addr)
{
    // const struct priv_t * const p = gb->direct.priv;
    const struct priv_t *const p = (const struct priv_t *)(gb->direct.priv);
    return p->cart_ram[addr];
}

/**
 * Writes a given byte to the cartridge RAM at the given address.
 */

void __not_in_flash_func(gb_cart_ram_write)(struct gb_s *gb, const uint_fast32_t addr,
                                            const uint8_t val)
{
    // const struct priv_t * const p = gb->direct.priv;
    const struct priv_t *const p = (const struct priv_t *)(gb->direct.priv);
    p->cart_ram[addr] = val;
}

/**
 * Returns a pointer to the allocated space containing the ROM. Must be freed.
 */
uint8_t *read_rom_to_ram(const char *file_name)
{
    uint8_t *rom = NULL;
#if 0
	FILE *rom_file = fopen(file_name, "rb");
	size_t rom_size;
	

	if(rom_file == NULL)
		return NULL;

	fseek(rom_file, 0, SEEK_END);
	rom_size = ftell(rom_file);
	rewind(rom_file);
	rom = malloc(rom_size);

	if(fread(rom, sizeof(uint8_t), rom_size, rom_file) != rom_size)
	{
		free(rom);
		fclose(rom_file);
		return NULL;
	}

	fclose(rom_file);
	return rom;
#endif
    return rom;
}

/**
 * Ignore all errors.
 */
void gb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t val)
{
    const char *gb_err_str[GB_INVALID_MAX] = {
        "UNKNOWN",
        "INVALID OPCODE",
        "INVALID READ",
        "INVALID WRITE",
        "HALT FOREVER"};
    // struct priv_t *priv = gb->direct.priv;
    const struct priv_t *const priv =  (const struct priv_t *)(gb->direct.priv);
    printf("Error %d occurred: %s at %04X\n. Exiting.\n",
           gb_err, gb_err_str[gb_err], val);

    /* Free memory and then exit. */
    free(priv->cart_ram);
    free(priv->rom);
    exit(EXIT_FAILURE);
}
int startemulation(uint8_t *rom, char *errormessage) {
       static char ErrorMessage[40];
       ErrorMessage[0] = 0;
       errormessage = ErrorMessage;
       priv.rom = rom;
       int isFatalError = 1;
       priv.rom =rom;
        ret = gb_init(&gb, &gb_rom_read, &gb_cart_ram_read,
                      &gb_cart_ram_write, &gb_error, &priv);

        if (ret != GB_INIT_NO_ERROR)
        {
            snprintf(ErrorMessage, 40, "Cannot init emulator %d", ret);
            printf("%s\n", ErrorMessage);
            return 0;
        }
        uint32_t save_size = gb_get_save_size(&gb);
        printf("Allocating %d bytes for cart ram.\n", save_size);
        if (save_size > 0 && save_size <= 0x2000)
        {
            priv.cart_ram = (uint8_t *)malloc(save_size);
        }
        if (save_size > 0x2000)
        {
            strcpy(ErrorMessage, "Save size too large, max 8KB");
            printf("%s\n", ErrorMessage);
            return 0;
        }

        return isFatalError;

}