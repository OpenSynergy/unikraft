/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Costin Lupu <costin.lupu@cs.pub.ro>
 *          Simon Kuenzer <simon.kuenzer@neclab.eu>
 *
 * Copyright (c) 2018, NEC Europe Ltd., NEC Corporation. All rights reserved.
 * Copyright (c) 2021, NEC Laboratories Europe GmbH, NEC Corporation.
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <uk/config.h>
#include <uk/arch/ctx.h>
#if CONFIG_LIBUKDEBUG
#include <uk/assert.h>
#include <uk/print.h>
#else /* !CONFIG_LIBUKDEBUG */
#define UK_ASSERT(..) do {} while (0)
#define uk_pr_debug(..) do {} while (0)
#endif /* !CONFIG_LIBUKDEBUG */

extern void _ctx_x86_clearregs(void);
extern void _ctx_x86_call1(void);
extern void _ctx_x86_call2(void);

void ukarch_ctx_init_entry0(struct ukarch_ctx *ctx,
			    __uptr sp, int keep_regs,
			    ukarch_ctx_entry0 entry)
{
	UK_ASSERT(ctx);
	UK_ASSERT(!keep_regs && sp);	/* a stack is needed when clearing */
	UK_ASSERT(entry);		/* NULL as func will cause a crash */
	UK_ASSERT(!(sp & UKARCH_SP_ALIGN_MASK)); /* sp properly aligned? */

	if (keep_regs) {
		ukarch_ctx_init_bare(ctx, sp, (long) entry);
	} else {
		sp = ukarch_rstack_push(sp, (long) entry);
		ukarch_ctx_init_bare(ctx, sp, (long) _ctx_x86_clearregs);
	}

	uk_pr_debug("ukarch_ctx %p: entry:%p() sp:%p\n", ctx, entry, (void *) sp);
}

void ukarch_ctx_init_entry1(struct ukarch_ctx *ctx,
			    __uptr sp, int keep_regs,
			    ukarch_ctx_entry1 entry, long arg)
{
	UK_ASSERT(ctx);
	UK_ASSERT(sp);			/* a stack is needed */
	UK_ASSERT(entry);		/* NULL as func will cause a crash */
	UK_ASSERT(!(sp & UKARCH_SP_ALIGN_MASK)); /* sp properly aligned? */

	sp = ukarch_rstack_push(sp, (long) entry);
	sp = ukarch_rstack_push(sp, arg);
	if (keep_regs) {
		ukarch_ctx_init_bare(ctx, sp, (long) _ctx_x86_call1);
	} else {
		sp = ukarch_rstack_push(sp, (long) _ctx_x86_call1);
		ukarch_ctx_init_bare(ctx, sp, (long) _ctx_x86_clearregs);
	}

	uk_pr_debug("ukarch_ctx %p: entry:%p(%lx), sp:%p\n", ctx, entry, arg, (void *) sp);
}

void ukarch_ctx_init_entry2(struct ukarch_ctx *ctx,
			    __uptr sp, int keep_regs,
			    ukarch_ctx_entry2 entry, long arg0, long arg1)
{
	UK_ASSERT(ctx);
	UK_ASSERT(sp);			/* a stack is needed */
	UK_ASSERT(entry);		/* NULL as func will cause a crash */
	UK_ASSERT(!(sp & UKARCH_SP_ALIGN_MASK)); /* sp properly aligned? */

	sp = ukarch_rstack_push(sp, (long) entry);
	sp = ukarch_rstack_push(sp, arg0);
	sp = ukarch_rstack_push(sp, arg1);
	if (keep_regs) {
		ukarch_ctx_init_bare(ctx, sp, (long) _ctx_x86_call2);
	} else {
		sp = ukarch_rstack_push(sp, (long) _ctx_x86_call2);
		ukarch_ctx_init_bare(ctx, sp, (long) _ctx_x86_clearregs);
	}

	uk_pr_debug("ukarch_ctx %p: entry:%p(%lx, %lx), sp:%p\n", ctx, entry, arg0, arg1, (void *) sp);
}
