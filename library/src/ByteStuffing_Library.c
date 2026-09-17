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
unsigned char ByteStuffingDecoder(unsigned char *DataIn, unsigned char *DataOut)

This function analyzes the data stream received from the UART to identify frame boundaries using the byte-stuffing algorithm.
The function argument is the character received via UART.
The function will return flags that can be used to interpret the symbol's position within the frame.

return value  - unsigned char ( 8 bit )

bit 7   . ErrorDetectedFlag	- If the flag is set, the data is invalid and synchronization is broken.
bit 4-3 . No information.
bit 2   . SkipByteFlag 		- This is a service byte, it must be skipped.
bit 1   . StartOfFrameFlag  - This byte is the first in the frame.
bit 0   . FrameRxDoneFlag   - This flag indicates that frame reception is complete.
*/

unsigned char ByteStuffingDecoder(unsigned char *DataIn, unsigned char *DataOut)
{

static unsigned short	RxSize 				=0;
static unsigned char 	ErrorDetectedFlag 	=0;
static unsigned char 	PreviousDataByte   	=0;

unsigned char SkipByteFlag 		=0;
unsigned char StartOfFrameFlag 	=0;
unsigned char FrameRxDoneFlag 	=0;


if (*DataIn==BS_FRAME_DELIMITER)
    {
    if (ErrorDetectedFlag == 0x0 )
        {
        // If there are no errors and at least one valid symbol has been received,
        // the reception of a FrameDelimiterSymbol indicates the completion of the previous frame's reception.
        if ((RxSize!=0)&&(SkipByteFlag ==0)) FrameRxDoneFlag =1;
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

unsigned int RetValue =0;

if (ErrorDetectedFlag ) RetValue |= (0x1 << 7);
if (SkipByteFlag      ) RetValue |= (0x1 << 2);
if (StartOfFrameFlag  ) RetValue |= (0x1 << 1);
if (FrameRxDoneFlag   ) RetValue |= (0x1 << 0);


return RetValue;
}


/*
unsigned char ByteStuffingEncoder(unsigned char *DataIn, unsigned char *DataOut_x2)

Example:
unsigned char TxDataArray [16];
unsigned char DataOut_x2 [2];

UART_Transmit(FrameDelimiter, 1);       // Sending FrameDelimiter
for(int i = 0; i<16; i++)
    {
    unsigned char SendSize = ByteStuffingEncoder(&TxDataArray[i], DataOut_x2);
    //SendSize value is equal to 1 or 2
    UART_Transmit(DataOut_x2, SendSize);// Sending Encoded Data
    }

UART_Transmit(FrameDelimiter, 1);       // Sending FrameDelimiter

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
