#ifndef SEC_HACC_EXPORT_H
#define SEC_HACC_EXPORT_H

// ========================================================
// HACC USER
// ========================================================
typedef enum{
    HACC_USER1 = 0,
    HACC_USER2,
    HACC_USER3
} HACC_USER;

// ========================================================
// SECURE DRIVER HACC FUNCTION
// ========================================================
extern unsigned int sec_hacc_blk_sz(void);

/**
 * sec_hacc_derive() - hardware-based key derivation function (KDF)
 *
 * @buf        Pointer to the buffer requested to be derived.
 * @size       Size of the buffer requested to be derived.
 * @user       User type used to select specific inital vector(IV).
 * @return     Return 0: means KDF process success.
 *             Return 1: means not support on the platform.
 *             Any other return values for processing error.
 *
 */
extern unsigned int sec_hacc_derive(unsigned char *buf, unsigned int size, HACC_USER user);

#endif
