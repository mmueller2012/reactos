#ifndef _NTDDBLUE_H_INCLUDED_
#define _NTDDBLUE_H_INCLUDED_


#define IOCTL_CONSOLE_GET_SCREEN_BUFFER_INFO    CTL_CODE(FILE_DEVICE_SCREEN, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_CONSOLE_SET_SCREEN_BUFFER_INFO    CTL_CODE(FILE_DEVICE_SCREEN, 0x802, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_CONSOLE_GET_CURSOR_INFO           CTL_CODE(FILE_DEVICE_SCREEN, 0x803, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_CONSOLE_SET_CURSOR_INFO           CTL_CODE(FILE_DEVICE_SCREEN, 0x804, METHOD_BUFFERED, FILE_WRITE_ACCESS)


#define IOCTL_CONSOLE_FILL_OUTPUT_ATTRIBUTE     CTL_CODE(FILE_DEVICE_SCREEN, 0x810, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_CONSOLE_READ_OUTPUT_ATTRIBUTE     CTL_CODE(FILE_DEVICE_SCREEN, 0x811, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_CONSOLE_WRITE_OUTPUT_ATTRIBUTE    CTL_CODE(FILE_DEVICE_SCREEN, 0x812, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_CONSOLE_SET_TEXT_ATTRIBUTE        CTL_CODE(FILE_DEVICE_SCREEN, 0x813, METHOD_BUFFERED, FILE_WRITE_ACCESS)


#define IOCTL_CONSOLE_FILL_OUTPUT_CHARACTER     CTL_CODE(FILE_DEVICE_SCREEN, 0x820, METHOD_BUFFERED, FILE_WRITE_ACCESS)



/* TYPEDEFS **************************************************************/

typedef struct _FILL_OUTPUT_CHARACTER
{
    WCHAR cCharacter;
    DWORD nLength;
    COORD dwWriteCoord;
} FILL_OUTPUT_CHARACTER, *PFILL_OUTPUT_CHARACTER;


typedef struct _FILL_OUTPUT_ATTRIBUTE
{
    WORD  wAttribute;
    DWORD nLength;
    COORD dwWriteCoord;
} FILL_OUTPUT_ATTRIBUTE, *PFILL_OUTPUT_ATTRIBUTE;


typedef struct _WRITE_OUTPUT_ATTRIBUTE
{
    CONST WORD *lpAttribute;
    DWORD nLength;
    COORD dwWriteCoord;
} WRITE_OUTPUT_ATTRIBUTE, *PWRITE_OUTPUT_ATTRIBUTE;



#endif /* _NTDDBLUE_H_INCLUDED_ */
