#include <iostream>
#include <nan.h>
#include "metrohash.h"

using namespace v8;
using namespace node;

// Helper functions.
static inline bool _is_buffer(Handle<Value> v) {
    return node::Buffer::HasInstance(v);
}

static inline Local<Object> _to_buffer(std::string s) {
    return Nan::CopyBuffer((char *) s.data(), s.size()).ToLocalChecked();
}

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
        if (! _is_buffer(info[0]) && ! info[0]->IsString()) {
            Nan::ThrowTypeError("`data` argument must be String or Buffer");
            return;
        }

        // Handle data argument.
        std::string data;
        if (_is_buffer(info[0])) {
            data = std::string(Buffer::Data(info[0]), Buffer::Length(info[0]));
        } else {
            data = std::string(*Nan::Utf8String(info[0]));
        }

        // Update hash.
        self->metro.Update(reinterpret_cast<const uint8_t *>( data.c_str() ), data.length());

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

        // Return as buffer.
        std::string hash(reinterpret_cast<char const *>(self->digest), DIGEST_SIZE);
        info.GetReturnValue().Set(_to_buffer(hash));
    }

    static NAN_METHOD(Hash) {
        Nan::HandleScope scope;
        NodeMetroHash* self = Nan::ObjectWrap::Unwrap<NodeMetroHash>(info.This());

        // Argument validation.
        if (info.Length() != 1) {
            Nan::ThrowTypeError("Missing argument");
            return;
        }
        if (! _is_buffer(info[0]) && ! info[0]->IsString()) {
            Nan::ThrowTypeError("`data` argument must be String or Buffer");
            return;
        }

        // Handle data argument.
        std::string data;
        if (_is_buffer(info[0])) {
            data = std::string(Buffer::Data(info[0]), Buffer::Length(info[0]));
        } else {
            data = std::string(*Nan::Utf8String(info[0]));
        }

        // One-shot hash.
        uint8_t digest[DIGEST_SIZE];
        self->metro.Hash(reinterpret_cast<const uint8_t *>( data.c_str() ), data.length(), digest, self->seed);

        // Return as buffer.
        std::string hash(reinterpret_cast<char const *>(digest), DIGEST_SIZE);
        info.GetReturnValue().Set(_to_buffer(hash));
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

// Addon initialization.
NAN_MODULE_INIT(InitAll) {
    NodeMetroHash64::Init("MetroHash64", target);
    NodeMetroHash128::Init("MetroHash128", target);
}

NODE_MODULE(metrohash, InitAll)
