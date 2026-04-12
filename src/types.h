/*
 * types.h - Tipe data dasar untuk IF-OS
 *
 * File ini sudah disiapkan oleh asisten praktikum.
 * JANGAN diubah.
 */
#ifndef TYPES_H
#define TYPES_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef signed int int32_t;
typedef unsigned long long uint64_t;

#define NULL ((void *)0)
#define true 1
#define false 0

/* Ukuran satu halaman memori (4 KB) */
#define PGSIZE 4096
#define PGBITS 12

/* Bulatkan alamat ke bawah ke page boundary */
#define pg_round_down(va) ((void *)((uint32_t)(va) & ~(uint32_t)(PGSIZE - 1)))

/* offsetof tanpa <stddef.h> — gunakan GCC builtin */
#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER)

/* Thread identifier */
typedef int tid_t;
#define TID_ERROR ((tid_t) - 1)

#endif /* TYPES_H */
