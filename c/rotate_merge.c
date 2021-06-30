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

static void FUNC(rotateMerge)(VAR* start, VAR* mid, VAR* end, MERGECMP cmp) {

}

static void FUNC(rotateMergeSort)(VAR* array, size_t length, VAR* tmp, MERGECMP cmp) {
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
            FUNC(rotateMerge)(i, i + gap, i + fullMerge, cmp);
        }
        if (i + gap < end) {
            FUNC(rotateMerge)(i, i + gap, end, cmp);
        }
    }
}
