#ifndef BYTESTUFFING_LIBRARY_H
#define BYTESTUFFING_LIBRARY_H

#ifdef __cplusplus
extern "C" {
#endif


#define MAX_PackSize 8192

#define FrameDelimiter          0x7E
#define EscapeSymbol            0x7D
#define EscapeMask0             0x5E
#define EscapeMask1             0x5D

unsigned char ByteStuffingDecoder(unsigned char *DataIn, unsigned char *DataOut);
unsigned char ByteStuffingEncoder(unsigned char *DataIn, unsigned char *DataOut_x2);
#ifdef __cplusplus
}
#endif

#endif // BYTESTUFFING_LIBRARY_H
