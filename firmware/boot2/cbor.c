
#include "cbor.h"

bool handle_cbor(uint8_t* data_cbor, uint8_t* len_cbor)
{
    nanocbor_value_t it;
    nanocbor_decoder_init(&it, data_cbor, *len_cbor);

    // First byte is always RPC method id.
    uint8_t method_id;
    nanocbor_get_uint8(&it, &method_id);

}