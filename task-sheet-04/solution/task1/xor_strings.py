def xor_and_print_ascii(key, string):
    result = []
    for i in range(len(string)):
        xor_val = key[i % len(key)] ^ string[i]
        result.append(chr(xor_val))

    ascii_string = "".join(result)
    print("Resulting ASCII string:", ascii_string)


# Given lists
key = [
    0x78,
    0x63,
    0x6E,
    0x76,
    0x72,
    0x6A,
    0x66,
    0x6F,
    0x33,
    0x39,
    0x38,
    0x34,
    0x39,
    0x39,
    0x28,
    0x39,
    0x24,
    0x29,
    0x29,
]

string = [
    0x08,
    0x11,
    0x07,
    0x18,
    0x06,
    0x0C,
]

xor_and_print_ascii(key, string)
