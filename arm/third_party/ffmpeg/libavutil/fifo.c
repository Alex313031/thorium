/*
 * a very simple circular buffer FIFO implementation
 * Copyright (c) 2000, 2001, 2002 Fabrice Bellard
 * Copyright (c) 2006 Roman Shaposhnik
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <string.h>

#include "avassert.h"
#include "error.h"
#include "fifo.h"
#include "macros.h"
#include "mem.h"

// by default the FIFO can be auto-grown to 1MB
#define AUTO_GROW_DEFAULT_BYTES (1024 * 1024)

struct AVFifo {
    uint8_t *buffer;

    size_t elem_size, nb_elems;
    size_t offset_r, offset_w;
    // distinguishes the ambiguous situation offset_r == offset_w
    int    is_empty;

    unsigned int flags;
    size_t       auto_grow_limit;
};

AVFifo *av_fifo_alloc2(size_t nb_elems, size_t elem_size,
                       unsigned int flags)
{
    AVFifo *f;
    void *buffer = NULL;

    if (!elem_size)
        return NULL;

    if (nb_elems) {
        buffer = av_realloc_array(NULL, nb_elems, elem_size);
        if (!buffer)
            return NULL;
    }
    f = av_mallocz(sizeof(*f));
    if (!f) {
        av_free(buffer);
        return NULL;
    }
    f->buffer    = buffer;
    f->nb_elems  = nb_elems;
    f->elem_size = elem_size;
    f->is_empty  = 1;

    f->flags           = flags;
    f->auto_grow_limit = FFMAX(AUTO_GROW_DEFAULT_BYTES / elem_size, 1);

    return f;
}

void av_fifo_auto_grow_limit(AVFifo *f, size_t max_elems)
{
    f->auto_grow_limit = max_elems;
}

size_t av_fifo_elem_size(const AVFifo *f)
{
    return f->elem_size;
}

size_t av_fifo_can_read(const AVFifo *f)
{
    if (f->offset_w <= f->offset_r && !f->is_empty)
        return f->nb_elems - f->offset_r + f->offset_w;
    return f->offset_w - f->offset_r;
}

size_t av_fifo_can_write(const AVFifo *f)
{
    return f->nb_elems - av_fifo_can_read(f);
}

int av_fifo_grow2(AVFifo *f, size_t inc)
{
    uint8_t *tmp;

    if (inc > SIZE_MAX - f->nb_elems)
        return AVERROR(EINVAL);

    tmp = av_realloc_array(f->buffer, f->nb_elems + inc, f->elem_size);
    if (!tmp)
        return AVERROR(ENOMEM);
    f->buffer = tmp;

    // move the data from the beginning of the ring buffer
    // to the newly allocated space
    if (f->offset_w <= f->offset_r && !f->is_empty) {
        const size_t copy = FFMIN(inc, f->offset_w);
        memcpy(tmp + f->nb_elems * f->elem_size, tmp, copy * f->elem_size);
        if (copy < f->offset_w) {
            memmove(tmp, tmp + copy * f->elem_size,
                    (f->offset_w - copy) * f->elem_size);
            f->offset_w -= copy;
        } else
            f->offset_w = copy == inc ? 0 : f->nb_elems + copy;
    }

    f->nb_elems += inc;

    return 0;
}

static int fifo_check_space(AVFifo *f, size_t to_write)
{
    const size_t can_write = av_fifo_can_write(f);
    const size_t need_grow = to_write > can_write ? to_write - can_write : 0;
    size_t can_grow;

    if (!need_grow)
        return 0;

    can_grow = f->auto_grow_limit > f->nb_elems ?
               f->auto_grow_limit - f->nb_elems : 0;
    if ((f->flags & AV_FIFO_FLAG_AUTO_GROW) && need_grow <= can_grow) {
        // allocate a bit more than necessary, if we can
        const size_t inc = (need_grow < can_grow / 2 ) ? need_grow * 2 : can_grow;
        return av_fifo_grow2(f, inc);
    }

    return AVERROR(ENOSPC);
}

static int fifo_write_common(AVFifo *f, const uint8_t *buf, size_t *nb_elems,
                             AVFifoCB read_cb, void *opaque)
{
    size_t to_write = *nb_elems;
    size_t offset_w;
    int         ret = 0;

    ret = fifo_check_space(f, to_write);
    if (ret < 0)
        return ret;

    offset_w = f->offset_w;

    while (to_write > 0) {
        size_t    len = FFMIN(f->nb_elems - offset_w, to_write);
        uint8_t *wptr = f->buffer + offset_w * f->elem_size;

        if (read_cb) {
            ret = read_cb(opaque, wptr, &len);
            if (ret < 0 || len == 0)
                break;
        } else {
            memcpy(wptr, buf, len * f->elem_size);
            buf += len * f->elem_size;
        }
        offset_w += len;
        if (offset_w >= f->nb_elems)
            offset_w = 0;
        to_write -= len;
    }
    f->offset_w = offset_w;

    if (*nb_elems != to_write)
        f->is_empty = 0;
    *nb_elems -= to_write;

    return ret;
}

int av_fifo_write(AVFifo *f, const void *buf, size_t nb_elems)
{
    return fifo_write_common(f, buf, &nb_elems, NULL, NULL);
}

int av_fifo_write_from_cb(AVFifo *f, AVFifoCB read_cb,
                          void *opaque, size_t *nb_elems)
{
    return fifo_write_common(f, NULL, nb_elems, read_cb, opaque);
}

static int fifo_peek_common(const AVFifo *f, uint8_t *buf, size_t *nb_elems,
                            size_t offset, AVFifoCB write_cb, void *opaque)
{
    size_t  to_read = *nb_elems;
    size_t offset_r = f->offset_r;
    size_t can_read = av_fifo_can_read(f);
    int         ret = 0;

    if (offset > can_read || to_read > can_read - offset) {
        *nb_elems = 0;
        return AVERROR(EINVAL);
    }

    if (offset_r >= f->nb_elems - offset)
        offset_r -= f->nb_elems - offset;
    else
        offset_r += offset;

    while (to_read > 0) {
        size_t    len = FFMIN(f->nb_elems - offset_r, to_read);
        uint8_t *rptr = f->buffer + offset_r * f->elem_size;

        if (write_cb) {
            ret = write_cb(opaque, rptr, &len);
            if (ret < 0 || len == 0)
                break;
        } else {
            memcpy(buf, rptr, len * f->elem_size);
            buf += len * f->elem_size;
        }
        offset_r += len;
        if (offset_r >= f->nb_elems)
            offset_r = 0;
        to_read -= len;
    }

    *nb_elems -= to_read;

    return ret;
}

int av_fifo_read(AVFifo *f, void *buf, size_t nb_elems)
{
    int ret = fifo_peek_common(f, buf, &nb_elems, 0, NULL, NULL);
    av_fifo_drain2(f, nb_elems);
    return ret;
}

int av_fifo_read_to_cb(AVFifo *f, AVFifoCB write_cb,
                       void *opaque, size_t *nb_elems)
{
    int ret = fifo_peek_common(f, NULL, nb_elems, 0, write_cb, opaque);
    av_fifo_drain2(f, *nb_elems);
    return ret;
}

int av_fifo_peek(const AVFifo *f, void *buf, size_t nb_elems, size_t offset)
{
    return fifo_peek_common(f, buf, &nb_elems, offset, NULL, NULL);
}

int av_fifo_peek_to_cb(const AVFifo *f, AVFifoCB write_cb, void *opaque,
                       size_t *nb_elems, size_t offset)
{
    return fifo_peek_common(f, NULL, nb_elems, offset, write_cb, opaque);
}

void av_fifo_drain2(AVFifo *f, size_t size)
{
    const size_t cur_size = av_fifo_can_read(f);

    av_assert0(cur_size >= size);
    if (cur_size == size)
        f->is_empty = 1;

    if (f->offset_r >= f->nb_elems - size)
        f->offset_r -= f->nb_elems - size;
    else
        f->offset_r += size;
}

void av_fifo_reset2(AVFifo *f)
{
    f->offset_r = f->offset_w = 0;
    f->is_empty = 1;
}

void av_fifo_freep2(AVFifo **f)
{
    if (*f) {
        av_freep(&(*f)->buffer);
        av_freep(f);
    }
}


#if FF_API_FIFO_OLD_API
#include "internal.h"
FF_DISABLE_DEPRECATION_WARNINGS
#define OLD_FIFO_SIZE_MAX (size_t)FFMIN3(INT_MAX, UINT32_MAX, SIZE_MAX)

AVFifoBuffer *av_fifo_alloc_array(size_t nmemb, size_t size)
{
    AVFifoBuffer *f;
    void *buffer;

    if (nmemb > OLD_FIFO_SIZE_MAX / size)
        return NULL;

    buffer = av_realloc_array(NULL, nmemb, size);
    if (!buffer)
        return NULL;
    f = av_mallocz(sizeof(AVFifoBuffer));
    if (!f) {
        av_free(buffer);
        return NULL;
    }
    f->buffer = buffer;
    f->end    = f->buffer + nmemb * size;
    av_fifo_reset(f);
    return f;
}

AVFifoBuffer *av_fifo_alloc(unsigned int size)
{
    return av_fifo_alloc_array(size, 1);
}

void av_fifo_free(AVFifoBuffer *f)
{
    if (f) {
        av_freep(&f->buffer);
        av_free(f);
    }
}

void av_fifo_freep(AVFifoBuffer **f)
{
    if (f) {
        av_fifo_free(*f);
        *f = NULL;
    }
}

void av_fifo_reset(AVFifoBuffer *f)
{
    f->wptr = f->rptr = f->buffer;
    f->wndx = f->rndx = 0;
}

int av_fifo_size(const AVFifoBuffer *f)
{
    return (uint32_t)(f->wndx - f->rndx);
}

int av_fifo_space(const AVFifoBuffer *f)
{
    return f->end - f->buffer - av_fifo_size(f);
}

int av_fifo_realloc2(AVFifoBuffer *f, unsigned int new_size)
{
    unsigned int old_size = f->end - f->buffer;

    if (new_size > OLD_FIFO_SIZE_MAX)
        return AVERROR(EINVAL);

    if (old_size < new_size) {
        size_t offset_r = f->rptr - f->buffer;
        size_t offset_w = f->wptr - f->buffer;
        uint8_t *tmp;

        tmp = av_realloc(f->buffer, new_size);
        if (!tmp)
            return AVERROR(ENOMEM);

        // move the data from the beginning of the ring buffer
        // to the newly allocated space
        // the second condition distinguishes full vs empty fifo
        if (offset_w <= offset_r && av_fifo_size(f)) {
            const size_t copy = FFMIN(new_size - old_size, offset_w);
            memcpy(tmp + old_size, tmp, copy);
            if (copy < offset_w) {
                memmove(tmp, tmp + copy , offset_w - copy);
                offset_w -= copy;
            } else
                offset_w = old_size + copy;
        }

        f->buffer = tmp;
        f->end    = f->buffer + new_size;
        f->rptr   = f->buffer + offset_r;
        f->wptr   = f->buffer + offset_w;
    }
    return 0;
}

int av_fifo_grow(AVFifoBuffer *f, unsigned int size)
{
    unsigned int old_size = f->end - f->buffer;
    if(size + (unsigned)av_fifo_size(f) < size)
        return AVERROR(EINVAL);

    size += av_fifo_size(f);

    if (old_size < size)
        return av_fifo_realloc2(f, FFMAX(size, 2*old_size));
    return 0;
}

/* src must NOT be const as it can be a context for func that may need
 * updating (like a pointer or byte counter) */
int av_fifo_generic_write(AVFifoBuffer *f, void *src, int size,
                          int (*func)(void *, void *, int))
{
    int total = size;
    uint32_t wndx= f->wndx;
    uint8_t *wptr= f->wptr;

    if (size > av_fifo_space(f))
        return AVERROR(ENOSPC);

    do {
        int len = FFMIN(f->end - wptr, size);
        if (func) {
            len = func(src, wptr, len);
            if (len <= 0)
                break;
        } else {
            memcpy(wptr, src, len);
            src = (uint8_t *)src + len;
        }
        wptr += len;
        if (wptr >= f->end)
            wptr = f->buffer;
        wndx    += len;
        size    -= len;
    } while (size > 0);
    f->wndx= wndx;
    f->wptr= wptr;
    return total - size;
}

int av_fifo_generic_peek_at(AVFifoBuffer *f, void *dest, int offset, int buf_size, void (*func)(void*, void*, int))
{
    uint8_t *rptr = f->rptr;

    if (offset < 0 || buf_size > av_fifo_size(f) - offset)
        return AVERROR(EINVAL);

    if (offset >= f->end - rptr)
        rptr += offset - (f->end - f->buffer);
    else
        rptr += offset;

    while (buf_size > 0) {
        int len;

        if (rptr >= f->end)
            rptr -= f->end - f->buffer;

        len = FFMIN(f->end - rptr, buf_size);
        if (func)
            func(dest, rptr, len);
        else {
            memcpy(dest, rptr, len);
            dest = (uint8_t *)dest + len;
        }

        buf_size -= len;
        rptr     += len;
    }

    return 0;
}

int av_fifo_generic_peek(AVFifoBuffer *f, void *dest, int buf_size,
                         void (*func)(void *, void *, int))
{
    return av_fifo_generic_peek_at(f, dest, 0, buf_size, func);
}

int av_fifo_generic_read(AVFifoBuffer *f, void *dest, int buf_size,
                         void (*func)(void *, void *, int))
{
    if (buf_size > av_fifo_size(f))
        return AVERROR(EINVAL);

    do {
        int len = FFMIN(f->end - f->rptr, buf_size);
        if (func)
            func(dest, f->rptr, len);
        else {
            memcpy(dest, f->rptr, len);
            dest = (uint8_t *)dest + len;
        }
        av_fifo_drain(f, len);
        buf_size -= len;
    } while (buf_size > 0);
    return 0;
}

/** Discard data from the FIFO. */
void av_fifo_drain(AVFifoBuffer *f, int size)
{
    av_assert2(av_fifo_size(f) >= size);
    f->rptr += size;
    if (f->rptr >= f->end)
        f->rptr -= f->end - f->buffer;
    f->rndx += size;
}
FF_ENABLE_DEPRECATION_WARNINGS
#endif
