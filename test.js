var metrohash = require('./index');
var expect    = require('chai').expect;

const TESTVECTOR_S = '012345678901234567890123456789012345678901234567890123456789012';
const TESTVECTOR_B = new Buffer(TESTVECTOR_S);

const TESTS = [
  { fn : 'metrohash64_1',               expected : '658F044F5C730E40' },
  { fn : 'metrohash64_2',               expected : '073CAAB960623211' },
  { fn : 'metrohash128_1',              expected : 'ED9997ED9D0A8B0FF3F266399477788F' },
  { fn : 'metrohash128_2',              expected : '7BBA6FE119CF35D45507EDF3505359AB' },
  { fn : 'metrohash128crc_1',           expected : 'B329ED67831604D3DFAC4E4876D8262F' },
  { fn : 'metrohash128crc_2',           expected : '0502A67E257BBD77206BBCA6BBEF2653' },

  { fn : 'metrohash64_1',     seed : 0, expected : '658F044F5C730E40' },
  { fn : 'metrohash64_2',     seed : 0, expected : '073CAAB960623211' },
  { fn : 'metrohash128_1',    seed : 0, expected : 'ED9997ED9D0A8B0FF3F266399477788F' },
  { fn : 'metrohash128_2',    seed : 0, expected : '7BBA6FE119CF35D45507EDF3505359AB' },
  { fn : 'metrohash128crc_1', seed : 0, expected : 'B329ED67831604D3DFAC4E4876D8262F' },
  { fn : 'metrohash128crc_2', seed : 0, expected : '0502A67E257BBD77206BBCA6BBEF2653' },

  { fn : 'metrohash64_1',     seed : 1, expected : 'AE49EBB0A856537B' },
  { fn : 'metrohash64_2',     seed : 1, expected : 'CF518E9CF58402C0' },
  { fn : 'metrohash128_1',    seed : 1, expected : 'DDA6BA67F7DE755EFDF6BEABECCFD1F4' },
  { fn : 'metrohash128_2',    seed : 1, expected : '2DA6AF149A5CDBC12B09DB0846D69EF0' },
  { fn : 'metrohash128crc_1', seed : 1, expected : 'E8FAB51AF19F18A7B10D0A57D4276DF2' },
  { fn : 'metrohash128crc_2', seed : 1, expected : '2D54F87181A0CF64B02C50D95692BC19' },
];

describe('metrohash', function() {

  TESTS.forEach(function(test) {
    var suffix = test.seed !== undefined ? ', ' + test.seed + ')' : ')';

    it(test.fn + '(String' + suffix, function() {
      var args = [ TESTVECTOR_S ];
      if (test.seed !== undefined) { args.push(test.seed); }
      expect(metrohash[test.fn].apply(null, args).toString('hex').toUpperCase()).to.equal(test.expected);
    });

    it(test.fn + '(Buffer' + suffix, function() {
      var args = [ TESTVECTOR_B ];
      if (test.seed !== undefined) { args.push(test.seed); }
      expect(metrohash[test.fn].apply(null, args).toString('hex').toUpperCase()).to.equal(test.expected);
    });

  });

});
