static inline VAR* FUNC(rotateMax)(VAR* a, VAR* b) {
    return (a > b) ? a : b;
}

static inline VAR* FUNC(rotateMin)(VAR* a, VAR* b) {
    return (a < b) ? a : b;
}

static void FUNC(rotateInsertionSort)(VAR* a, VAR* b, MERGECMP cmp) {
    VAR* i = a + 1, *j, *k, tmp, *pos, *pos1;
    if (cmp(i - 1, i) > 0) {
        i++;
        while (i < b && cmp(i - 1, i) > 0) i++;
        j = a;
        k = i - 1;
        while (j < k) {
            tmp = *j;
            *j++ = *k;
            *k-- = tmp;
        }
    } else {
        i++;
        while (i < b && cmp(i - 1, i) <= 0) i++;
    }

    while (i < b) {
        tmp = *i;
        pos = i - 1;
        pos1 = i;
        while (pos1 > a && cmp(pos, &tmp) > 0) {
            *pos1-- = *pos--;
        }
        *pos1 = tmp;
        i++;
    }
}

static void FUNC(rotateRotate)(VAR* pos, size_t left, size_t right, VAR* ext) {
    if (left < 1 || right < 1) return;

    VAR *a = pos, *b = pos + left, *c = pos + right, *d = b + right, tmp;

    size_t bridge, loop;
    if (left < right) {
        bridge = right - left;

        if (bridge < left) {
            if (bridge > EXTLEN) {
                goto rotateInPlace;
            }

            loop = left;

            memcpy(ext, b, bridge * sizeof(VAR));

            while (loop--) {
                *--c = *--d;
                *  d = *--b;
            }
            memcpy(a, ext, bridge * sizeof(VAR));
        } else {
            if (left > EXTLEN) {
                goto rotateInPlace;
            }

            memcpy(ext, a, left * sizeof(VAR));
            memmove(a, b, right * sizeof(VAR));
            memcpy(c, ext, left * sizeof(VAR));
        }
    } else if (right < left) {
        bridge = left - right;

        if (bridge < right) {
            if (bridge > EXTLEN) {
                goto rotateInPlace;
            }

            loop = right;

            memcpy(ext, c, bridge * sizeof(VAR));
            
            while (loop--) {
                *c++ = *a;
                *a++ = *b++;
            }
            memcpy(d - bridge, ext, bridge * sizeof(VAR));
        } else {
            if (right > EXTLEN) {
                goto rotateInPlace;
            }

            memcpy(ext, b, right * sizeof(VAR));
            while (left--) {
                *--d = *--b;
            }
            memcpy(a, ext, right * sizeof(VAR));
        }
    } else {
        while (left--) {
            tmp = *a;
            *a++ = *b;
            *b++ = tmp;
        }
    }
    return;

    rotateInPlace:;
    c = b;
    b--;
    d--;

    while (a < b && c < d) {
        tmp = *b;
        *b-- = *a;
        *a++ = *c;
        *c++ = *d;
        *d-- = tmp;
    }
    while (a < b) {
        tmp = *b;
        *b-- = *a;
        *a++ = *d;
        *d-- = tmp;
    }
    while (c < d) {
        tmp = *c;
        *c++ = *d;
        *d-- = *a;
        *a++ = tmp;
    }
    while (a < d) {
        tmp = *a;
        *a++ = *d;
        *d-- = tmp;
    }
}

static VAR* FUNC(rotateMonoboundLeft)(VAR* start, VAR* end, VAR* val, MERGECMP cmp) {
    size_t top, mid;

    top = end - start;

    while (top > 1) {
        mid = top / 2;

        if (cmp(val, end - mid) <= 0) {
            end -= mid;
        }
        top -= mid;
    }

    if (cmp(val, end - 1) <= 0) {
        return end - 1;
    }
    return end;
}

static VAR* FUNC(rotateMonoboundRight)(VAR* start, VAR* end, VAR* val, MERGECMP cmp) {
    size_t top, mid;

    top = end - start;

    while (top > 1) {
        mid = top / 2;

        if (cmp(start + mid, val) <= 0) {
            start += mid;
        }
        top -= mid;
    }

    if (cmp(start, val) <= 0) {
        return start + 1;
    }
    return start;
}

static VAR* FUNC(rotateLeftExpSearch)(VAR* a, VAR* b, VAR* val, MERGECMP cmp) {
    size_t i = 1;
    while (a - 1 + i < b && cmp(val, a - 1 + i) >= 0) i *= 2;

    return FUNC(rotateMonoboundRight)(a + i / 2, FUNC(rotateMin)(b, a - 1 + i), val, cmp);
}

static VAR* FUNC(rotateRightExpSearch)(VAR* a, VAR* b, VAR* val, MERGECMP cmp) {
    size_t i = 1;
    while (b - i >= a && cmp(val, b - i) <= 0) i *= 2;

    return FUNC(rotateMonoboundLeft)(FUNC(rotateMax)(a, b - i + 1), b - i / 2, val, cmp);
}

static void FUNC(rotateMergeUp)(VAR* start, VAR* mid, VAR* end, VAR* ext, MERGECMP cmp) {
    memcpy(ext, start, (mid - start) * sizeof(VAR));

    VAR *left = start, *right = mid;

    while (left < right && right < end) {
        if (cmp(ext, right) > 0)
            *left++ = *right++;
        else
            *left++ = *ext++;
    }

    while (left < right)
        *left++ = *ext++;
}

static void FUNC(rotateMergeDown)(VAR* start, VAR* mid, VAR* end, VAR* ext, MERGECMP cmp) {
    memcpy(ext, mid, (end - mid) * sizeof(VAR));

    VAR* buf = end - mid - 1 + ext;
    VAR *left = mid - 1, *right = end - 1;

    start--;
    while (right > left && left > start) {
        if (cmp(buf, left) < 0)
            *right-- = *left--;
        else
            *right-- = *buf--;
    }

    while (right > left)
        *right-- = *buf--;
}

static void FUNC(rotateMerge)(VAR* start, VAR* mid, VAR* end, VAR* ext, MERGECMP cmp) {
    if (start >= mid) return;
    if (cmp(start, end - 1) > 0) {
        FUNC(rotateRotate)(start, mid - start, end - mid, ext);
        return;
    }
    end = FUNC(rotateRightExpSearch)(mid, end, mid - 1, cmp);
    if (end < mid) return;
    start = FUNC(rotateLeftExpSearch)(start, mid, mid, cmp);
    size_t llen = mid - start, rlen = end - mid;
    if (((llen < rlen) ? llen : rlen) > EXTLEN) {
        VAR *m1, *m2, *m3;
        if (llen >= rlen) {
            m1 = start + (llen) / 2;
            m2 = FUNC(rotateMonoboundLeft)(mid, end, m1, cmp);
            m3 = m1 + (m2 - mid);
        } else {
            m2 = mid + (rlen) / 2;
            m1 = FUNC(rotateMonoboundRight)(start, mid, m2, cmp);
            m3 = (m2++) - (mid - m1);
        }
        FUNC(rotateRotate)(m1, mid - m1, m2 - mid, ext);
        FUNC(rotateMerge)(m3 + 1, m2, end, ext, cmp);
        FUNC(rotateMerge)(start, m1, m3, ext, cmp);
    } else {
        if (rlen < llen) {
            FUNC(rotateMergeDown)(start, mid, end, ext, cmp);
        } else {
            FUNC(rotateMergeUp)(start, mid, end, ext, cmp);
        }
    }
}

static void FUNC(rotateMergeSort)(VAR* array, size_t length, VAR* ext, MERGECMP cmp) {
    VAR* end = array + length;

    VAR* i;
    for (i = array; i + MIN_RUN < end; i += MIN_RUN) {
        FUNC(rotateInsertionSort)(i, i + MIN_RUN, cmp);
    }
    if (i + 1 < end) {
        FUNC(rotateInsertionSort)(i, end, cmp);
    }

    size_t gap, fullMerge;
    for (gap = MIN_RUN; gap < length; gap = fullMerge) {
        fullMerge = 2 * gap;
        for (i = array; i + fullMerge < end; i += fullMerge) {
            FUNC(rotateMerge)(i, i + gap, i + fullMerge, ext, cmp);
        }
        if (i + gap < end) {
            FUNC(rotateMerge)(i, i + gap, end, ext, cmp);
        }
    }
}
