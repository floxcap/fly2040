#ifndef FLY2040_CONSTANTS_H
#define FLY2040_CONSTANTS_H

// Remaining RgbState size = 255 bytes - header items (mode, num_pixels etc).
#define RGB_CBOR_DATA_LEN 216

enum RgbMode
{
    RgbMode_Off,
    RgbMode_Pulse,
    RgbMode_Cycle,
    RgbMode_Rainbow,
    RgbMode_CpuLoad,
    RgbMode_GpuLoad,
    RgbMode_CpuGpuLoad,
    RgbMode_Temperature,
    RgbMode_Wren
};

// Register mapping for i2c device.
typedef struct __attribute__((__packed__)) {
    uint8_t mode;
    uint8_t num_pixels;
    uint16_t version;
    uint32_t brightness;
    uint32_t speed;
    uint32_t hue;
    uint32_t temp;
    uint32_t cpu;
    uint32_t gpu;
    uint32_t res0;
    uint32_t res1;
    uint8_t rd_cbor;
    uint8_t wr_cbor;
    uint8_t len_cbor;
    uint8_t data_cbor[RGB_CBOR_DATA_LEN];
#ifdef __cplusplus
#endif
} RgbState;

typedef union
{
    RgbState state;
    uint8_t mem[256];
} RgbRegister;

// Size of struct member.
#ifndef SIZEOF
#define SIZEOF(s,m) ((size_t) sizeof(((s *)0)->m))
#endif

#define RGB_OFFSET(m)  offsetof(RgbState,m)
#define RGB_SIZEOF(m)  SIZEOF(RgbState,m)

#define MAX_PIXELS 50
#define I2C_SLAVE_ADDRESS   0x52//0x17

//see:
// libstratosphere/source/i2c/driver/board/nintendo/nx/i2c_bus_device_map.inc
// DeviceCode_I2c1
#define I2C_BAUDRATE 100000 // 100 kHz

#ifndef MIN
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#endif

#ifndef MAX
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#endif

#ifndef CLAMP
#define CLAMP(x, lower, upper) (MIN((upper), MAX((x), (lower))))
#endif

#ifndef U32F_SCALE
#define U32F_SCALE  10000
#endif

#ifndef F2U32
#define F2U32(f)    ((uint32_t)(f*(float)U32F_SCALE))
#endif

#ifndef U322F
#define U322F(u)    ((float)u/(float)U32F_SCALE)
#endif

//TDODO ? check __FLOAT_WORD_ORDER__
#if defined(__BYTE_ORDER__)
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        #define SET_32(item, val) item = val
        #define GET_32(item) item
    #else
        #warning "Convert endian"
        #define SET_32(item, val) item = __builtin_bswap32(val)
        #define GET_32(item) __builtin_bswap32(item)
    #endif
#else
#error "Unknown endian"
#endif

#endif //FLY2040_CONSTANTS_H
