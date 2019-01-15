#ifndef __SYS_H
#define __SYS_H
#include "stm32f10x.h"

//0,不支持ucos
//1,支持ucos
#define SYSTEM_SUPPORT_UCOS     1   //定义系统文件夹是否支持UCOS


//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum)       ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr)              *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum)      MEM_ADDR(BITBAND(addr, bitnum))
//IO口地址映射
#define GPIOA_ODR_Addr      (GPIOA_BASE+12) //0x4001080C
#define GPIOB_ODR_Addr      (GPIOB_BASE+12) //0x40010C0C
#define GPIOC_ODR_Addr      (GPIOC_BASE+12) //0x4001100C
#define GPIOD_ODR_Addr      (GPIOD_BASE+12) //0x4001140C
#define GPIOE_ODR_Addr      (GPIOE_BASE+12) //0x4001180C
#define GPIOF_ODR_Addr      (GPIOF_BASE+12) //0x40011A0C
#define GPIOG_ODR_Addr      (GPIOG_BASE+12) //0x40011E0C

#define GPIOA_IDR_Addr      (GPIOA_BASE+8) //0x40010808
#define GPIOB_IDR_Addr      (GPIOB_BASE+8) //0x40010C08
#define GPIOC_IDR_Addr      (GPIOC_BASE+8) //0x40011008
#define GPIOD_IDR_Addr      (GPIOD_BASE+8) //0x40011408
#define GPIOE_IDR_Addr      (GPIOE_BASE+8) //0x40011808
#define GPIOF_IDR_Addr      (GPIOF_BASE+8) //0x40011A08
#define GPIOG_IDR_Addr      (GPIOG_BASE+8) //0x40011E08

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr, n)//输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr, n)//输入

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr, n)//输出
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr, n)//输入

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr, n)//输出
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr, n)//输入

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr, n)//输出
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr, n)//输入

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr, n)//输出
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr, n)//输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr, n)//输出
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr, n)//输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr, n)//输出
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr, n)//输入

typedef enum {FALSE = 0, TRUE = !FALSE} bool;

#define NULL    0


#define kNoErr                      0       //! No error occurred.
#define kGeneralErr                -1       //! General error.
#define kInProgressErr              1       //! Operation in progress.

// Generic error codes are in the range -6700 to -6779.

#define kGenericErrorBase           -6700   //! Starting error code for all generic errors.

#define kUnknownErr                 -6700   //! Unknown error occurred.
#define kOptionErr                  -6701   //! Option was not acceptable.
#define kSelectorErr                -6702   //! Selector passed in is invalid or unknown.
#define kExecutionStateErr          -6703   //! Call made in the wrong execution state (e.g. called at interrupt time).
#define kPathErr                    -6704   //! Path is invalid, too long, or otherwise not usable.
#define kParamErr                   -6705   //! Parameter is incorrect, missing, or not appropriate.
#define kUserRequiredErr            -6706   //! User interaction is required.
#define kCommandErr                 -6707   //! Command invalid or not supported.
#define kIDErr                      -6708   //! Unknown, invalid, or inappropriate identifier.
#define kStateErr                   -6709   //! Not in appropriate state to perform operation.
#define kRangeErr                   -6710   //! Index is out of range or not valid.
#define kRequestErr                 -6711   //! Request was improperly formed or not appropriate.
#define kResponseErr                -6712   //! Response was incorrect or out of sequence.
#define kChecksumErr                -6713   //! Checksum does not match the actual data.
#define kNotHandledErr              -6714   //! Operation was not handled (or not handled completely).
#define kVersionErr                 -6715   //! Version is not correct or not compatible.
#define kSignatureErr               -6716   //! Signature did not match what was expected.
#define kFormatErr                  -6717   //! Unknown, invalid, or inappropriate file/data format.
#define kNotInitializedErr          -6718   //! Action request before needed services were initialized.
#define kAlreadyInitializedErr      -6719   //! Attempt made to initialize when already initialized.
#define kNotInUseErr                -6720   //! Object not in use (e.g. cannot abort if not already in use).
#define kAlreadyInUseErr            -6721   //! Object is in use (e.g. cannot reuse active param blocks).
#define kTimeoutErr                 -6722   //! Timeout occurred.
#define kCanceledErr                -6723   //! Operation canceled (successful cancel).
#define kAlreadyCanceledErr         -6724   //! Operation has already been canceled.
#define kCannotCancelErr            -6725   //! Operation could not be canceled (maybe already done or invalid).
#define kDeletedErr                 -6726   //! Object has already been deleted.
#define kNotFoundErr                -6727   //! Something was not found.
#define kNoMemoryErr                -6728   //! Not enough memory was available to perform the operation.
#define kNoResourcesErr             -6729   //! Resources unavailable to perform the operation.
#define kDuplicateErr               -6730   //! Duplicate found or something is a duplicate.
#define kImmutableErr               -6731   //! Entity is not changeable.
#define kUnsupportedDataErr         -6732   //! Data is unknown or not supported.
#define kIntegrityErr               -6733   //! Data is corrupt.
#define kIncompatibleErr            -6734   //! Data is not compatible or it is in an incompatible format.
#define kUnsupportedErr             -6735   //! Feature or option is not supported.
#define kUnexpectedErr              -6736   //! Error occurred that was not expected.
#define kValueErr                   -6737   //! Value is not appropriate.
#define kNotReadableErr             -6738   //! Could not read or reading is not allowed.
#define kNotWritableErr             -6739   //! Could not write or writing is not allowed.
#define kBadReferenceErr            -6740   //! An invalid or inappropriate reference was specified.
#define kFlagErr                    -6741   //! An invalid, inappropriate, or unsupported flag was specified.
#define kMalformedErr               -6742   //! Something was not formed correctly.
#define kSizeErr                    -6743   //! Size was too big, too small, or not appropriate.
#define kNameErr                    -6744   //! Name was not correct, allowed, or appropriate.
#define kNotPreparedErr             -6745   //! Device or service is not ready.
#define kReadErr                    -6746   //! Could not read.
#define kWriteErr                   -6747   //! Could not write.
#define kMismatchErr                -6748   //! Something does not match.
#define kDateErr                    -6749   //! Date is invalid or out-of-range.
#define kUnderrunErr                -6750   //! Less data than expected.
#define kOverrunErr                 -6751   //! More data than expected.
#define kEndingErr                  -6752   //! Connection, session, or something is ending.
#define kConnectionErr              -6753   //! Connection failed or could not be established.
#define kAuthenticationErr          -6754   //! Authentication failed or is not supported.
#define kOpenErr                    -6755   //! Could not open file, pipe, device, etc.
#define kTypeErr                    -6756   //! Incorrect or incompatible type (e.g. file, data, etc.).
#define kSkipErr                    -6757   //! Items should be or was skipped.
#define kNoAckErr                   -6758   //! No acknowledge.
#define kCollisionErr               -6759   //! Collision occurred (e.g. two on bus at same time).
#define kBackoffErr                 -6760   //! Backoff in progress and operation intentionally failed.
#define kNoAddressAckErr            -6761   //! No acknowledge of address.
#define kInternalErr                -6762   //! An error internal to the implementation occurred.
#define kNoSpaceErr                 -6763   //! Not enough space to perform operation.
#define kCountErr                   -6764   //! Count is incorrect.
#define kEndOfDataErr               -6765   //! Reached the end of the data (e.g. recv returned 0).
#define kWouldBlockErr              -6766   //! Would need to block to continue (e.g. non-blocking read/write).
#define kLookErr                    -6767   //! Special case that needs to be looked at (e.g. interleaved data).
#define kSecurityRequiredErr        -6768   //! Security is required for the operation (e.g. must use encryption).
#define kOrderErr                   -6769   //! Order is incorrect.
#define kUpgradeErr                 -6770   //! Must upgrade.
#define kAsyncNoErr                 -6771   //! Async operation successfully started and is now in progress.
#define kDeprecatedErr              -6772   //! Operation or data is deprecated.
#define kPermissionErr              -6773   //! Permission denied.

#define kGenericErrorEnd            -6779   //! Last generic error code (inclusive)

#define unlikely( EXPRESSSION )     !!(EXPRESSSION)
#define require_action_quiet( X, LABEL, ACTION )                                                        \
    do                                                                                                  \
    {                                                                                                   \
        if( unlikely( !(X) ) )                                                                          \
        {                                                                                               \
            { ACTION; }                                                                                 \
            goto LABEL;                                                                                 \
        }                                                                                               \
                                                                                                        \
    }   while( 1==0 )

#define WriteBig16( PTR, X ) \
    do \
    { \
        ( (uint8_t *)(PTR) )[ 0 ] = (uint8_t)( ( (X) >>  8 ) & 0xFF ); \
        ( (uint8_t *)(PTR) )[ 1 ] = (uint8_t)(   (X)         & 0xFF ); \
    \
    }   while( 0 )

#define require_quiet( X, LABEL )                                                                       \
    do                                                                                                  \
    {                                                                                                   \
        if( unlikely( !(X) ) )                                                                          \
        {                                                                                               \
            goto LABEL;                                                                                 \
        }                                                                                               \
                                                                                                        \
    }   while( 1==0 )

#define require( X, LABEL )                                                                             \
    do                                                                                                  \
    {                                                                                                   \
        if( unlikely( !(X) ) )                                                                          \
        {                                                                                               \
            /*debug_print_assert( 0, #X, NULL, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__ );*/                 \
            goto LABEL;                                                                                 \
        }                                                                                               \
                                                                                                        \
    }   while( 1==0 )

#define ReadBig16( PTR ) \
        ( (uint16_t)( \
        ( ( (uint16_t)( (uint8_t *)(PTR) )[ 0 ] ) << 8 ) | \
          ( (uint16_t)( (uint8_t *)(PTR) )[ 1 ] ) ) )

void NVIC_Configuration(void);



#endif
