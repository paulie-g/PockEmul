#include <QDebug>

#include "upd16434.h"
#include "pcxxxx.h"
#include "cpu.h"
#include "Log.h"

/*
STOP        Set Stop Mode                       01
DISP OFF    Display Off                         08
DISP ON     Display On                          09
SFF         Set Frame Frequency                 10 to 14
SMM         Set Multiplexing Mode               18 to 1F
BRESET      Bit Reset                           20 to 3F
BSET        Bit set                             40 to 5F
SRM         Set Read Mode                       60 to 63
SWM         Set Write Mode                      64 to 67
SORM        Set OR Mode                         68 to 6B
SANDM       Set AND Mode                        6C to 6F
SCML        Set Character Mode with Left entry  71
SCMR        Set Character Mode with Right entry 72
CLCURS      Clear Cursor                        7C
WRCURS      Write Cursor                        7D
LDPI        Load Data Pointer with Immediate    80 to B1, C0 to F1
*/

#define MASK_STOP       0x01
#define MASK_DISPOFF    0x08
#define MASK_DISPON     0x09
#define MASK_SFF        0x10
#define MASK_SMM        0x18
#define MASK_BRESET     0x20
#define MASK_BSET       0x40
#define MASK_SRM        0x60
#define MASK_SWM        0x64
#define MASK_SORM       0x68
#define MASK_SANDM      0x6C
#define MASK_SCML       0x71
#define MASK_SCMR       0x72
#define MASK_CLCURS     0x7C
#define MASK_WRCURS     0x7D
#define MASK_LDPI       0x80

#define SWM  0x64
#define SCMR 0x72
#define SCML 0x71

const unsigned short upd16434charfont5x7[] = {
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char
    0x00, 0x00, 0x4F, 0x00, 0x00,            // Code for char !
    0x00, 0x07, 0x00, 0x07, 0x00,            // Code for char "
    0x14, 0x7F, 0x14, 0x7F, 0x14,            // Code for char #
    0x2C, 0x2A, 0x7F, 0x2A, 0x12,            // Code for char $
    0x23, 0x13, 0x08, 0x64, 0x62,            // Code for char %
    0x36, 0x49, 0x55, 0x22, 0x50,            // Code for char &
    0x00, 0x05, 0x03, 0x00, 0x00,            // Code for char '
    0x00, 0x00, 0x1C, 0x22, 0x41,            // Code for char (
    0x41, 0x22, 0x1C, 0x00, 0x00,            // Code for char )
    0x14, 0x08, 0x3E, 0x08, 0x14,            // Code for char *
    0x08, 0x08, 0x3E, 0x08, 0x08,            // Code for char +
    0x50, 0x30, 0x00, 0x00, 0x00,            // Code for char ,
    0x08, 0x08, 0x08, 0x08, 0x08,            // Code for char -
    0x00, 0x60, 0x60, 0x00, 0x00,            // Code for char .
    0x20, 0x10, 0x08, 0x04, 0x02,            // Code for char /
    0x3E, 0x51, 0x49, 0x45, 0x3E,            // Code for char 0
    0x00, 0x42, 0x7F, 0x40, 0x00,            // Code for char 1
    0x42, 0x61, 0x51, 0x49, 0x46,            // Code for char 2
    0x21, 0x41, 0x45, 0x4B, 0x31,            // Code for char 3
    0x18, 0x14, 0x12, 0x7F, 0x10,            // Code for char 4
    0x27, 0x45, 0x45, 0x45, 0x39,            // Code for char 5
    0x3C, 0x4A, 0x49, 0x49, 0x30,            // Code for char 6
    0x01, 0x71, 0x09, 0x05, 0x03,            // Code for char 7
    0x36, 0x49, 0x49, 0x49, 0x36,            // Code for char 8
    0x06, 0x49, 0x49, 0x29, 0x1E,            // Code for char 9
    0x00, 0x36, 0x36, 0x00, 0x00,            // Code for char :
    0x00, 0x56, 0x36, 0x00, 0x00,            // Code for char ;
    0x08, 0x14, 0x22, 0x41, 0x00,            // Code for char <
    0x14, 0x14, 0x14, 0x14, 0x14,            // Code for char =
    0x00, 0x41, 0x22, 0x14, 0x08,            // Code for char >
    0x02, 0x01, 0x51, 0x09, 0x06,            // Code for char ?
    0x32, 0x49, 0x79, 0x41, 0x3E,            // Code for char @
    0x7E, 0x11, 0x11, 0x11, 0x7E,            // Code for char A
    0x7F, 0x49, 0x49, 0x49, 0x36,            // Code for char B
    0x3E, 0x41, 0x41, 0x41, 0x22,            // Code for char C
    0x7F, 0x41, 0x41, 0x22, 0x1C,            // Code for char D
    0x7F, 0x49, 0x49, 0x49, 0x41,            // Code for char E
    0x7F, 0x09, 0x09, 0x09, 0x01,            // Code for char F
    0x3E, 0x41, 0x49, 0x49, 0x78,            // Code for char G
    0x7F, 0x08, 0x08, 0x08, 0x7F,            // Code for char H
    0x00, 0x41, 0x7F, 0x41, 0x00,            // Code for char I
    0x20, 0x40, 0x41, 0x3F, 0x01,            // Code for char J
    0x7F, 0x08, 0x14, 0x22, 0x41,            // Code for char K
    0x7F, 0x40, 0x40, 0x40, 0x40,            // Code for char L
    0x7F, 0x02, 0x0C, 0x02, 0x7F,            // Code for char M
    0x7F, 0x04, 0x08, 0x10, 0x7F,            // Code for char N
    0x3E, 0x41, 0x41, 0x41, 0x3E,            // Code for char O
    0x7F, 0x09, 0x09, 0x09, 0x06,            // Code for char P
    0x3E, 0x41, 0x51, 0x21, 0x5E,            // Code for char Q
    0x7F, 0x09, 0x19, 0x29, 0x46,            // Code for char R
    0x46, 0x49, 0x49, 0x49, 0x31,            // Code for char S
    0x01, 0x01, 0x7F, 0x01, 0x01,            // Code for char T
    0x3F, 0x40, 0x40, 0x40, 0x3F,            // Code for char U
    0x1F, 0x20, 0x40, 0x20, 0x1F,            // Code for char V
    0x3F, 0x40, 0x38, 0x40, 0x3F,            // Code for char W
    0x63, 0x14, 0x08, 0x14, 0x63,            // Code for char X
    0x07, 0x08, 0x70, 0x08, 0x07,            // Code for char Y
    0x61, 0x51, 0x49, 0x45, 0x43,            // Code for char Z
    0x00, 0x7F, 0x41, 0x41, 0x00,            // Code for char [
    0x02, 0x04, 0x08, 0x10, 0x20,            // Code for char BackSlash
    0x00, 0x41, 0x41, 0x7F, 0x00,            // Code for char ]
    0x04, 0x02, 0x01, 0x02, 0x04,            // Code for char ^
    0x40, 0x40, 0x40, 0x40, 0x40,            // Code for char _
    0x00, 0x01, 0x02, 0x04, 0x00,            // Code for char `
    0x20, 0x54, 0x54, 0x54, 0x78,            // Code for char a
    0x00, 0x7F, 0x44, 0x44, 0x38,            // Code for char b
    0x38, 0x44, 0x44, 0x44, 0x48,            // Code for char c
    0x00, 0x38, 0x44, 0x44, 0x7F,            // Code for char d
    0x38, 0x54, 0x54, 0x54, 0x58,            // Code for char e
    0x00, 0x04, 0x7E, 0x05, 0x01,            // Code for char f
    0x08, 0x54, 0x54, 0x54, 0x3C,            // Code for char g
    0x00, 0x7F, 0x04, 0x04, 0x78,            // Code for char h
    0x00, 0x44, 0x7D, 0x40, 0x00,            // Code for char i
    0x20, 0x40, 0x40, 0x3D, 0x00,            // Code for char j
    0x00, 0x7F, 0x10, 0x28, 0x44,            // Code for char k
    0x00, 0x41, 0x7F, 0x40, 0x00,            // Code for char l
    0x7C, 0x04, 0x78, 0x04, 0x78,            // Code for char m
    0x00, 0x7C, 0x04, 0x04, 0x78,            // Code for char n
    0x38, 0x44, 0x44, 0x44, 0x38,            // Code for char o
    0x7C, 0x14, 0x14, 0x14, 0x08,            // Code for char p
    0x08, 0x14, 0x14, 0x14, 0x7C,            // Code for char q
    0x00, 0x7C, 0x08, 0x04, 0x04,            // Code for char r
    0x58, 0x54, 0x54, 0x54, 0x30,            // Code for char s
    0x00, 0x04, 0x3E, 0x44, 0x40,            // Code for char t
    0x00, 0x3C, 0x40, 0x40, 0x7C,            // Code for char u
    0x1C, 0x20, 0x40, 0x20, 0x1C,            // Code for char v
    0x3C, 0x40, 0x30, 0x40, 0x3C,            // Code for char w
    0x44, 0x28, 0x10, 0x28, 0x44,            // Code for char x
    0x00, 0x0C, 0x50, 0x50, 0x3C,            // Code for char y
    0x44, 0x64, 0x54, 0x4C, 0x44,            // Code for char z
    0x00, 0x08, 0x36, 0x41, 0x00,            // Code for char {
    0x00, 0x00, 0x7F, 0x00, 0x00,            // Code for char |
    0x00, 0x41, 0x36, 0x08, 0x00,            // Code for char }
    0x04, 0x02, 0x04, 0x08, 0x04,            // Code for char ~
    0x7F, 0x7F, 0x7F, 0x7F, 0x7F,            // Code for char 
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x70, 0x50, 0x70, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x0F, 0x01, 0x01,            // Code for char �
    0x40, 0x40, 0x78, 0x00, 0x00,            // Code for char �
    0x10, 0x20, 0x40, 0x00, 0x00,            // Code for char �
    0x00, 0x0C, 0x0C, 0x00, 0x00,            // Code for char �
    0x0A, 0x0A, 0x4A, 0x2A, 0x1E,            // Code for char �
    0x04, 0x44, 0x34, 0x14, 0x0C,            // Code for char �
    0x20, 0x10, 0x78, 0x04, 0x00,            // Code for char �
    0x18, 0x08, 0x4C, 0x48, 0x38,            // Code for char �
    0x48, 0x48, 0x78, 0x48, 0x48,            // Code for char �
    0x48, 0x28, 0x18, 0x7C, 0x08,            // Code for char �
    0x08, 0x7C, 0x08, 0x28, 0x18,            // Code for char �
    0x40, 0x48, 0x48, 0x78, 0x40,            // Code for char �
    0x54, 0x54, 0x54, 0x7C, 0x00,            // Code for char �
    0x18, 0x00, 0x58, 0x40, 0x38,            // Code for char �
    0x08, 0x08, 0x08, 0x08, 0x08,            // Code for char �
    0x01, 0x41, 0x3D, 0x09, 0x07,            // Code for char �
    0x10, 0x08, 0x7C, 0x02, 0x01,            // Code for char �
    0x0E, 0x02, 0x43, 0x22, 0x1E,            // Code for char �
    0x42, 0x42, 0x7E, 0x42, 0x42,            // Code for char �
    0x22, 0x12, 0x0A, 0x7F, 0x02,            // Code for char �
    0x42, 0x3F, 0x02, 0x42, 0x3E,            // Code for char �
    0x0A, 0x0A, 0x7F, 0x0A, 0x0A,            // Code for char �
    0x08, 0x46, 0x42, 0x22, 0x1E,            // Code for char �
    0x04, 0x03, 0x42, 0x3E, 0x02,            // Code for char �
    0x42, 0x42, 0x42, 0x42, 0x7E,            // Code for char �
    0x02, 0x4F, 0x22, 0x1F, 0x02,            // Code for char �
    0x4A, 0x4A, 0x40, 0x20, 0x1C,            // Code for char �
    0x42, 0x22, 0x12, 0x2A, 0x46,            // Code for char �
    0x02, 0x3F, 0x42, 0x4A, 0x46,            // Code for char �
    0x06, 0x48, 0x40, 0x20, 0x1E,            // Code for char �
    0x08, 0x46, 0x4A, 0x32, 0x1E,            // Code for char
    0x0A, 0x4A, 0x3E, 0x09, 0x08,            // Code for char �
    0x0E, 0x00, 0x4E, 0x20, 0x1E,            // Code for char �
    0x04, 0x45, 0x3D, 0x05, 0x04,            // Code for char �
    0x00, 0x7F, 0x08, 0x10, 0x00,            // Code for char �
    0x44, 0x24, 0x1F, 0x04, 0x04,            // Code for char �
    0x40, 0x42, 0x42, 0x42, 0x40,            // Code for char �
    0x42, 0x2A, 0x12, 0x2A, 0x06,            // Code for char �
    0x22, 0x12, 0x7B, 0x16, 0x22,            // Code for char �
    0x00, 0x40, 0x20, 0x1F, 0x00,            // Code for char �
    0x78, 0x00, 0x02, 0x04, 0x78,            // Code for char �
    0x3F, 0x44, 0x44, 0x44, 0x44,            // Code for char �
    0x02, 0x42, 0x42, 0x22, 0x1E,            // Code for char �
    0x04, 0x02, 0x04, 0x08, 0x30,            // Code for char �
    0x32, 0x02, 0x7F, 0x02, 0x32,            // Code for char �
    0x02, 0x12, 0x22, 0x52, 0x0E,            // Code for char �
    0x20, 0x2A, 0x2A, 0x2A, 0x40,            // Code for char �
    0x38, 0x24, 0x22, 0x20, 0x70,            // Code for char �
    0x40, 0x28, 0x10, 0x28, 0x06,            // Code for char �
    0x0A, 0x3E, 0x4A, 0x4A, 0x4A,            // Code for char �
    0x40, 0x42, 0x42, 0x7E, 0x40,            // Code for char �
    0x4A, 0x4A, 0x4A, 0x4A, 0x7E,            // Code for char �
    0x04, 0x05, 0x45, 0x25, 0x1C,            // Code for char �
    0x0F, 0x40, 0x20, 0x1F, 0x00,            // Code for char �
    0x78, 0x00, 0x7C, 0x40, 0x30,            // Code for char �
    0x7E, 0x40, 0x20, 0x10, 0x08,            // Code for char �
    0x7E, 0x42, 0x42, 0x42, 0x7E,            // Code for char �
    0x0E, 0x02, 0x42, 0x22, 0x1E,            // Code for char �
    0x42, 0x42, 0x40, 0x20, 0x18,            // Code for char �
    0x04, 0x08, 0x02, 0x04, 0x00,            // Code for char �
    0x07, 0x05, 0x07, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00,            // Code for char �
    0x00, 0x00, 0x00, 0x00, 0x00             // Code for char �
    };



CUPD16434::CUPD16434(CpcXXXX *parent, quint8 id,chipModel mod)
{
    pPC = parent;
    this->id = id;
    switch (mod) {
    case UPD16434: memoryLimit = 0x7f; break;
    case UPD07728: memoryLimit = 39; break;
    }
}

CUPD16434::~CUPD16434() {

}



bool CUPD16434::init()
{
    Reset();

    return true;
}

bool CUPD16434::exit()
{
    return true;
}

void CUPD16434::Reset()
{
//    qWarning()<<"UPD16434["<<id<<"]"<<" RESET";
    memset(info.imem,0,sizeof(info.imem));
    info.mode = SWM;
    info.on_off = 0;
    info.dataPointer = 0;
    info.status = 0;
    updated = true;
}

bool CUPD16434::step()
{
    return true;
}

void CUPD16434::addChar(quint8 c,bool right) {
//    qWarning()<<"UPD16434 ADD CHAR at "<<info.dataPointer<<": "<<c<<"=("<<QChar(c)<<")";
    if (pPC->pCPU->fp_log)fprintf(pPC->pCPU->fp_log,"UPD16434 ADD CHAR at %i: %02x=(%c)\n",info.dataPointer,c,c);
    for (int i = 0;i<5;i++) {
        info.imem[info.dataPointer] = upd16434charfont5x7[(c-0x20)*5+4-i];
        if (right) info.dataPointer ++;
        else info.dataPointer --;

        info.dataPointer &= 0x7f;
        if (info.dataPointer > 0x7f) {
            qWarning()<<"INDEX ERROR";
        }
    }
}

BYTE CUPD16434::data(quint8 cmd)
{
//    qWarning()<<"UPD16434["<<id<<"]"<<" start DATA:"<<cmd<<" mode:"<<info.mode<<"  DP:"<<info.dataPointer;
    switch (info.mode) {

    case SCMR: addChar(cmd,true);
        updated = true;
        break;
    case SCML: addChar(cmd,false);
        updated = true;
        break;
    default:
        switch (info.mode & 0xfc) {
        case SWM:
            if (info.dataPointer>=0x80) {
                qWarning()<<"INDEX ERROR2";
            }
            info.imem[info.dataPointer] = cmd;
            if ((info.mode & 0x03)==0) { info.dataPointer++; info.dataPointer &= 0x7f; }
            if ((info.mode & 0x03)==1) { info.dataPointer--; info.dataPointer &= 0x7f; }
            break;

        default:
//            qWarning()<<"UPD16434["<<id<<"]"<<" Unknown mode ";
//            if (pPC->pCPU->fp_log)fprintf(pPC->pCPU->fp_log,"UPD16434 Unknown mode %i\n",info.mode);
//            AddLog(LOG_DISPLAY,QString("mode:%1").arg(info.mode));
            break;
        }

//        qWarning()<<"UPD16434["<<id<<"]"<<" end  DATA:"<<cmd<<" mode:"<<info.mode<<"  DP:"<<info.dataPointer;
        updated = true;

    }

//    info.imem[info.dataPointer] = cmd;

    return 0;
}

BYTE CUPD16434::instruction(quint8 cmd)
{

    if (pPC->pCPU->fp_log)fprintf(pPC->pCPU->fp_log,"UPD16434 CMD: %02x\n",cmd);

//    AddLog(LOG_DISPLAY,tr("UPD16434 CMD:%1").arg(cmd,4,16,QChar('0')));
//    qWarning()<<"UPD16434["<<id<<"]"<<" CMD:"<<cmd;
//    updated = true;

    if ((cmd & MASK_LDPI) == MASK_LDPI ) { return cmd_LDPI(cmd); }
    else
    if ((cmd & MASK_WRCURS) == MASK_WRCURS ) { return cmd_WRCURS(cmd); }
    else
    if ((cmd & MASK_CLCURS) == MASK_CLCURS ) { return cmd_CLCURS(cmd); }
    else
    if ((cmd & MASK_SCMR) == MASK_SCMR ) { return cmd_MODE(cmd); }
    else
    if ((cmd & MASK_SCML) == MASK_SCML ) { return cmd_MODE(cmd); }
    else
    if ((cmd & MASK_SANDM) == MASK_SANDM ) { return cmd_MODE(cmd); }
    else
    if ((cmd & MASK_SORM) == MASK_SORM ) { return cmd_MODE(cmd); }
    else
    if ((cmd & MASK_SWM) == MASK_SWM ) { return cmd_MODE(cmd); }
    else
    if ((cmd & MASK_SRM) == MASK_SRM ) { return cmd_MODE(cmd); }
    else
    if ((cmd & MASK_BSET) == MASK_BSET ) { return cmd_BSET(cmd,true); }
    else
    if ((cmd & MASK_BRESET) == MASK_BRESET ) { return cmd_BSET(cmd,false); }
    else
    if ((cmd & MASK_SMM) == MASK_SMM ) { return cmd_SMM(cmd); }
    else
    if ((cmd & MASK_SFF) == MASK_SFF ) { return cmd_SFF(cmd); }
    else
    if ((cmd & MASK_DISPON) == MASK_DISPON ) { return cmd_DISPON(cmd); }
    else
    if ((cmd & MASK_DISPOFF) == MASK_DISPOFF ) { return cmd_DISPOFF(cmd); }
    else
    if ((cmd & MASK_STOP) == MASK_STOP ) { return cmd_STOP(cmd); }


    return 0;
}


BYTE CUPD16434::cmd_LDPI(quint8 cmd)
{
    info.dataPointer = cmd & 0x7F;
    updated = true;

    if ((info.mode >= 0x60) && (info.mode <= 0x63)) {
        outputRegister = info.imem[info.dataPointer];
        outputBit=7;

        qWarning()<<"UPD16434["<<id<<"]"<<" Output register:"
                 <<QString("%1").arg(outputRegister,2,16,QChar('0'))
                <<" from:"<<info.dataPointer;
        if ((info.mode & 0x03)==0) { info.dataPointer++; info.dataPointer &= 0x7f; }
        if ((info.mode & 0x03)==1) { info.dataPointer--; info.dataPointer &= 0x7f; }
    }

    return 0;
}

BYTE CUPD16434::cmd_WRCURS(quint8 cmd)
{
    updated = true;
    return 0;
}

BYTE CUPD16434::cmd_CLCURS(quint8 cmd)
{
    updated = true;
    return 0;
}

BYTE CUPD16434::cmd_MODE(quint8 cmd)
{
    info.mode = cmd;
    updated = true;

    if ((cmd >= 0x60) && (cmd <= 0x63)) {
        info.mode = cmd;
        outputRegister = info.imem[info.dataPointer];
        outputBit=7;

        qWarning()<<"UPD16434["<<id<<"]"<<" READ MODE("<<cmd<<"):"
                 <<QString("%1").arg(outputRegister,2,16,QChar('0'))
                <<" from:"<<info.dataPointer;
        if ((info.mode & 0x03)==0) { info.dataPointer++; info.dataPointer &= 0x7f; }
        if ((info.mode & 0x03)==1) { info.dataPointer--; info.dataPointer &= 0x7f; }
    }

    return cmd;
}



BYTE CUPD16434::cmd_BSET(quint8 cmd,bool set)
{
    quint8 bit = (cmd >> 2) & 0x07;
    quint8 mode= cmd & 0x03;
    if (set){
        if (pPC->pCPU->fp_log)fprintf(pPC->pCPU->fp_log,"UPD16434 PSET(%i,%i)\n",info.dataPointer,bit);
        if (info.dataPointer>0x7f) {
            qWarning()<<"INDEX ERROR2";
        }
        info.imem[info.dataPointer] |= (0x01 << bit);
    }
    else {
        if (pPC->pCPU->fp_log)fprintf(pPC->pCPU->fp_log,"UPD16434 PRESET(%i,%i)\n",info.dataPointer,bit);
        if (info.dataPointer>0x7f) {
            qWarning()<<"INDEX ERROR3";
        }
        info.imem[info.dataPointer] &= ~(0x01 << bit);
    }
    if (mode == 0) { info.dataPointer++; info.dataPointer &= 0x7f; }
    if (mode == 1) { info.dataPointer--; info.dataPointer &= 0x7f; }

    updated = true;
    return 0;
}

BYTE CUPD16434::cmd_SMM(quint8 cmd)
{
    return 0;
}

BYTE CUPD16434::cmd_SFF(quint8 cmd)
{
    return 0;
}

BYTE CUPD16434::cmd_DISPON(quint8 cmd)
{
    return 0;
}

BYTE CUPD16434::cmd_DISPOFF(quint8 cmd)
{
    return 0;
}

BYTE CUPD16434::cmd_STOP(quint8 cmd)
{
    return 0;
}

bool CUPD16434::getBit()
{
    bool _ret = true;

    if ((info.mode >= 0x60) && (info.mode <= 0x63)) {
        _ret = (outputRegister & (1<<outputBit)) ? true : false;
//        qWarning()<<"UPD16434["<<id<<"]"<<" SO:"<<_ret<<" bit:"<<outputBit;
        outputBit--;
        if (outputBit == -1) {
            outputBit=7;
            outputRegister = info.imem[info.dataPointer];
//            qWarning()<<"UPD16434["<<id<<"]"<<" READ Next Byte="
//                     <<QString("%1").arg(outputRegister,2,16,QChar('0'))
//                     <<" from:"<<info.dataPointer;
            if ((info.mode & 0x03)==0) { info.dataPointer++; info.dataPointer &= 0x7f; }
            if ((info.mode & 0x03)==1) { info.dataPointer--; info.dataPointer &= 0x7f; }
        }
    }
    return _ret;
}

void CUPD16434::Load_Internal(QXmlStreamReader *xmlIn)
{
#if 1
    if (xmlIn->readNextStartElement()) {
        if ( (xmlIn->name()=="cpu") &&
             (xmlIn->attributes().value("model").toString() == "upd16434")) {
            QByteArray ba_reg = QByteArray::fromBase64(xmlIn->attributes().value("registers").toString().toLatin1());
            memcpy((char *) &info,ba_reg.data(),sizeof(info));
        }
        xmlIn->skipCurrentElement();
    }
#endif
}

void CUPD16434::save_internal(QXmlStreamWriter *xmlOut)
{

    xmlOut->writeStartElement("cpu");
        xmlOut->writeAttribute("model","upd16434");
        QByteArray ba_reg((char*)&info,sizeof(info));
        xmlOut->writeAttribute("registers",ba_reg.toBase64());
    xmlOut->writeEndElement();
}

