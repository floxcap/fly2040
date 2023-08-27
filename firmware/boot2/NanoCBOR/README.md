# NanoCBOR

NanoCBOR is a tiny [CBOR](https://tools.ietf.org/html/rfc7049) library aimed at embedded and heavily constrained devices.
It is optimized for 32 bit architectures but should run fine on 8 bit and 16 bit architectures.
NanoCBOR is optimized for decoding known CBOR structures while optimizing the flash footprint of both NanoCBOR and the code using NanoCBOR.

The decoder of NanoCBOR should compile to 600-800 bytes on a Cortex-M0+ MCU, depending on whether floating point decoding is required.

## Usage

To achieve the small code size, two patterns are used throughout the decode library.

 - Every decode call will first check the type and refuse to decode if the CBOR element is not of the required type.
 - Every decode call will, on succesfull decode, advance the decode context to the next CBOR element.

This allows using code to call decode functions and check the return code of the function without requiring an if value of type, decode value, advance to next item dance, and requiring only a single call to decode an expected type and advance to the next element.

Start the decoding of a buffer with:

```C
nanocbor_value_t decoder;
nanocbor_decoder_init(&decoder, buffer, buffer_len);
```

Where `buffer` is an `const uint8_t` array containing an CBOR structure.

To decode an `int32_t` from a cbor structure and bail out if the element is not of the integer type:

```C
int32_t value = 0;
if (nanocbor_get_int32(&decoder, &value) < 0) {
  return ERR_INVALID_STRUCTURE;
}
return use_value(value);
```

Iterating over an CBOR array and calling a function passing every element is as simple as:

```C
nanocbor_value_t arr; /* Array value instance */

if (nanocbor_enter_array(&decoder, &arr) < 0) {
    return ERR_INVALID_STRUCTURE;
}
while (!nanocbor_at_end(&arr)) {
    handle_array_element(&arr);
}
```

Decoding a map is similar to an array, except that every map entry consists of two CBOR elements requiring separate decoding.
For example, a map using integers as keys and strings as values can be decoded with:

```C
while (!nanocbor_at_end(&map)) {
    int32_t key;
    const char *value;
    size_t value_len;
    if (nanocbor_get_int32(&map, &integer_key) < 0) {
        return ERR_INVALID_STRUCTURE;
    }
    if (nanocbor_get_tstr(&map, &value, &value_len) < 0) {
        return ERR_INVALID_STRUCTURE;
    }
    handle_map_element(key, value, value_len);
}
```


### Dependencies:

Only dependency are two functions to provide endian conversion.
These are not provided by the library and have to be configured in the header file.
On a bare metal ARM platform, `__builtin_bswap64` and `__builtin_bswap32` can be used for this conversion.

### Contributing

Open an issue, PR, the usual.

