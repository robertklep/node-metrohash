# node-metrohash

Wrapper around [MetroHash](https://github.com/jandrewrogers/MetroHash).
 
[![Build Status](https://travis-ci.org/robertklep/node-metrohash.svg)](https://travis-ci.org/robertklep/node-metrohash)

### Installation

```
$ npm install metrohash
```

### Usage

The module exports 6 functions:

``` javascript
// 64-bit hashing functions
metrohash64_1(input:[String | Buffer][, seed:Number]) : Buffer
metrohash64_2(input:[String | Buffer][, seed:Number]) : Buffer

// 128-bit hashing functions
metrohash128_1(input:[String | Buffer][, seed:Number]) : Buffer
metrohash128_2(input:[String | Buffer][, seed:Number]) : Buffer

// 128-bit hashing functions based on SSE4 `crc32` instruction
// (see https://msdn.microsoft.com/en-us/library/bb514033.aspx)
metrohash128crc_1(input:[String | Buffer][, seed:Number]) : Buffer
metrohash128crc_2(input:[String | Buffer][, seed:Number]) : Buffer
```

In short:

- all functions take either a `String` or a `Buffer` argument as input;
- all functions accept an optional seed passed as a `Number` (default: 0);
- all functions return a `Buffer` as output;

### Examples

``` javascript
var metrohash = require('metrohash');

// Calculate a 64-bit hash.
var hash = metrohash.metrohash64_1('Hello, World!');

// Convert to hex string.
var hexHash = hash.toString('hex');
```
