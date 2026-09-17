////////////////////////////////////////////////////////////////////////////////
//MIT License

//Copyright (c) 2026 Sergio Batu    ikhan.sergio@gmail.com

//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#ifndef BYTESTUFFING_LIBRARY_H
#define BYTESTUFFING_LIBRARY_H

#ifdef __cplusplus
extern "C" {
#endif

#define BS_MAX_PACK_SIZE        8192

typedef struct
{
unsigned short RxSize;
unsigned char ErrorDetectedFlag;
unsigned char PreviousDataByte;
} ByteStuff_FSM_Str;

#define BS_FRAME_DELIMITER          0x7E
#define BS_ESCAPE_SYMBOL            0x7D
#define BS_ESCAPE_MASK0             0x5E
#define BS_ESCAPE_MASK1             0x5D

unsigned char ByteStuffingDecoder(unsigned char *DataIn, unsigned char *DataOut, ByteStuff_FSM_Str *p);
unsigned char ByteStuffingEncoder(unsigned char *DataIn, unsigned char *DataOut_x2);
void          ByteStuffingResetFSM(ByteStuff_FSM_Str *p);
#ifdef __cplusplus
}
#endif

#endif // BYTESTUFFING_LIBRARY_H
