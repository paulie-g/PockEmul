#include <QtGui>
#include <QTime>
#include <QFileDialog>
#include <QThread>
#include <QDebug>

#include "common.h"
#include "fluidlauncher.h"

#include "rlp1002.h"
#include "rlh1000.h"
#include "pcxxxx.h"
#include "Log.h"
#include "watchpoint.h"
#include "paperwidget.h"
#include "Keyb.h"
#include "Connect.h"
#include "buspanasonic.h"

#define DOWN    0
#define UP              1


Crlp1002::Crlp1002(CPObject *parent):Cce515p(this)
{ //[constructor]
    Q_UNUSED(parent)

    setfrequency( 0);

    margin = 25;
    BackGroundFname     = P_RES(":/rlh1000/rlp1002.png");
    setcfgfname("rlp1002");


    pTIMER              = new Ctimer(this);
    pKEYB               = new Ckeyb(this,"rlp1002.map");
    setDXmm(227);
    setDYmm(95);
    setDZmm(31);


    setDX(848);
    setDY(340);

    setPaperPos(QRect(57,-20,318,236));


    rotate = false;
    INTrequest = false;
    printing = false;
    receiveMode = false;
    CRLFPending = false;

    memsize             = 0x2000;
    InitMemValue        = 0x7f;
    SlotList.clear();
    SlotList.append(CSlot(8 , 0x0000 ,  P_RES(":/rlh1000/rlp1002.bin")    , "" , CSlot::ROM , "Printer ROM"));

}

Crlp1002::~Crlp1002() {

    delete pCONNECTOR;

}


#define LATENCY (pTIMER->pPC->getfrequency()/3200)
bool Crlp1002::run(void)
{
    static quint64 _state=0;
    CbusPanasonic bus;

    bus.fromUInt64(pCONNECTOR->Get_values());

    if (bus.getFunc()==BUS_SLEEP) return true;

    if (bus.getDest()!=0) return true;
    bus.setDest(0);


//    qWarning()<<"PRINTER:"<<bus.toLog();
    if ( (bus.getFunc()==BUS_LINE0) && !bus.isWrite() ) {
//        qWarning()<<"1002: read BUS_LINE0:"<<bus.getData();
        bus.setData(0x00);
        bus.setFunc(BUS_READDATA);
        pCONNECTOR->Set_values(bus.toUInt64());
        return true;
    }
    if ( (bus.getFunc()==BUS_LINE0) && bus.isWrite() ) {
        qWarning()<<"1002: write BUS_LINE0:"<<bus.getData();
        bus.setFunc(BUS_ACK);
        return true;
    }

    if ( (bus.getFunc()==BUS_LINE1) && bus.isWrite() ) {
        qWarning()<<"1002: write BUS_LINE1:"<<bus.getData();
        if (receiveMode) {
            Command( bus.getData());

            INTrequest = true;
        }
        bus.setFunc(BUS_ACK);
    }
    if ( (bus.getFunc()==BUS_LINE1) && !bus.isWrite() ) {
        qWarning()<<"1002: read BUS_LINE1:"<<bus.getData();
        bus.setFunc(BUS_ACK);
    }

    if ( (bus.getFunc()==BUS_LINE2) && bus.isWrite() ) {
//        qWarning()<<"1002 BUS SELECT:"<<bus.getData();

        switch (bus.getData()) {
        case 1: Power = true; break;
        default: Power = false; break;
        }
        if (Power)
        {
            bus.setFunc(BUS_READDATA);
            bus.setData(0x01);
            pCONNECTOR->Set_values(bus.toUInt64());
        }
        return true;
    }
    if ( (bus.getFunc()==BUS_LINE2) && !bus.isWrite() ) {
        qWarning()<<"1002: read BUS_LINE2:"<<bus.getData();
        bus.setFunc(BUS_ACK);
    }

    if ( (bus.getFunc()==BUS_LINE3) && bus.isWrite() ) {
        qWarning()<<"1002: write BUS_LINE3:"<<bus.getData();
        INTrequest = true;

        bus.setFunc(BUS_ACK);
    }

    if ( (bus.getFunc()==BUS_LINE3) && !bus.isWrite() ) {
//        qWarning()<<"1002: read BUS_LINE3:"<<bus.getData();
        if (INTrequest) {
            qWarning()<<"INTREQUEST:true";
            bus.setINT(true);
            bus.setData(0x00);
            INTrequest = false;
        }
        else {
            qWarning()<<"INTREQUEST:false";
            bus.setData(0xff);
        }
        bus.setFunc(BUS_READDATA);
        pCONNECTOR->Set_values(bus.toUInt64());
        return true;
       }



    if (!Power) return true;

    quint32 adr = bus.getAddr();

    switch (bus.getFunc()) {
    case BUS_SLEEP: break;
    case BUS_WRITEDATA:
//        qWarning()<<tr("Try to write:%1").arg(adr,4,16,QChar('0'));
        break;
    case BUS_READDATA:
        if ( (adr>=0x2000) && (adr<0x3000) ) bus.setData(mem[adr-0x2000]);
        else {
//            qWarning()<<tr("Try to read:%1").arg(adr,4,16,QChar('0'));
            bus.setData(0x7f);
        }
        break;
    default: break;

    }

    pCONNECTOR->Set_values(bus.toUInt64());
    return true;

}





bool Crlp1002::init(void)
{
    Cce515p::init();

    setfrequency( 0);

    pCONNECTOR = new Cconnector(this,44,0,
                                    Cconnector::Panasonic_44,
                                    "44 pins conector",
                                    true,
                                    QPoint(37,72),
                                    Cconnector::WEST);
    publish(pCONNECTOR);
    WatchPoint.add(&pCONNECTOR_value,64,44,this,"Printer connector");
    AddLog(LOG_PRINTER,tr("PRT initializing..."));

    if(pKEYB)   pKEYB->init();
    if(pTIMER)  pTIMER->init();

    paperWidget->hide();
    return true;
}



/*****************************************************/
/* Exit PRINTER                                                                          */
/*****************************************************/
bool Crlp1002::exit(void)
{
    AddLog(LOG_PRINTER,"PRT Closing...");
    AddLog(LOG_PRINTER,"done.");
    Cce515p::exit();
    return true;
}


/*****************************************************/
/* CE-126P PRINTER emulation                                             */
/*****************************************************/



#define         WAIT ( pPC->frequency / 10000*6)

#define RECEIVE_MODE    1
#define SEND_MODE               2
#define TEST_MODE               3



bool Crlp1002::Get_Connector(Cbus *_bus) {
    return true;
}

bool Crlp1002::Set_Connector(Cbus *_bus) {
    return true;
}


void Crlp1002::Rotate()
{
    rotate = ! rotate;

    delete BackgroundImageBackup;
    BackgroundImageBackup = CreateImage(QSize(getDX(), getDY()),BackGroundFname,false,false,rotate?180:0);
    delete BackgroundImage;
    BackgroundImage = new QImage(*BackgroundImageBackup);
    delete FinalImage;
    FinalImage = new QImage(*BackgroundImageBackup);

    pCONNECTOR->setSnap(rotate?QPoint(811,72):QPoint(37,72));

    pCONNECTOR->setDir(rotate?Cconnector::EAST:Cconnector::WEST);
    mask = QPixmap::fromImage(*BackgroundImageBackup).scaled(getDX()*mainwindow->zoom/100,getDY()*mainwindow->zoom/100);
    setMask(mask.mask());

    update();

        // adapt SNAP connector
}


extern int ask(QWidget *parent,QString msg,int nbButton);
#define KEY(c)	((pKEYB->keyPressedList.contains(TOUPPER(c)) || \
                  pKEYB->keyPressedList.contains(c) || \
                  pKEYB->keyPressedList.contains(TOLOWER(c)))?1:0)
void Crlp1002::ComputeKey(KEYEVENT ke,int scancode)
{
    Q_UNUSED(ke)
    Q_UNUSED(scancode)

    if (pKEYB->LastKey == K_PFEED) {
        qWarning()<<"PaperFeed";
        PaperFeed();
    }

}

