/**
* Copyright (c) 2010-2014 "Jabber Bees"
*
* This file is part of the ZcPostIt application for the Zeecrowd platform.
*
* Zeecrowd is an online collaboration platform [http://www.zeecrowd.com]
*
* ZcPostIt is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.5
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2

Item {

    property int postItX : 0
    property int postItY : 0
    property int postItZ : 0
    property string idItem : ""
    property alias textPostIt : textArea.text
    property alias postItColor: postIt.postItColor

    function clear() {
        postItX = 0;
        postItY = 0;
        postItZ = 0;
        postIt.width = 200;
        postIt.height = 200;
        textArea.text = ""
        idItem = "";
    }

    function getPositionString() {
        return postItX +"| " + postItY + "|" + postItZ + "|" + postIt.width + "|" + postIt.height + "|" + postItColor;
    }

    onIdItemChanged: {
        if (idItem === null || idItem === "")
            return;
        var local = mainView.localItems[idItem];
        if (local!==undefined) {
            postItX = local.x;
            postItY = local.y;
            postItZ = local.z;
            textArea.text =local.text
            postIt.width = local.width;
            postIt.height = local.height;
            postIt.postItColor = local.postItColor;
        }
    }

    Rectangle {
        anchors.fill : parent
        color : "transparent"
//        MouseArea {
//            anchors.fill: parent
//        }
    }

    FocusScope
    {
        id : postIt
        anchors.fill : parent

        property alias  text    : textArea.text

        property string postItColor : "yellow"

        BorderImage
        {
            id              : activeBorder
            border { left: 30; top: 30; right: 30; bottom: 30 }
            anchors.fill    : parent
            source                      : "Resources/postit_" + postIt.postItColor +  ".png"
        }

        state           : "readonly"

        Image
        {
            source                      : "Resources/tack.png"
            anchors.top                 : parent.top
            anchors.horizontalCenter    : parent.horizontalCenter
        }

        Item
        {
            id                      : mouseAreaItem
            anchors.bottom          : parent.bottom
            anchors.bottomMargin    : 30
            anchors.left            : parent.left
            anchors.leftMargin      : 20
            anchors.right           : parent.right
            anchors.rightMargin     : 20
            anchors.top             : parent.top
            anchors.topMargin       : 40
            clip                    : true

            TextArea
            {
                id                      : textArea
                anchors.top             : parent.top
                anchors.topMargin       : -2
                anchors.bottom          : parent.bottom
                anchors.bottomMargin    : -2
                anchors.left            : parent.left
                anchors.leftMargin      : -2
                anchors.right           : parent.right
                font.pixelSize          : 18
                anchors.rightMargin     : -2
                wrapMode                : TextEdit.WordWrap
                backgroundVisible: false

            }
        }

        Rectangle
        {
            x : 0
            y : 0

            color : "#00000000"
            visible : growMouseArea.drag.active ? true :  false
            width : grow.x
            height : grow.y

            border.width : 2
            border.color : "blue"
        }

        Rectangle
        {
            id : grow

            radius : 10
            color : "#00000000"
            border.width    : 2
            border.color    : "blue"


            width           : 20
            height          : 20
            x               : postIt.width - 10
            y               : postIt.height - 30

            MouseArea
            {
                id : growMouseArea

                anchors.fill  : parent

                drag.target     : grow
                drag.axis       : Drag.XAndYAxis
                drag.minimumX   : 0
                drag.minimumY   : 0

                onMouseXChanged: main.setPocketSize(idpocket,grow.x+10,grow.y+30);
                onMouseYChanged: main.setPocketSize(idpocket,grow.x+10,grow.y+30);
                onReleased:
                {
                    postIt.width = grow.x
                    postIt.height = grow.y
                    //postIt.positionChanged(postIt.x,postIt.y,postIt.z,postIt.width, postIt.height,postIt.postItColor)
                    //main.setGeo(idpocket,postIt.x,postIt.y,postIt.width, postIt.height);
                }
            }
        }
    }
}
