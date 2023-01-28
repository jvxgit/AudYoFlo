#ifndef __HJVXBITFIELDTOOLS_H__
#define __HJVXBITFIELDTOOLS_H__

#ifdef JVX_NUMBER_32BITS_BITFIELD

#define JVX_EVALUATE_BITFIELD(a) (a).evaluateBool()

jvxSize jvx_bitFieldSize();
jvxBool jvx_bitTest(jvxBitField entry, jvxSize position);
jvxBool jvx_cbitTest(jvxCBitField entry, jvxSize position);
void jvx_bitZero(jvxBitField& entry);
void jvx_bitSet(jvxBitField& entry, jvxSize position);
void jvx_cbitSet(jvxCBitField& entry, jvxSize position);
void jvx_bitZSet(jvxBitField& entry, jvxSize position);
void jvx_cbitZSet(jvxCBitField& entry, jvxSize position);
void jvx_bitClear(jvxBitField& entry, jvxSize position);
void jvx_cbitClear(jvxCBitField& entry, jvxSize position);
void jvx_bitFull(jvxBitField& entry);
void jvx_bitFClear(jvxBitField& entry);
void jvx_bitFClear(jvxCBitField& entry);
void jvx_bitFSet(jvxBitField& entry);
void jvx_bitFSet(jvxCBitField& entry);
void jvx_bitToggle(jvxBitField& entry, jvxSize position);
void jvx_cbitToggle(jvxCBitField& entry, jvxSize position);
jvxSize jvx_bitCountFilled(jvxBitField entry);
jvxSize jvx_bitNumSelections(jvxBitField entry);
jvxUInt32 jvx_bitFieldValue32(jvxBitField entry);
jvxBitField jvx_string2BitField(const std::string& str, jvxBool & err);
jvxBitField jvx_string2BitField2SelId(const std::string& str, jvxSize ll, jvxBool& err);
std::string jvx_bitField2String(jvxBitField& entry);
int jvx_maxNumberSelections();

// ==========================================================
// Some C bitfield operations
// ==========================================================

jvxBool jvx_bitTest(jvxCBitField entry, jvxSize position);
void jvx_bitSet(jvxCBitField& entry, jvxSize position);
void jvx_bitZSet(jvxCBitField& entry, jvxSize position);
void jvx_bitClear(jvxCBitField& entry, jvxSize position);
void jvx_bitToggle(jvxCBitField& entry, jvxSize position);
jvxUInt32 jvx_bitFieldValue32(jvxCBitField entry);
// ==========================================================


#else

#define JVX_EVALUATE_BITFIELD(a) a 

jvxSize jvx_bitFieldSize();
jvxBool jvx_bitTest(jvxBitField entry, jvxSize position);
void jvx_bitSet(jvxBitField& entry, jvxSize position);
void jvx_bitZSet(jvxBitField& entry, jvxSize position);
void jvx_bitClear(jvxBitField& entry, jvxSize position);
void jvx_bitToggle(jvxBitField& entry, jvxSize position);
jvxUInt32 jvx_bitFieldValue32(jvxBitField entry);
#ifndef JVX_COMPILE_SMALL
jvxBitField jvx_string2BitField(std::string& str);
std::string jvx_bitField2String(jvxBitField& entry);
#endif
int jvx_maxNumberSelections();

#endif

#endif
