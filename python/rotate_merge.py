__all__ = ['rotate_merge_sort', 'sort']

MIN_RUN = 8


def insertion_sort(l: list, a: int, b: int):
    i = a + 1
    if l[i - 1] > l[i]:
        i += 1
        while i < b and l[i - 1] > l[i]:
            i += 1
        j = a
        k = i - 1
        while j < k:
            l[j], l[k] = l[k], l[j]
            j += 1
            k -= 1
    else:
        i += 1
        while i < b and l[i - 1] <= l[i]:
            i += 1

    while i < b:
        tmp = l[i]
        pos = i - 1
        pos1 = i
        while pos1 > a and l[pos] > tmp:
            l[pos1] = l[pos]
            pos1 -= 1
            pos -= 1
        l[pos1] = tmp
        i += 1


def rotate_in_place(l: list, a: int, b: int, c: int, d: int):
    c = b
    b -= 1
    d -= 1

    while a < b and c < d:
        tmp = l[b]
        l[b] = l[a]
        b -= 1
        l[a] = l[c]
        a += 1
        l[c] = l[d]
        c += 1
        l[d] = tmp
        d -= 1
    while a < b:
        tmp = l[b]
        l[b] = l[a]
        b -= 1
        l[a] = l[d]
        a += 1
        l[d] = tmp
        d -= 1
    while c < d:
        tmp = l[c]
        l[c] = l[d]
        c += 1
        l[d] = l[a]
        d -= 1
        l[a] = tmp
        a += 1
    while a < d:
        l[a], l[d] = l[d], l[a]
        a += 1
        d -= 1


def rotate(l: list, pos: int, left: int, right: int, ext: list):
    if left < 1 or right < 1:
        return

    a = pos
    b = pos + left
    c = pos + right
    d = b + right

    if left < right:
        bridge = right - left
        if bridge < left:
            if bridge > len(ext):
                rotate_in_place(l, a, b, c, d)
            loop = left
            ext[:bridge] = l[b:b + bridge]
            while loop:
                loop -= 1
                c -= 1
                d -= 1
                b -= 1
                l[c] = l[d]
                l[d] = l[b]
            l[a:a + bridge] = ext[:bridge]
        else:
            if left > len(ext):
                rotate_in_place(l, a, b, c, d)
            ext[:left] = l[a:a + left]
            l[a:a + right] = l[b:b + right]
            l[c:c + left] = ext[:left]
    elif right < left:
        bridge = left - right
        if bridge < right:
            if bridge > len(ext):
                rotate_in_place(l, a, b, c, d)
            loop = right
            ext[:bridge] = l[c:c + bridge]
            while loop:
                loop -= 1
                l[c] = l[a]
                l[a] = l[b]
                c += 1
                a += 1
                b += 1
            l[d - bridge:d] = ext[:bridge]
        else:
            if right > len(ext):
                rotate_in_place(l, a, b, c, d)
            ext[:right] = l[b:b + right]
            while left:
                left -= 1
                d -= 1
                b -= 1
                l[d] = l[b]
            l[a:a + right] = ext[:right]
    else:
        while left:
            left -= 1
            l[a], l[b] = l[b], l[a]
            a += 1
            b += 1


def monobound_left(l: list, start: int, end: int, val):
    top = end - start

    while top > 1:
        mid = top // 2

        if val <= l[end - mid]:
            end -= mid
        top -= mid

    if val <= l[end - 1]:
        return end - 1
    return end


def monobound_right(l: list, start: int, end: int, val):
    top = end - start

    while top > 1:
        mid = top // 2

        if l[start + mid] <= val:
            start += mid
        top -= mid

    if l[start] <= val:
        return start + 1
    return start


def left_exp_search(l: list, a: int, b: int, val):
    i = 1
    while a - 1 + i < b and val >= l[a - 1 + i]:
        i *= 2
    
    return monobound_right(l, a + i // 2, min(b, a - 1 + i), val)


def right_exp_search(l: list, a: int, b: int, val):
    i = 1
    while b - i >= a and val <= l[b - i]:
        i *= 2

    return monobound_left(l, max(a, b - i + 1), b - i // 2, val)


def merge_up(l: list, start: int, mid: int, end: int, ext: list):
    ext[:mid - start] = l[start:mid]

    buf = 0
    left = start
    right = mid

    while left < right and right < end:
        if ext[buf] > l[right]:
            l[left] = l[right]
            right += 1
        else:
            l[left] = ext[buf]
            buf += 1
        left += 1

    while left < right:
        l[left] = ext[buf]
        left += 1
        buf += 1


def merge_down(l: list, start: int, mid: int, end: int, ext: list):
    ext[:end - mid] = l[mid:end]

    buf = end - mid - 1
    left = mid - 1
    right = end - 1

    start -= 1
    while right > left and left > start:
        if ext[buf] < l[left]:
            l[right] = l[left]
            left -= 1
        else:
            l[right] = ext[buf]
            buf -= 1
        right -= 1
    
    while right > left:
        l[right] = ext[buf]
        right -= 1
        buf -= 1


def merge(l: list, start: int, mid: int, end: int, ext: list):
    if start >= mid:
        return
    if l[start] > l[end - 1]:
        rotate(l, start, mid - start, end - mid, ext)
        return
    end = right_exp_search(l, mid, end, l[mid - 1])
    if end < mid:
        return
    start = left_exp_search(l, start, mid, l[mid])
    llen = mid - start
    rlen = end - mid
    if llen > len(ext) or rlen > len(ext):
        if llen >= rlen:
            m1 = start + llen // 2
            m2 = monobound_left(l, mid, end, l[m1])
            m3 = m1 + (m2 - mid)
        else:
            m2 = mid + rlen // 2
            m1 = monobound_right(l, start, mid, l[m2])
            m3 = m2 - (mid - m1)
            m2 += 1
        rotate(l, m1, mid - m1, m2 - mid, ext)
        merge(l, m3 + 1, m2, end, ext)
        merge(l, start, m1, m3, ext)
    else:
        if rlen < llen:
            merge_down(l, start, mid, end, ext)
        else:
            merge_up(l, start, mid, end, ext)


def rotate_merge_sort(l: list, array: int, length: int, ext: list):
    end = array + length

    i = array
    while i + MIN_RUN < end:
        insertion_sort(l, i, i + MIN_RUN)
        i += MIN_RUN
    if i + 1 < end:
        insertion_sort(l, i, end)

    gap = MIN_RUN
    while gap < length:
        full_merge = 2 * gap
        i = array
        while i + full_merge < end:
            merge(l, i, i + gap, i + full_merge, ext)
            i += full_merge
        if i + gap < end:
            merge(l, i, i + gap, end, ext)
        gap = full_merge


def sort(l: list):
    rotate_merge_sort(l, 0, len(l), [None] * 256)
