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

class NodeMetroHash64 : public ObjectWrap {
    static Persistent<Function> constructor;
    MetroHash64                 metro;
    uint8_t                     digest[8];
    bool                        wasFinalized = false;

    explicit NodeMetroHash64(uint64_t seed = 0) {
        metro.Initialize(seed);
    }

    static NAN_METHOD(New) {
        NanScope();
        if (args.IsConstructCall()) {
            // TODO: check if argument type makes sense
            NodeMetroHash64* hasher = new NodeMetroHash64(args[0]->IsUndefined() ? 0 : args[0]->NumberValue());
            hasher->Wrap(args.This());
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
        NodeMetroHash64* self = ObjectWrap::Unwrap<NodeMetroHash64>(args.This());

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
        NodeMetroHash64* self = ObjectWrap::Unwrap<NodeMetroHash64>(args.This());

        // Finalize and get the hash value. Make sure we only finalize once.
        if (! self->wasFinalized) {
            self->metro.Finalize(self->digest);
            self->wasFinalized = true;
        }

        // Return as buffer.
        std::string hash(reinterpret_cast<char const *>(self->digest), 8);
        NanReturnValue(_to_buffer(hash));
    }

public:
    static void Init(Handle<Object> exports) {
        NanScope();
        Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

        // Prepare constructor template
        tpl->SetClassName(NanSymbol("MetroHash64"));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        // Prototype
        NODE_SET_PROTOTYPE_METHOD(tpl, "update", NodeMetroHash64::Update);
        NODE_SET_PROTOTYPE_METHOD(tpl, "digest", NodeMetroHash64::Digest);

        NanAssignPersistent(constructor, tpl->GetFunction());
        exports->Set(NanNew("MetroHash64"), tpl->GetFunction());
    }
};

Persistent<Function> NodeMetroHash64::constructor;

class NodeMetroHash128 : public ObjectWrap {
    static Persistent<Function> constructor;
    MetroHash128                metro;
    uint8_t                     digest[16];
    bool                        wasFinalized = false;

    explicit NodeMetroHash128(uint64_t seed = 0) {
        metro.Initialize(seed);
    }

    static NAN_METHOD(New) {
        NanScope();
        if (args.IsConstructCall()) {
            // TODO: check if argument type makes sense
            NodeMetroHash128* hasher = new NodeMetroHash128(args[0]->IsUndefined() ? 0 : args[0]->NumberValue());
            hasher->Wrap(args.This());
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
        NodeMetroHash128* self = ObjectWrap::Unwrap<NodeMetroHash128>(args.This());

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
        NodeMetroHash128* self = ObjectWrap::Unwrap<NodeMetroHash128>(args.This());

        // Finalize and get the hash value. Make sure we only finalize once.
        if (! self->wasFinalized) {
            self->metro.Finalize(self->digest);
            self->wasFinalized = true;
        }

        // Return as buffer.
        std::string hash(reinterpret_cast<char const *>(self->digest), 16);
        NanReturnValue(_to_buffer(hash));
    }

public:
    static void Init(Handle<Object> exports) {
        NanScope();
        Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

        // Prepare constructor template
        tpl->SetClassName(NanSymbol("MetroHash128"));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        // Prototype
        NODE_SET_PROTOTYPE_METHOD(tpl, "update", NodeMetroHash128::Update);
        NODE_SET_PROTOTYPE_METHOD(tpl, "digest", NodeMetroHash128::Digest);

        NanAssignPersistent(constructor, tpl->GetFunction());
        exports->Set(NanNew("MetroHash128"), tpl->GetFunction());
    }
};

Persistent<Function> NodeMetroHash128::constructor;

// Addon initialization.
void InitAll(Handle<Object> exports) {
    NodeMetroHash64::Init(exports);
    NodeMetroHash128::Init(exports);
}

NODE_MODULE(metrohash, InitAll)
