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
    return NanNewBufferHandle((char *) s.data(), s.size());
}

template <typename HASHER, int DIGEST_SIZE>
class NodeMetroHash : public ObjectWrap {
    static Persistent<Function> constructor;
    HASHER                      metro;
    uint8_t                     digest[DIGEST_SIZE];
    bool                        wasFinalized = false;

    explicit NodeMetroHash(uint64_t seed = 0) {
        metro.Initialize(seed);
    }

    static NAN_METHOD(New) {
        NanScope();
        if (args.IsConstructCall()) {
            // TODO: check if argument type makes sense
            NodeMetroHash* hash = new NodeMetroHash(args[0]->IsUndefined() ? 0 : args[0]->NumberValue());
            hash->Wrap(args.This());
            NanReturnValue(args.This());
        } else {
            const int argc = 1;
            Local<Value> argv[argc] = { args[0] };
            Local<Function> cons = NanNew<Function>(constructor);
            NanReturnValue(cons->NewInstance(argc, argv));
        }
    }

    static NAN_METHOD(Update) {
        NanScope();
        NodeMetroHash* self = ObjectWrap::Unwrap<NodeMetroHash>(args.This());

        // Argument validation.
        if (args.Length() != 1) {
            NanThrowTypeError("Missing argument");
            NanReturnUndefined();
        }
        if (! _is_buffer(args[0]) && ! args[0]->IsString()) {
            NanThrowTypeError("`data` argument must be String or Buffer");
            NanReturnUndefined();
        }

        // Handle data argument.
        std::string data;
        if (_is_buffer(args[0])) {
            data = std::string(Buffer::Data(args[0]), Buffer::Length(args[0]));
        } else {
            data = std::string(*NanUtf8String(args[0]));
        }

        // Update hash.
        self->metro.Update(reinterpret_cast<const uint8_t *>( data.c_str() ), data.length());

        // Allow for chaining.
        NanReturnValue(args.This());
    }

    static NAN_METHOD(Digest) {
        NanScope();
        NodeMetroHash* self = ObjectWrap::Unwrap<NodeMetroHash>(args.This());

        // Finalize and get the hash value. Make sure we only finalize once.
        if (! self->wasFinalized) {
            self->metro.Finalize(self->digest);
            self->wasFinalized = true;
        }

        // Return as buffer.
        std::string hash(reinterpret_cast<char const *>(self->digest), DIGEST_SIZE);
        NanReturnValue(_to_buffer(hash));
    }

public:
    static void Init(const char *js_class_name, Handle<Object> exports) {
        NanScope();
        Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

        // Prepare constructor template
        tpl->SetClassName(NanSymbol(js_class_name));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        // Prototype
        NODE_SET_PROTOTYPE_METHOD(tpl, "update", Update);
        NODE_SET_PROTOTYPE_METHOD(tpl, "digest", Digest);

        NanAssignPersistent(constructor, tpl->GetFunction());
        exports->Set(NanNew(js_class_name), tpl->GetFunction());
    }
};

template <typename HASHER, int DIGEST_SIZE>
Persistent<Function> NodeMetroHash<HASHER, DIGEST_SIZE>::constructor;

typedef NodeMetroHash<MetroHash64,   8>  NodeMetroHash64;
typedef NodeMetroHash<MetroHash128, 16> NodeMetroHash128;

// Addon initialization.
void InitAll(Handle<Object> exports) {
    NodeMetroHash64::Init("MetroHash64", exports);
    NodeMetroHash128::Init("MetroHash128", exports);
}

NODE_MODULE(metrohash, InitAll)
