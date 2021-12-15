#pragma once

#include "../types.hpp"
#include <cassert>

namespace paqFe::internal {

static const uint8_t StateTable[256][2] = {
{  1,  2},{  3,  5},{  4,  6},{  7, 10},{  8, 12},{  9, 13},{ 11, 14}, // 0
{ 15, 19},{ 16, 23},{ 17, 24},{ 18, 25},{ 20, 27},{ 21, 28},{ 22, 29}, // 7
{ 26, 30},{ 31, 33},{ 32, 35},{ 32, 35},{ 32, 35},{ 32, 35},{ 34, 37}, // 14
{ 34, 37},{ 34, 37},{ 34, 37},{ 34, 37},{ 34, 37},{ 36, 39},{ 36, 39}, // 21
{ 36, 39},{ 36, 39},{ 38, 40},{ 41, 43},{ 42, 45},{ 42, 45},{ 44, 47}, // 28
{ 44, 47},{ 46, 49},{ 46, 49},{ 48, 51},{ 48, 51},{ 50, 52},{ 53, 43}, // 35
{ 54, 57},{ 54, 57},{ 56, 59},{ 56, 59},{ 58, 61},{ 58, 61},{ 60, 63}, // 42
{ 60, 63},{ 62, 65},{ 62, 65},{ 50, 66},{ 67, 55},{ 68, 57},{ 68, 57}, // 49
{ 70, 73},{ 70, 73},{ 72, 75},{ 72, 75},{ 74, 77},{ 74, 77},{ 76, 79}, // 56
{ 76, 79},{ 62, 81},{ 62, 81},{ 64, 82},{ 83, 69},{ 84, 71},{ 84, 71}, // 63
{ 86, 73},{ 86, 73},{ 44, 59},{ 44, 59},{ 58, 61},{ 58, 61},{ 60, 49}, // 70
{ 60, 49},{ 76, 89},{ 76, 89},{ 78, 91},{ 78, 91},{ 80, 92},{ 93, 69}, // 77
{ 94, 87},{ 94, 87},{ 96, 45},{ 96, 45},{ 48, 99},{ 48, 99},{ 88,101}, // 84
{ 88,101},{ 80,102},{103, 69},{104, 87},{104, 87},{106, 57},{106, 57}, // 91
{ 62,109},{ 62,109},{ 88,111},{ 88,111},{ 80,112},{113, 85},{114, 87}, // 98
{114, 87},{116, 57},{116, 57},{ 62,119},{ 62,119},{ 88,121},{ 88,121}, // 105
{ 90,122},{123, 85},{124, 97},{124, 97},{126, 57},{126, 57},{ 62,129}, // 112
{ 62,129},{ 98,131},{ 98,131},{ 90,132},{133, 85},{134, 97},{134, 97}, // 119
{136, 57},{136, 57},{ 62,139},{ 62,139},{ 98,141},{ 98,141},{ 90,142}, // 126
{143, 95},{144, 97},{144, 97},{ 68, 57},{ 68, 57},{ 62, 81},{ 62, 81}, // 133
{ 98,147},{ 98,147},{100,148},{149, 95},{150,107},{150,107},{108,151}, // 140
{108,151},{100,152},{153, 95},{154,107},{108,155},{100,156},{157, 95}, // 147
{158,107},{108,159},{100,160},{161,105},{162,107},{108,163},{110,164}, // 154
{165,105},{166,117},{118,167},{110,168},{169,105},{170,117},{118,171}, // 161
{110,172},{173,105},{174,117},{118,175},{110,176},{177,105},{178,117}, // 168
{118,179},{110,180},{181,115},{182,117},{118,183},{120,184},{185,115}, // 175
{186,127},{128,187},{120,188},{189,115},{190,127},{128,191},{120,192}, // 182
{193,115},{194,127},{128,195},{120,196},{197,115},{198,127},{128,199}, // 189
{120,200},{201,115},{202,127},{128,203},{120,204},{205,115},{206,127}, // 196
{128,207},{120,208},{209,125},{210,127},{128,211},{130,212},{213,125}, // 203
{214,137},{138,215},{130,216},{217,125},{218,137},{138,219},{130,220}, // 210
{221,125},{222,137},{138,223},{130,224},{225,125},{226,137},{138,227}, // 217
{130,228},{229,125},{230,137},{138,231},{130,232},{233,125},{234,137}, // 224
{138,235},{130,236},{237,125},{238,137},{138,239},{130,240},{241,125}, // 231
{242,137},{138,243},{130,244},{245,135},{246,137},{138,247},{140,248}, // 238
{249,135},{250, 69},{ 80,251},{140,252},{249,135},{250, 69},{ 80,251}, // 245
{140,252},{  0,  0},{  0,  0},{  0,  0}};  // 252


class State {
  uint8_t s;
public:

  void next(uint8_t bit){
    s = StateTable[s][bit];
  }

  operator uint8_t() {
    return s;
  }
};


template<size_t size>
class StateMap {
public:
  struct StateInfo
  {
    int32_t prob;
    int32_t count;
  };
  
private:

  static_assert(isPow2(size));
  static constexpr size_t SizeMask = size - 1;

  uint32_t state_prev = 0;
  StateInfo infos[size];

public:
  StateMap() {
    for(int i=0;i<size;i++) {
      infos[i].prob = (1 << 21);
      infos[i].count = 0;
    }
  };

  Prob predict(uint8_t bit, uint32_t state) {
    StateInfo &info = infos[state_prev];
    info.prob += (((int32_t)bit << 22) - info.prob) * 2 / (2 * info.count + 3);

    if(info.count < 1023) {
      info.count = info.count + 1;
    }

    state_prev = state & SizeMask;
    return infos[state_prev].prob >> 10;
  }

  /*
  ~StateMap() {
    printf("Prob state_map[256] = {\n\t");
    for(int i=0;i<256;i++) {
      printf("0X%X, ", infos[i].prob >> 10);
      if(((i + 1) % 8) == 0)
        printf("\n\t");
    }
    printf("};\n");
  }
  */

};

class StaticStateMap {
  const static Prob map[256];
public:
  Prob predict(uint8_t bit, uint32_t state) {
    assert(state < 256);
    return map[state];
  }
  
  Prob predict(uint32_t state) {
    assert(state < 256);
    return map[state];
  }

  Prob operator[](uint32_t state) {
    return map[state];
  }
};

const Prob StaticStateMap::map[256] = {
  0X78B, 0X231, 0XD08, 0X14D, 0X678, 0X8F3, 0XE4D, 0XC9, 
  0X473, 0X73E, 0X6E2, 0X7EC, 0XAF3, 0XA0F, 0XF22, 0XB0, 
  0X4A2, 0X2DF, 0X3C4, 0X678, 0X800, 0X91F, 0X88D, 0X800, 
  0X88D, 0X735, 0XAE0, 0XB64, 0XA76, 0XA55, 0XEF8, 0X95, 
  0X387, 0X308, 0X725, 0X68B, 0X874, 0XA0A, 0XB64, 0XBD1, 
  0XF6A, 0X70, 0X2B6, 0X419, 0X5B7, 0X500, 0X835, 0X6FB, 
  0XB4A, 0XA22, 0XD2E, 0XCF2, 0XF47, 0X50, 0X2E2, 0X3E2, 
  0X51F, 0X489, 0X6FC, 0X696, 0X971, 0X86E, 0XC12, 0XB0A, 
  0XC53, 0XD93, 0XF82, 0X3F, 0X281, 0X1A9, 0X488, 0X402, 
  0X662, 0X5BB, 0X81B, 0X771, 0XAA1, 0X95B, 0XD40, 0XBDC, 
  0XE9B, 0XDB8, 0XFAF, 0X3C, 0X1F8, 0XA2, 0X4BC, 0X3C2, 
  0XD75, 0XC3B, 0XB59, 0XDC6, 0XF96, 0X2F, 0X215, 0XBA, 
  0X3C8, 0X33C, 0XD1C, 0XCA2, 0XE53, 0XDB6, 0XFC6, 0X57, 
  0X1ED, 0X46, 0X3A8, 0X29B, 0XD34, 0XD37, 0XC25, 0XDC7, 
  0XFB3, 0X18, 0X1BC, 0X124, 0X31E, 0X274, 0XE21, 0XD81, 
  0XFA6, 0XE15, 0XFB9, 0X40, 0X1D4, 0X114, 0X307, 0X232, 
  0XDCB, 0XDE2, 0XD1E, 0XE3A, 0XFDA, 0X3D, 0X191, 0XA7, 
  0X2B7, 0X1D7, 0XE37, 0XDB2, 0XE0C, 0XE59, 0XFB3, 0X20, 
  0X157, 0X800, 0X800, 0XE8C, 0XFC3, 0X2E, 0X164, 0XE75, 
  0XFCC, 0X21, 0X130, 0XEAE, 0XFDE, 0XE, 0XF6, 0XECB, 
  0XFD3, 0X15, 0X15A, 0XEE2, 0XFE7, 0XF, 0XF8, 0XEE2, 
  0XFF2, 0X1D, 0XD2, 0XEE1, 0XFD0, 0X1D, 0XEA, 0XEFD, 
  0XFE6, 0X25, 0X104, 0XEF0, 0XFE5, 0X10, 0X116, 0XEE9, 
  0XFE4, 0X9, 0XB5, 0XF0D, 0XFF0, 0X10, 0XD5, 0XF1A, 
  0XFCB, 0X20, 0X11C, 0XF03, 0XFF0, 0X2, 0XE4, 0XF57, 
  0XFFC, 0X19, 0XB4, 0XF37, 0XFEF, 0X2, 0XCF, 0XF50, 
  0XFEF, 0X1A, 0X84, 0XF27, 0XFD3, 0XA, 0XBC, 0XF59, 
  0XFDF, 0XA, 0X8F, 0XF69, 0XFED, 0X13, 0XC6, 0XF13, 
  0XFED, 0XA, 0X8E, 0XF59, 0XFDE, 0X13, 0X9B, 0XF4B, 
  0XFFC, 0X13, 0X91, 0XF98, 0XFEC, 0X40, 0X6E, 0XF77, 
  0XFEC, 0X2, 0X82, 0XF62, 0XFFB, 0X27, 0X7E, 0XF64, 
  0XFFB, 0X1, 0X28, 0XFDD, 0XFF8, 0X800, 0X800, 0X800, 
};

}