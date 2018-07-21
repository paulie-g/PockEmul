#include <QMenu>
#include <QClipboard>
#include <QFileDialog>
#include <QPainter>
#include <QDebug>
#include <QBuffer>

#include "cprinter.h"
#include "paperwidget.h"
#include "Log.h"

//TODO  Lorsque pointeur positionne sur papier, afficher scroolbar verticale et gerer mousewheel
//TODO remove LoadSession_File et SaveSession_File and use the CPObject ones
Cprinter::Cprinter(CPObject *parent):CPObject(parent)
{
	PaperColor = QColor(255,255,255);
	paperWidget = 0;
    pos=QRect(0,0,0,0);

}

Cprinter::~Cprinter()
{
	delete paperWidget;
}

bool Cprinter::UpdateFinalImage(void) {

    CPObject::UpdateFinalImage();

    paintingImage.lock();
    QPainter painter;
    painter.begin(FinalImage);


    float ratio = ( (float) paperWidget->width() ) / ( paperWidget->bufferImage->width() - paperWidget->getOffset().x() );

//    ratio *= charsize;
    QRect source = QRect( QPoint(paperWidget->getOffset().x() ,
                                 paperWidget->getOffset().y()  - paperWidget->height() / ratio ) ,
                          QPoint(paperWidget->bufferImage->width(),
                                 paperWidget->getOffset().y() +10)
                          );
//    MSG_ERROR(QString("%1 - %2").arg(source.width()).arg(PaperPos().width()));

    QRect _target = QRect(PaperPos().topLeft()*internalImageRatio,PaperPos().size()*internalImageRatio);
//    qWarning()<<"internalImageRatio="<<internalImageRatio<<PaperPos()<<_target;
    painter.drawImage(_target,
                      paperWidget->bufferImage->copy(source).scaled(_target.size(),
                                                                    Qt::IgnoreAspectRatio,
                                                                    Qt::SmoothTransformation )
                      );

    painter.end();
    paintingImage.unlock();

//    emit updatedPObject(this);

    return true;
}

void Cprinter::resizeEvent ( QResizeEvent * ) {
    float ratio = (float)this->width()/getDX() ;

    if (!paperWidget) return;

    QRect rect = this->paperWidget->baseRect;
    this->paperWidget->setGeometry( rect.x()*ratio,
                                    rect.y()*ratio,
                                    rect.width()*ratio,
                                    rect.height()*ratio);
    this->paperWidget->updated=true;
}

QImage * Cprinter::checkPaper(QImage *printerbuf,int top) {
    int _height = printerbuf->height();
    if (top >= (_height-500)) {
        qWarning()<<"increase size:"<<_height;
        QImage *_tmp = printerbuf;
        printerbuf = new QImage(_tmp->width(),_height+500,QImage::Format_ARGB32);
        printerbuf->fill(PaperColor.rgba());

        qWarning()<<"increased size:"<<printerbuf->size();
        QPainter painter(printerbuf);
        painter.drawImage(0,0,*_tmp);
        painter.end();
        paperWidget->bufferImage = printerbuf;
        delete _tmp;

    }
    return printerbuf;
}

void Cprinter::BuildContextMenu(QMenu *menu)
{
    CPObject::BuildContextMenu(menu);

    QMenu * menuPaper = menu->addMenu(tr("Paper"));
    menuPaper->addAction(tr("Copy Image"),paperWidget,SLOT(paperCopy()));
    menuPaper->addAction(tr("Copy Text"),paperWidget,SLOT(paperCopyText()));
    menuPaper->addAction(tr("Cut"),paperWidget,SLOT(paperCut()));
    menuPaper->addAction(tr("Save Image ..."),paperWidget,SLOT(paperSaveImage()));
    menuPaper->addAction(tr("Save Text ..."),paperWidget,SLOT(paperSaveText()));

}

void Cprinter::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu *menu= new QMenu(this);

    BuildContextMenu(menu);

    menu->popup(event->globalPos () );
    event->accept();
}

void Cprinter::moveEvent ( QMoveEvent * event ) {
    if (paperWidget) paperWidget->updated=true;
}

void Cprinter::setPaperPos(QRect pos)
{
    qWarning()<<"setPaperPos"<<pos;
    this->pos = pos;
}

QRect Cprinter::PaperPos()
{
    return(pos);
}

void Cprinter::raise()
{
    if (paperWidget) paperWidget->updated = true;
    AddLog(LOG_TEMP,"RAISE");
    CPObject::raise();

}

bool Cprinter::SaveSession_File(QXmlStreamWriter *xmlOut)
{


    xmlOut->writeStartElement("session");
        xmlOut->writeAttribute("version", "2.0");

        SaveConfig(xmlOut);

    xmlOut->writeEndElement();  // session
    return true;
}

bool Cprinter::LoadSession_File(QXmlStreamReader *xmlIn)
{
    if (xmlIn->name()=="session") {
        LoadConfig(xmlIn);
    }

    return true;
}

bool Cprinter::LoadConfig(QXmlStreamReader *xmlIn)
{
    Q_UNUSED(xmlIn)

    int _x = xmlIn->attributes().value("posx").toString().toInt();
    int _y = xmlIn->attributes().value("posy").toString().toInt();
    paperWidget->setOffset(QPoint(_x,_y));

    TextBuffer.clear();
    TextBuffer = QByteArray::fromBase64(xmlIn->attributes().value("buffer").toString().toLatin1());
    if (!xmlIn->attributes().value("paper").isEmpty()) {
        paperWidget->bufferImage->loadFromData(
                    QByteArray::fromBase64(xmlIn->attributes().value("paper").toString().toLatin1()),"PNG");
    }

    return true;
}
bool Cprinter::SaveConfig(QXmlStreamWriter *xmlOut)
{
    Q_UNUSED(xmlOut)

    QByteArray ba;
    QBuffer buffer(&ba);
    paperWidget->bufferImage->save(&buffer, "PNG");

    xmlOut->writeAttribute("posx",QString("%1").arg(paperWidget->getOffset().x()));
    xmlOut->writeAttribute("posy",QString("%1").arg(paperWidget->getOffset().y()));
    xmlOut->writeAttribute("buffer",QString(TextBuffer.toBase64()));
    xmlOut->writeAttribute("paper",QString(ba.toBase64()));

    return true;
}
