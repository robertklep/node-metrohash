#include <nan.h>
#include "metrohash.h"

using namespace v8;
using namespace node;

// Helper functions.
static inline bool _is_buffer(Handle<Value> v) {
    return node::Buffer::HasInstance(v);
}

static inline Local<Object> _to_buffer(std::string s) {
    return NanNewBufferHandle((char *) s.data(), s.size());
}

// Generic wrapper for all hashing methods.
Local<Value> Hash(_NAN_METHOD_ARGS, uint64_t len, void (*HashMethod)(const uint8_t *, uint64_t, uint32_t, uint8_t *)) {
    // Argument validation.
    if (args.Length() < 1) {
        return NanError("Wrong number of arguments");
    }
    if (! _is_buffer(args[0]) && ! args[0]->IsString()) {
        return NanError("`data` argument must be String or Buffer");
    }

    // Handle seed argument.
    uint32_t seed = 0;
    if (args.Length() >= 2) {
        if (! args[1]->IsNumber()) {
            return NanError("`seed` argument must be a Number");
        }
        seed = args[1]->ToInt32()->Value();
    }

    // Handle data argument.
    std::string data;
    if (_is_buffer(args[0])) {
        data = std::string(Buffer::Data(args[0]), Buffer::Length(args[0]));
    } else {
        data = std::string(*NanUtf8String(args[0]));
    }

    // Hash it.
    uint8_t out[len];
    HashMethod(reinterpret_cast<const uint8_t *>( data.c_str() ), data.length(), seed, out);

    // Return as buffer.
    std::string hash(reinterpret_cast<char const *>(out), len);
    return _to_buffer(hash);
}

NAN_METHOD(_metrohash64_1) {
    NanScope();
    Local<Value> ret = Hash(args, 8, metrohash64_1);

    if (ret->IsNativeError()) {
        NanThrowTypeError(*NanUtf8String(ret));
        NanReturnUndefined();
    } else {
        NanReturnValue(ret);
    }
}

NAN_METHOD(_metrohash64_2) {
    NanScope();
    Local<Value> ret = Hash(args, 8, metrohash64_2);

    if (ret->IsNativeError()) {
        NanThrowTypeError(*NanUtf8String(ret));
        NanReturnUndefined();
    } else {
        NanReturnValue(ret);
    }
}

NAN_METHOD(_metrohash128_1) {
    NanScope();
    Local<Value> ret = Hash(args, 16, metrohash128_1);

    if (ret->IsNativeError()) {
        NanThrowTypeError(*NanUtf8String(ret));
        NanReturnUndefined();
    } else {
        NanReturnValue(ret);
    }
}

NAN_METHOD(_metrohash128_2) {
    NanScope();
    Local<Value> ret = Hash(args, 16, metrohash128_2);

    if (ret->IsNativeError()) {
        NanThrowTypeError(*NanUtf8String(ret));
        NanReturnUndefined();
    } else {
        NanReturnValue(ret);
    }
}

NAN_METHOD(_metrohash128crc_1) {
    NanScope();
    Local<Value> ret = Hash(args, 16, metrohash128crc_1);

    if (ret->IsNativeError()) {
        NanThrowTypeError(*NanUtf8String(ret));
        NanReturnUndefined();
    } else {
        NanReturnValue(ret);
    }
}

NAN_METHOD(_metrohash128crc_2) {
    NanScope();
    Local<Value> ret = Hash(args, 16, metrohash128crc_2);

    if (ret->IsNativeError()) {
        NanThrowTypeError(*NanUtf8String(ret));
        NanReturnUndefined();
    } else {
        NanReturnValue(ret);
    }
}

// Addon initialization.
void InitAll(Handle<Object> exports) {
    exports->Set(NanNew("metrohash64_1"), NanNew<FunctionTemplate>(_metrohash64_1)->GetFunction());
    exports->Set(NanNew("metrohash64_2"), NanNew<FunctionTemplate>(_metrohash64_2)->GetFunction());
    exports->Set(NanNew("metrohash128_1"), NanNew<FunctionTemplate>(_metrohash128_1)->GetFunction());
    exports->Set(NanNew("metrohash128_2"), NanNew<FunctionTemplate>(_metrohash128_2)->GetFunction());
    exports->Set(NanNew("metrohash128crc_1"), NanNew<FunctionTemplate>(_metrohash128crc_1)->GetFunction());
    exports->Set(NanNew("metrohash128crc_2"), NanNew<FunctionTemplate>(_metrohash128crc_2)->GetFunction());
}

NODE_MODULE(metrohash, InitAll)
