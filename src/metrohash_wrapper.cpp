#include <iostream>
#include <nan.h>
#include "metrohash.h"

using namespace v8;
using namespace node;

template <typename HASHER, int DIGEST_SIZE>
class NodeMetroHash : public Nan::ObjectWrap {
    static Nan::Persistent<Function> constructor;
    HASHER                           metro;
    uint64_t                         seed;
    uint8_t                          digest[DIGEST_SIZE];
    bool                             wasFinalized = false;

    explicit NodeMetroHash(uint64_t seed = 0) : seed(seed) {
        metro.Initialize(seed);
    }

    static NAN_METHOD(New) {
        Nan::HandleScope scope;
        if (info.IsConstructCall()) {
            // TODO: check if argument type makes sense
            NodeMetroHash* hash = new NodeMetroHash(info[0]->IsUndefined() ? 0 : info[0]->NumberValue());
            hash->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
        } else {
            const int argc = 1;
            Local<Value> argv[argc] = { info[0] };
            Local<Function> cons = Nan::New<Function>(constructor);
            info.GetReturnValue().Set(cons->NewInstance(argc, argv));
        }
    }

    static NAN_METHOD(Update) {
        Nan::HandleScope scope;
        NodeMetroHash* self = Nan::ObjectWrap::Unwrap<NodeMetroHash>(info.This());

        // Argument validation.
        if (info.Length() != 1) {
            Nan::ThrowTypeError("Missing argument");
            return;
        }

        // Determine data and size of content to be hashed.
        char*  data;
        size_t size;
        if (Buffer::HasInstance(info[0])) {
            data = Buffer::Data(info[0]);
            size = Buffer::Length(info[0]);
        } else if (info[0]->IsString()) {
            std::string str = *Nan::Utf8String(info[0]);
            data = (char *) str.c_str();
            size = str.length();
        } else {
            Nan::ThrowTypeError("`data` argument must be String or Buffer");
            return;
        }

        // Update hash.
        self->metro.Update((const uint8_t *) data, size);

        // Allow for chaining.
        info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Digest) {
        Nan::HandleScope scope;
        NodeMetroHash* self = Nan::ObjectWrap::Unwrap<NodeMetroHash>(info.This());

        // Finalize and get the hash value. Make sure we only finalize once.
        if (! self->wasFinalized) {
            self->metro.Finalize(self->digest);
            self->wasFinalized = true;
        }

        // Return as hex string.
        info.GetReturnValue().Set(Nan::Encode((char *) self->digest, DIGEST_SIZE, Nan::HEX));
    }

    static NAN_METHOD(Hash) {
        Nan::HandleScope scope;
        NodeMetroHash* self = Nan::ObjectWrap::Unwrap<NodeMetroHash>(info.This());

        // Argument validation.
        if (info.Length() != 1) {
            Nan::ThrowTypeError("Missing argument");
            return;
        }

        // Determine data and size of content to be hashed.
        char*  data;
        size_t size;
        if (Buffer::HasInstance(info[0])) {
            data = Buffer::Data(info[0]);
            size = Buffer::Length(info[0]);
        } else if (info[0]->IsString()) {
            std::string str = *Nan::Utf8String(info[0]);
            data = (char *) str.c_str();
            size = str.length();
        } else {
            Nan::ThrowTypeError("`data` argument must be String or Buffer");
            return;
        }

        // One-shot hash.
        uint8_t digest[DIGEST_SIZE];
        self->metro.Hash((const uint8_t * ) data, size, digest, self->seed);

        // Return as hex string.
        info.GetReturnValue().Set(Nan::Encode((char *) digest, DIGEST_SIZE, Nan::HEX));
    }

public:
    static void Init(const char *js_class_name, Handle<Object> target) {
        Nan::HandleScope scope;
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);

        // Prepare constructor template
        tpl->SetClassName(Nan::New(js_class_name).ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        // Prototype
        Nan::SetPrototypeMethod(tpl, "update", Update);
        Nan::SetPrototypeMethod(tpl, "digest", Digest);
        Nan::SetPrototypeMethod(tpl, "hash",   Hash);

        constructor.Reset(tpl->GetFunction());
        Nan::Set(target, Nan::New(js_class_name).ToLocalChecked(), tpl->GetFunction());
    }
};

template <typename HASHER, int DIGEST_SIZE>
Nan::Persistent<Function> NodeMetroHash<HASHER, DIGEST_SIZE>::constructor;

typedef NodeMetroHash<MetroHash64,   8>  NodeMetroHash64;
typedef NodeMetroHash<MetroHash128, 16> NodeMetroHash128;

// Standalone hashers
#define STANDALONE_HASHER(BITS) \
    NAN_METHOD(metrohash ## BITS) { \
        uint8_t digest[ BITS >> 3 ]; \
 \
        if (info.Length() != 1) { \
            Nan::ThrowTypeError("Missing argument"); \
            return; \
        } \
        char  *data; \
        size_t size; \
        if (Buffer::HasInstance(info[0])) { \
            data = Buffer::Data(info[0]); \
            size = Buffer::Length(info[0]); \
        } else if (info[0]->IsString()) { \
            std::string str = *Nan::Utf8String(info[0]); \
            data = (char *) str.c_str(); \
            size = str.length(); \
        } else { \
            Nan::ThrowTypeError("`data` argument must be String or Buffer"); \
            return; \
        } \
        MetroHash ## BITS::Hash((const unsigned char *) data, size, digest, 0); \
        info.GetReturnValue().Set(Nan::Encode((char *) digest, BITS >> 3, Nan::HEX)); \
    };

STANDALONE_HASHER(64)
STANDALONE_HASHER(128)

// Addon initialization.
NAN_MODULE_INIT(InitAll) {
    // Export classes.
    NodeMetroHash64::Init("MetroHash64", target);
    NodeMetroHash128::Init("MetroHash128", target);

    // Export standalone hashing functions
    Local<v8::Function> fn64   = Nan::GetFunction(Nan::New<FunctionTemplate>(metrohash64)).ToLocalChecked();
    Local<v8::String>   name64 = Nan::New("metrohash64").ToLocalChecked();
    fn64->SetName(name64);
    Nan::Set(target, name64, fn64);

    Local<v8::Function> fn128   = Nan::GetFunction(Nan::New<FunctionTemplate>(metrohash128)).ToLocalChecked();
    Local<v8::String>   name128 = Nan::New("metrohash128").ToLocalChecked();
    fn128->SetName(name128);
    Nan::Set(target, name128, fn128);
}

NODE_MODULE(metrohash, InitAll)
