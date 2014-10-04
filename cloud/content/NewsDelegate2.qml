/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtQml module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import "counter.js" as Counter

import ".."

Item {
    id: delegate
    height: column.height + 40
    width: delegate.ListView.view.width

    // Create a property to contain the visibility of the details.
    // We can bind multiple element's opacity to this one property,
    // rather than having a "PropertyChanges" line for each element we
    // want to fade.
    property real detailsOpacity : 0

    property bool ismine: (username == cloud.getValueFor("username",""))
    property bool changed: false
    property int newpublicstatus: ispublic

    // A simple rounded rectangle for the background
    Rectangle {
        id: background
        x: 2; y: 2; width: parent.width - x*2; height: parent.height - y*2
        color: "white"
        border.color: "orange"
        radius: 5
    }

    MouseArea {
        anchors.fill: parent
        onClicked: delegate.state = 'Details';
    }

    Column {
        id: column
        x: 20; y: 20
        width: parent.width - 30
        //        height: background.height
        Row {
            width: parent.width;
            spacing: 5
            //    *     0 => 'Private',
            //    *    -2 => 'Friends',
            //    *     1 => 'Logged in users',
            //    *     2 => 'Public',
            Image {
                id: accessIcon
                source: (access_id == -2 ? "images/friend.png" :"images/public.png")
                width: titleText.height *.5
                height: titleText.height * .5
                visible: (access_id!=0)
            }
            Edit {
                id: titleText
                text: (title=="")?"No title":title
                nbLine: 1
                readOnly: (detailsOpacity==0) || (!ismine)
                interactive: !readOnly

                wrapMode: Text.WordWrap
                width: parent.width - 30 - (buttonColumn.visible? buttonColumn.width:0) - 5
                font { bold: true; family: "Helvetica"; pointSize: 14 }
                textColor: changed ? "red" : "black"
                onTextChanged: {
                    checkmodif()
                }
            }
        }
        Row {
            width: parent.width;
            spacing: 5
            Image {
                id: username_avatar
                source: avatar_url
            }

            Text {
                id: usernameText
                text: username;
                //            width: parent.width;
                wrapMode: Text.WordWrap
                font { bold: false; family: "Helvetica"; pointSize: 14 }
            }
        }
        Row {
            Image {
                id: pmlThumbImage
                width: 100
                height: 100
                asynchronous: true
                cache: false
                source:
                        "image://PockEmulCloud/"+
                        cloud.getValueFor("serverURL","").replace("http://","")+
                        "services/api/rest/xml/?method=file.get_snap"+
                        "&file_guid="+pmlid+
                        "&size=medium"
//                        "mod/file/thumbnail.php"+
//                        "?file_guid="+pmlid+
//                        "&size=medium"

                    //serverURL+"getPMLthumb/"+pmlid+"/"+getThumbId(pmlid)
                fillMode: Image.PreserveAspectFit;
                Timer {
                        id: reset
                        interval: 500;
                        onTriggered: pmlThumbImage.source="image://PockEmulCloud/"+
                                     cloud.getValueFor("serverURL","").replace("http://","")+
                                     "services/api/rest/xml/?method=file.get_snap"+
                                     "&file_guid="+pmlid+
                                     "&size=medium"
//                                     "mod/file/thumbnail.php"+
//                                     "?file_guid="+pmlid+
//                                     "&size=medium"
//                                     "&ghost="+getThumbId(pmlid);
                    }
                onStatusChanged: {
                    if (status == Image.Error) {
                        source = "";
                        reset.restart();
                    }
//                                     console.log("*****image status("+index+")="+pmlThumbImage.status);
                }

//                MouseArea {
//                    anchors.fill: parent
////                    onClicked: {
////                        root.bigposter.source = serverURL+"getPMLsnap/"+pmlid;
////                        root.bigposter.visible = true;
////                    }
//                }
            }
//            TextEdit {
//                id: objectsText
//                text: objects
//                readOnly: true
//                opacity: delegate.detailsOpacity
//            }
        }

        Edit {
            id: descriptionText
            width: parent.width;
//            height:background.height - y - 25
            text: (description=="")?"No description":description
            readOnly: !ismine
//            wrapMode: Text.WordWrap;
            font.family: "Helvetica"; font.pointSize: 14
//            nbLine: 4
            textColor: changed ? "red" : "black"
            visible: (delegate.detailsOpacity == 1)
            opacity: delegate.detailsOpacity
            onTextChanged: checkmodif()
        }
        Item {
            id: comments
        }
    }


    Item {
        id: details
        opacity: delegate.detailsOpacity
    }
    Column {
        id: buttonColumn
        visible: (delegate.detailsOpacity==1) || ((width+pmlThumbImage.width)<column.width)
        y:40
        anchors { right: background.right; rightMargin: 10 }
        spacing: 5
        TextButton {
            id: closeButton
            visible: (delegate.detailsOpacity == 1)
            opacity: delegate.detailsOpacity
            text: "Close"
            onClicked: {
                if (changed) {
                    root.sendWarning("Cancel changes before closing.");
                }
                else {
                    delegate.state = '';
                    delegate.ListView.view.interactive= true;
                }
            }
        }
        TextButton {
            id: downloadButton
            text: "Download File"
            onClicked: cloud.getPML(pmlid,1,root.auth_token);
        }
        TextButton {
            id: importButton
            visible: !ismine && (delegate.detailsOpacity == 1)
            opacity: delegate.detailsOpacity
            text: "Clone to private"
            onClicked: {
                root.clone_pml(pmlid,
                               function(){xmlpmlModel.reload();},
                               function(){}
                               );
            }
        }
        //    *     0 => 'Private',
        //    *    -2 => 'Friends',
        //    *     1 => 'Logged in users',
        //    *     2 => 'Public',
        TextButton {
            id: makePrivate
            visible: (access_id != 0) && ismine && (delegate.detailsOpacity == 1)
            opacity: delegate.detailsOpacity
            text: "Make private"
            onClicked: {
                root.set_access(pmlid,0,
                               function(){

                                   refpmlModel.setProperty(rowid,"access_id",0);
                                   pmlModel.setProperty(index,"access_id",0);

//                                   pmlview.populate(newprivateSearchItem.text);
                               },
                               function(){}
                               );
            }
        }
        TextButton {
            id: makeFriend
            visible: (access_id != -2) && ismine && (delegate.detailsOpacity == 1)
            opacity: delegate.detailsOpacity
            text: "Share with Friends"
            onClicked: {
                root.set_access(pmlid,-2,
                               function(){

                                   refpmlModel.setProperty(rowid,"access_id",-2);
                                   pmlModel.setProperty(index,"access_id",-2);

//                                   pmlview.populate(newprivateSearchItem.text);
                               },
                               function(){}
                               );
            }
        }
        TextButton {
            id: makePublic
            visible: (access_id != 2) && ismine
            opacity: delegate.detailsOpacity
            text: "Make public"
            onClicked: {
                root.set_access(pmlid,2,
                               function(){

                                   refpmlModel.setProperty(rowid,"access_id",2);
                                   pmlModel.setProperty(index,"access_id",2);
//                                   pmlview.populate(newprivateSearchItem.text);
                               },
                               function(){}
                               );
            }
        }
        TextButton {
            id: saveCurrentSessionButtonIn
            text: "Save current session"
            visible: ismine && (delegate.detailsOpacity == 1)
            opacity: delegate.detailsOpacity
            onClicked: {
                if (cloud.askDialog("Do you want to overwrite this session file ?",2)==2) return;
                var serverURL = cloud.getValueFor("serverURL","")+'services/api/rest/json/';
                var url = serverURL+ '?method=file.update_pmldata'+
                        '&file_guid='+pmlid+
                        '&api_key=7118206e08fed2c5ec8c0f2db61bbbdc09ab2dfa'+
                        '&auth_token='+auth_token;
                var xml = cloud.save();
                console.log('url:'+url);
                requestPost(url, xml , function (o) {

                    if (o.readyState == 4 ) {
                        if (o.status==200) {
                            var obj = JSON.parse(o.responseText);
                            console.log(o.responseText);
                            if (obj.status == 0) {
                                message.showMessage("Session updated",5000);
                                // refresh thumb
                                cloud.clearCache(pmlThumbImage.source);
                                pmlThumbImage.source="";
                                reset.restart();
                            }
                            else {
                                message.showErrorMessage(obj.message,5000);
                            }
                        }
                    }
                });



            }

        }
        TextButton {
            id: saveButton
            visible: changed
            text: "Save"
            onClicked: {
                var serverURL = cloud.getValueFor("serverURL","")+'services/api/rest/json/';
                var url = serverURL+ '?method=file.update&'+
                        '&guid='+pmlid+
                        '&access='+newpublicstatus+
                        '&title='+encodeURIComponent(titleText.text)+
                        '&description='+encodeURIComponent(descriptionText.text)+
                        '&api_key=7118206e08fed2c5ec8c0f2db61bbbdc09ab2dfa'+
                        '&auth_token='+auth_token;

                console.log('url:'+url);
                //                if (cloud.askMsg("Are you sure ?",2) == 1)
                {
                    // update data
                    requestPost(url, "",function (o) {
                        if (o.readyState == 4) {
                            //                        console.log("status:"+o.status);
                            if (o.status==200) {
                                //                        cloud.askMsg("Ok, saved !",1);
                                //Reload record
                                changed = false;
                                var publicstatuschanged = false;
                                refpmlModel.setProperty(rowid,"title",titleText.text);
                                refpmlModel.setProperty(rowid,"description",descriptionText.text);
                            }
                        }
                    });
                }

            }
        }
        TextButton {
            id: revertButton
            //            y: 100
            //            anchors { right: background.right; rightMargin: 10 }
            visible: changed
            text: "Cancel changes"
            onClicked: {
                titleText.text = (title=="")?"No title":title
                descriptionText.text = (description=="")?"No description":description
                newpublicstatus = ispublic
                switchelt.state = (ispublic==1) ? "on" :"off"
                checkmodif()
            }
        }
        TextButton {
            id: undeleteButton
            text: "UnDelete"
            visible: (isdeleted ==1) && ismine
            onClicked: {
                var url = serverURL + "undelPML/" + currentApiKey +"/" + pmlid;
                requestGet(url,function (o) {
                    if (o.readyState == 4 ) {
                        if (o.status==200) {
                            refpmlModel.setProperty(rowid,"isdeleted",0);
                            cloud.saveCache(cacheFileName,serializerefpmlModel());
                            pmlview.categoryListView.currentIndex = 0;
                            pmlview.objid = 0;
                            populatePMLModel("");
                            populateCategoryModel("");
                            // Position at the just undeleted item
                            console.log("****search pmlid:"+pmlid)
                            pmlview.focusPml(pmlid);

                        }}
                });
            }
        }
        TextButton {
            id: deleteButton
            visible: ismine && (delegate.detailsOpacity == 1)
            opacity: delegate.detailsOpacity
            text: (isdeleted ==1) ? "Permanently delete" : "Delete"
            onClicked: {
                if (cloud.askDialog("Do you want to delete this session ?",2)==2) return;

                root.delete_pml(pmlid,
                               function(){xmlpmlModel.reload();},
                               function(){}
                               );
            }
        }

    }

    function checkmodif() {
        if ( (titleText.text == ((title=="")?"No title":title)) &&
                (descriptionText.text == ((description=="")?"No description":description)) &&
                (ispublic == newpublicstatus)) {
            changed = false;
        }
        else {
//            console.log("title:"+titleText.text+" vs "+((title=="")?"No title":title));
//            console.log("desc:"+ descriptionText.text+" vs "+((description=="")?"No description":description));
//            console.log("public:"+ispublic+ "vs "+newpublicstatus);
            changed = true;
        }
    }

    states: State {
        id: viewstate
        name: "Details"

        PropertyChanges { target: background; color: "white" }
        PropertyChanges { target: pmlThumbImage; width: 200; height: 200;} // Make picture bigger
        PropertyChanges { target: delegate; detailsOpacity: 1; x: 0 } // Make details visible
        PropertyChanges { target: delegate; height: list.height } // Fill the entire list area with the detailed view
        PropertyChanges { target: categoriesView; width: root.isPortrait?0:categoriesView.width }
        // Move the list so that this item is at the top.
        PropertyChanges { target: delegate.ListView.view; explicit: true; contentY: delegate.y }

        // Disallow flicking while we're in detailed view
//        PropertyChanges { target: delegate.ListView.view; interactive: false }
        PropertyChanges { target: delegate.ListView.view; interactive: false }
    }

    transitions: Transition {
        // Make the state changes smooth
        ParallelAnimation {
            ColorAnimation { property: "color"; duration: 500 }
            NumberAnimation { duration: 300; properties: "detailsOpacity,x,contentY,height,width" }
        }
    }

    Rectangle {
        width: parent.width; height: 1; color: "#cccccc"
        anchors.bottom: parent.bottom
    }

}
