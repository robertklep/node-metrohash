# node-metrohash

Wrapper around [MetroHash](https://github.com/jandrewrogers/MetroHash).
 
[![Build Status](https://travis-ci.org/robertklep/node-metrohash.svg)](https://travis-ci.org/robertklep/node-metrohash)

### Installation

```
$ npm install metrohash
```

### API change!

Between v1 and v2, the API for this library has changed to allow for maximum hashing speed.

The biggest change is that a calculated hash is now returned as a (hex-encoded) string instead of a `Buffer`.

If a `Buffer` is still required, it's easy to convert the string:

```
let buffer = Buffer.from(metrohash64('input'), 'hex');
```

Also, the `.hash()` methods for the hasher classes have been removed in favor of standalone functions (see below).

### Usage

The module exports 2 classes, `MetroHash64` and `MetroHash128`, and two functions, `metrohash64` and `metrohash128`.

The classes are meant for incremental hashing, the functions for standalone hash calculations.

#### Class interface

``` javascript
const MetroHash64 = require('metrohash').MetroHash64;

// Constructor.
MetroHash64([ seed:Number = 0 ]) : Object

// Update.
MetroHash64#update(input : [String | Buffer]) : Object

// Finalize and get hash digest.
MetroHash64#digest() : String
```

(likewise for `MetroHash128`).

#### Function interface

```
const metrohash64 = require('metrohash').metrohash64;

metrohash64(input : [String | Buffer], [ seed:Number = 0 ]) : String
```

(likewise for `metrohash128`).

### Examples

``` javascript
//// Classes

const MetroHash64 = require('metrohash').MetroHash64;

// Instantiate using seed 123 (`new` is optional).
let hash = new MetroHash64(123);

// Update using a string as input.
hash.update('Hello, World!');

// The same as above:
// hash.update('Hello, ').update('World!');

// Finalize to get the digest as a hex string.
let digest = hash.digest();

//// Functions
const metrohash64 = require('metrohash').metrohash64;

let digest = metrohash64('Hello, World!', 123);
```

### Speed

From v2.0 onwards, MetroHash is [pretty fast](https://medium.com/@drainingsun/in-search-of-a-good-node-js-hashing-algorithm-8052b6923a3b).
