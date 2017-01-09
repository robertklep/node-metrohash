'use strict';
var MetroHash64  = require('./index').MetroHash64;
var MetroHash128 = require('./index').MetroHash128;
var metrohash64  = require('./index').metrohash64;
var metrohash128 = require('./index').metrohash128;
var expect       = require('chai').expect;

const TESTVECTOR_S = '012345678901234567890123456789012345678901234567890123456789012';
const TESTVECTOR_B = new Buffer(TESTVECTOR_S);

const CLS_TESTS = [
  { cls : MetroHash64,            expected : '6b753dae06704bad' },
  { cls : MetroHash64,  seed : 0, expected : '6b753dae06704bad' },
  { cls : MetroHash64,  seed : 1, expected : '3b0d481cf4b9b8df' },
  { cls : MetroHash128,           expected : 'c77ce2bfa4ed9f9b0548b2ac5074a297' },
  { cls : MetroHash128, seed : 0, expected : 'c77ce2bfa4ed9f9b0548b2ac5074a297' },
  { cls : MetroHash128, seed : 1, expected : '45a3cdb838199d7fbdd68d867a14ecef' },
];

const FUN_TESTS = [
  { fn : metrohash64,             expected : '6b753dae06704bad' },
  { fn : metrohash64,  seed : 0,  expected : '6b753dae06704bad' },
  { fn : metrohash64,  seed : 1,  expected : '3b0d481cf4b9b8df' },
  { fn : metrohash128,            expected : 'c77ce2bfa4ed9f9b0548b2ac5074a297' },
  { fn : metrohash128, seed : 0,  expected : 'c77ce2bfa4ed9f9b0548b2ac5074a297' },
  { fn : metrohash128, seed : 1,  expected : '45a3cdb838199d7fbdd68d867a14ecef' },
];

describe('Instantiation and method calling', function() {

  it('new MetroHash64() should work', function() {
    expect(function() { new MetroHash64(); }).to.not.throw();
  });

  it('MetroHash64() should work', function() {
    expect(function() { MetroHash64(); }).to.not.throw();
  });

  it('new MetroHash128() should work', function() {
    expect(function() { new MetroHash128(); }).to.not.throw();
  });

  it('MetroHash128() should work', function() {
    expect(function() { MetroHash128(); }).to.not.throw();
  });

  it('MetroHash64() should allow chaining', function() {
    expect(function() {
      new MetroHash64().update(TESTVECTOR_B).digest();
    }).to.not.throw();
  });

  it('MetroHash128() should allow chaining', function() {
    expect(function() {
      new MetroHash128().update(TESTVECTOR_B).digest();
    }).to.not.throw();
  });

});

describe('Test vectors', function() {

  CLS_TESTS.forEach(function(test) {
    var suffix = test.seed !== undefined ? ', seed = ' + test.seed + ')' : ')';

    it('new ' + test.cls.name + '(String' + suffix, function() {
      var instance = new test.cls(test.seed);
      instance.update(TESTVECTOR_S);
      expect(instance.digest()).to.equal(test.expected);
    });

    it(test.cls.name + '(String' + suffix, function() {
      var instance = test.cls(test.seed);
      instance.update(TESTVECTOR_S);
      expect(instance.digest()).to.equal(test.expected);
    });

    it('new ' + test.cls.name + '(Buffer' + suffix, function() {
      var instance = new test.cls(test.seed);
      instance.update(TESTVECTOR_B);
      expect(instance.digest()).to.equal(test.expected);
    });

    it(test.cls.name + '(Buffer' + suffix, function() {
      var instance = test.cls(test.seed);
      instance.update(TESTVECTOR_B);
      expect(instance.digest()).to.equal(test.expected);
    });

    it(test.cls.name + '(String' + suffix + ' incremental', function() {
      var instance = test.cls(test.seed);
      TESTVECTOR_S.split('').forEach(function(c) {
        instance.update(c);
      });
      expect(instance.digest()).to.equal(test.expected);
    });

    it(test.cls.name + '(Buffer' + suffix + ' incremental', function() {
      var instance = test.cls(test.seed);
      for (var i = 0; i < TESTVECTOR_B.length; i++) {
        instance.update(TESTVECTOR_B.slice(i, i + 1));
      }
      expect(instance.digest()).to.equal(test.expected);
    });
  });

  FUN_TESTS.forEach(function(test) {
    let hasSeed = test.seed !== undefined;
    let suffix  = hasSeed ? ', seed = ' + test.seed + ')' : ')';

    it('Standalone: ' + test.fn.name + '(String' + suffix, function() {
      let result = hasSeed ? test.fn(TESTVECTOR_S, test.seed) : test.fn(TESTVECTOR_S);
      expect(result).to.equal(test.expected);
    });

    it('Standalone: ' + test.fn.name + '(Buffer' + suffix, function() {
      let result = hasSeed ? test.fn(TESTVECTOR_B, test.seed) : test.fn(TESTVECTOR_B);
      expect(result).to.equal(test.expected);
    });

    it('Standalone: ' + test.fn.name + ', handle invalid data', function() {
      expect(function() { test.fn(123) }).to.fail;
    });

    it('Standalone: ' + test.fn.name + ', handle invalid seed', function() {
      expect(function() { test.fn(TESTVECTOR_S, 'foo') }).to.fail;
    });

    it('Standalone: ' + test.fn.name + ', handle invalid data and seed', function() {
      expect(function() { test.fn(123, 'foo') }).to.fail;
    });

  });

});
