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


#include "ByteStuffing_Library.h"

/*
void ByteStuffingResetFSM(ByteStuff_FSM_Str *p)
This function must be called once before using the library.

#define UARTs_Count        4

ByteStuff_FSM_Str ByteStuff_FSM[UARTs_Count];

for (int i=0; i<UARTs_Count; i++) ByteStuffingResetFSM(&ByteStuff_FSM[i]);

*/

void ByteStuffingResetFSM(ByteStuff_FSM_Str *p)
{
    p->RxSize=0;
    p->ErrorDetectedFlag=0;
    p->PreviousDataByte=0;
};

/*
unsigned char ByteStuffingDecoder(unsigned char *DataIn, unsigned char *DataOut, ByteStuff_FSM_Str *p)

This function analyzes the data stream received from the UART to identify frame boundaries using the byte-stuffing algorithm.
The function argument is the character received via UART.
The function will return flags that can be used to interpret the symbol's position within the frame.

return value  - unsigned char ( 8 bit )

bit 7   . ErrorDetectedFlag	- If the flag is set, the data is invalid and synchronization is broken.
bit 6   . FrameRxUnexFinish	- If the flag is set, it means the frame reception completed unexpectedly.
bit 4-3 . No information.
bit 2   . SkipByteFlag 		- This is a service byte, it must be skipped.
bit 1   . StartOfFrameFlag  - This byte is the first in the frame.
bit 0   . FrameRxDoneFlag   - This flag indicates that frame reception is complete.

Examples :

#define BS_FRAME_DELIMITER          0x7E
#define BS_ESCAPE_SYMBOL            0x7D
#define BS_ESCAPE_MASK0             0x5E
#define BS_ESCAPE_MASK1             0x5D

0x7E 0x7E - No DATA
0x7E 0x7E 0x7E - No DATA
0x7E DATA 0x7E	- One byte of DATA
0x7E 0x7D 0x5E 0x7E	- One byte of DATA
0x7E 0x7D 0x5D 0x7E	- One byte of DATA

0x7E DATA 0x7D 0x5D 0x7E	- Two bytes of DATA
0x7E 0x7D 0x5D DATA 0x7E	- Two bytes of DATA

0x7E 0x7D 0x5D 0x7D 0x5D 0x7E	- Two bytes of DATA
0x7E 0x7D 0x5D 0x7D 0x5E 0x7E	- Two bytes of DATA

0x7E DATA DATA 0x7D 0x7E 		-> 0x7D 0x7E - Error sequence
0x7E DATA 0x7D 0x7D 0x7E 		-> 0x7D 0x7D - Error sequence
0x7E 0x7D 0x7D DATA 0x7E 		-> 0x7D 0x7D - Error sequence
0x7E DATA 0x7D 0x7D DATA 0x7E 	-> 0x7D 0x7D - Error sequence
0x7E 0x7D 0x7D DATA DATA 0x7E 	-> 0x7D 0x7D - Error sequence


// ByteStuffing decoder FSM is made external variable, for using library with many UART devices
#define UARTs_Count        4

ByteStuff_FSM_Str ByteStuff_FSM[UARTs_Count];

unsigned char RetValue = ByteStuffingDecoder(DataIn, DataOut, &ByteStuff_FSM[0]);
*/

unsigned char ByteStuffingDecoder(unsigned char *DataIn, unsigned char *DataOut, ByteStuff_FSM_Str *p)
{
unsigned short	RxSize 				=p->RxSize;
unsigned char 	ErrorDetectedFlag 	=p->ErrorDetectedFlag;
unsigned char 	PreviousDataByte   	=p->PreviousDataByte;

unsigned char SkipByteFlag 		=0;
unsigned char StartOfFrameFlag 	=0;
unsigned char FrameRxDoneFlag 	=0;
unsigned char FrameRxUnexFinish =0;


if (*DataIn==BS_FRAME_DELIMITER)
    {
    if (ErrorDetectedFlag == 0x0 )
        {
        // If there are no errors and at least one valid symbol has been received,
        // the reception of a FrameDelimiterSymbol indicates the completion of the previous frame's reception.
        if ((RxSize!=0)&&(SkipByteFlag ==0)) {FrameRxDoneFlag =1; FrameRxUnexFinish =0;}
            else if ((RxSize!=0)&&(SkipByteFlag ==1)) {FrameRxDoneFlag =1; FrameRxUnexFinish =1;}
        }
    RxSize = 0;
    SkipByteFlag =1;
    ErrorDetectedFlag =0;
    }
    else if ((*DataIn!=BS_FRAME_DELIMITER))
        {
        if (ErrorDetectedFlag == 0x0 )
            {
            if ((*DataIn==BS_ESCAPE_SYMBOL)&&(PreviousDataByte==BS_ESCAPE_SYMBOL)) { SkipByteFlag =1; ErrorDetectedFlag =1;}
                else if ((*DataIn==BS_ESCAPE_SYMBOL)&&(PreviousDataByte!=BS_ESCAPE_SYMBOL)) { SkipByteFlag =1; }
                    else if ((*DataIn==BS_ESCAPE_MASK0)&&(PreviousDataByte==BS_ESCAPE_SYMBOL)) { SkipByteFlag =0; RxSize++; *DataOut = BS_FRAME_DELIMITER;}
                        else if ((*DataIn==BS_ESCAPE_MASK1)&&(PreviousDataByte==BS_ESCAPE_SYMBOL)) { SkipByteFlag =0;  RxSize++; *DataOut = BS_ESCAPE_SYMBOL;}
                            else if ((*DataIn!=BS_ESCAPE_MASK1)&&(*DataIn!=BS_ESCAPE_MASK0)&&(PreviousDataByte==BS_ESCAPE_SYMBOL)) { SkipByteFlag =1; ErrorDetectedFlag =1;}
                                else {SkipByteFlag =0; RxSize++; *DataOut = *DataIn; };
            } else if (ErrorDetectedFlag != 0x0 )
                {
                SkipByteFlag =1; ErrorDetectedFlag =1;
                }
        }

if ((RxSize ==1)&&(SkipByteFlag==0)) {StartOfFrameFlag =1;} else {StartOfFrameFlag =0;}

if  (RxSize > BS_MAX_PACK_SIZE ) ErrorDetectedFlag =1;
PreviousDataByte = *DataIn;

unsigned char RetValue =0;

//Error flag is ErrorDetectedFlag or FrameRxUnexFinish condition.
if (ErrorDetectedFlag       ) RetValue |= (0x1 << 7);
if (FrameRxUnexFinish       ) RetValue |= (0x1 << 7);

if (FrameRxUnexFinish       ) RetValue |= (0x1 << 6);

if (SkipByteFlag            ) RetValue |= (0x1 << 2);
if (StartOfFrameFlag        ) RetValue |= (0x1 << 1);
if (FrameRxDoneFlag         ) RetValue |= (0x1 << 0);


p->RxSize               =   RxSize;
p->ErrorDetectedFlag    =   ErrorDetectedFlag;
p->PreviousDataByte     =   PreviousDataByte;

return RetValue;
}


/*
unsigned char ByteStuffingEncoder(unsigned char *DataIn, unsigned char *DataOut_x2)

Example1:	Blocking UART_Transmit 

unsigned char TxDataArray [16];
unsigned char DataOut_x2 [2];

// Call Blocking UART_Transmit 
UART_Transmit(FrameDelimiter, 1);       // Sending FrameDelimiter

for(int i = 0; i<16; i++)
    {
    unsigned char SendSize = ByteStuffingEncoder(&TxDataArray[i], DataOut_x2);
    //SendSize value is equal to 1 or 2
	
	// Call Blocking UART_Transmit 
    UART_Transmit(DataOut_x2, SendSize);// Sending Encoded Data
    }
	
// Call Blocking UART_Transmit 
UART_Transmit(FrameDelimiter, 1);       // Sending FrameDelimiter


Example2:	Non-Blocking UART_Transmit 

unsigned char TxDataArray [16];
unsigned char TxDataArrayEncoded [16*2 +2];

unsigned short TxBytesCount=0;

TxDataArrayEncoded[TxBytesCount] = FrameDelimiter;		// Sending FrameDelimiter 
TxBytesCount++;

for(int i = 0; i<16; i++)
    {
    unsigned char SendSize = ByteStuffingEncoder(&TxDataArray[i], &TxDataArrayEncoded[TxBytesCount]);
    //SendSize value is equal to 1 or 2
	TxBytesCount += SendSize;
    }
	
TxDataArrayEncoded[TxBytesCount] = FrameDelimiter;		// Sending FrameDelimiter
TxBytesCount++;	

// Call Non-Blocking UART_Transmit 
UART_Transmit_IT(TxDataArrayEncoded, TxBytesCount); 

*/

unsigned char ByteStuffingEncoder(unsigned char *DataIn, unsigned char *DataOut_x2)
{
    unsigned char Size=0;
    //If the data contains a FrameDelimiterSymbol, we perform a replacement.
    if (DataIn[0]==BS_FRAME_DELIMITER)
        {
            DataOut_x2[Size]=BS_ESCAPE_SYMBOL;
            Size++;
            DataOut_x2[Size]=BS_ESCAPE_MASK0;
            Size++;
            // If the data contains an EscapeSymbol, we perform a replacement.
            } else if (DataIn[0]==BS_ESCAPE_SYMBOL)
                {
                DataOut_x2[Size]=BS_ESCAPE_SYMBOL;
                Size++;
                DataOut_x2[Size]=BS_ESCAPE_MASK1;
                Size++;
                // Otherwise, the data remain unchanged.
                } else
                    {
                    DataOut_x2[Size] = DataIn[0];
                    Size++;
                    }
return Size;
}
