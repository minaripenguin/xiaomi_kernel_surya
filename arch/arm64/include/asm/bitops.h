/*
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __ASM_BITOPS_H
#define __ASM_BITOPS_H

#include <linux/atomic.h>
#include <linux/compiler.h>
#ifdef CONFIG_LTO
#include <asm/barrier.h>
#endif

#ifndef _LINUX_BITOPS_H
#error only <linux/bitops.h> can be included directly
#endif

#ifdef CONFIG_LTO
/*
 * Little endian assembly atomic bitops.
 * Use bitops.S variants with LTO to avoid
 * a regression.
 */
extern void set_bit(int nr, volatile unsigned long *p);
extern void clear_bit(int nr, volatile unsigned long *p);
extern void change_bit(int nr, volatile unsigned long *p);
extern int test_and_set_bit(int nr, volatile unsigned long *p);
extern int test_and_clear_bit(int nr, volatile unsigned long *p);
extern int test_and_change_bit(int nr, volatile unsigned long *p);
#else
/*
 * Use rewritten atomics when LTO not enabled.
 * To, again, avoid regression.
 * This is a terrbile hack and I am ashamed.
 */
static inline void set_bit(unsigned int nr, volatile unsigned long *p)
{
	p += BIT_WORD(nr);
	atomic_long_or(BIT_MASK(nr), (atomic_long_t *)p);
}

static inline void clear_bit(unsigned int nr, volatile unsigned long *p)
{
	p += BIT_WORD(nr);
	atomic_long_andnot(BIT_MASK(nr), (atomic_long_t *)p);
}

static inline void change_bit(unsigned int nr, volatile unsigned long *p)
{
	p += BIT_WORD(nr);
	atomic_long_xor(BIT_MASK(nr), (atomic_long_t *)p);
}

static inline int test_and_set_bit(unsigned int nr, volatile unsigned long *p)
{
	long old;
	unsigned long mask = BIT_MASK(nr);

	p += BIT_WORD(nr);
	if (READ_ONCE(*p) & mask)
		return 1;

	old = atomic_long_fetch_or(mask, (atomic_long_t *)p);
	return !!(old & mask);
}

static inline int test_and_clear_bit(unsigned int nr, volatile unsigned long *p)
{
	long old;
	unsigned long mask = BIT_MASK(nr);

	p += BIT_WORD(nr);
	if (!(READ_ONCE(*p) & mask))
		return 0;

	old = atomic_long_fetch_andnot(mask, (atomic_long_t *)p);
	return !!(old & mask);
}

static inline int test_and_change_bit(unsigned int nr, volatile unsigned long *p)
{
	long old;
	unsigned long mask = BIT_MASK(nr);

	p += BIT_WORD(nr);
	old = atomic_long_fetch_xor(mask, (atomic_long_t *)p);
	return !!(old & mask);
}
#endif

#include <asm-generic/bitops/builtin-__ffs.h>
#include <asm-generic/bitops/builtin-ffs.h>
#include <asm-generic/bitops/builtin-__fls.h>
#include <asm-generic/bitops/builtin-fls.h>

#include <asm-generic/bitops/ffz.h>
#include <asm-generic/bitops/fls64.h>
#include <asm-generic/bitops/find.h>

#include <asm-generic/bitops/sched.h>
#include <asm-generic/bitops/hweight.h>
#include <asm-generic/bitops/lock.h>

#include <asm-generic/bitops/non-atomic.h>
#include <asm-generic/bitops/le.h>
#include <asm-generic/bitops/ext2-atomic-setbit.h>

#endif /* __ASM_BITOPS_H */
