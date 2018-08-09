// Run using `benchr`:
// $ npm i benchr -g
// $ benchr benchmark.js

const MetroHash64  = require('./index').MetroHash64;
const MetroHash128 = require('./index').MetroHash128;
const metrohash64  = require('./index').metrohash64;
const metrohash128 = require('./index').metrohash128;

const TESTS = [
  { name: '16 byte',   input : Buffer.alloc(16, 'foobar') },
  { name: '32 byte',   input : Buffer.alloc(32, 'foobar') },
  { name: '64 byte',   input : Buffer.alloc(64, 'foobar') },
  { name: '128 byte',  input : Buffer.alloc(128, 'foobar') },
  { name: '256 byte',  input : Buffer.alloc(256, 'foobar') },
  { name: '1024 byte', input : Buffer.alloc(1024, 'foobar') },
];

module.exports = (suite, benchmark) => {

  for (const test of TESTS) {

    suite(test.name, () => {

      benchmark('MetroHash64', () => {
        let hasher = new MetroHash64();
        hasher.update(test.input);
        return hasher.digest();
      });

      benchmark('MetroHash128', () => {
        let hasher = new MetroHash128();
        hasher.update(test.input);
        return hasher.digest();
      });

      benchmark('metrohash64', () => {
        return metrohash64(test.input);
      });

      benchmark('metrohash128', () => {
        return metrohash128(test.input);
      });

    });
  }

}
