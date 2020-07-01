
/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.12 as Kirigami
import kclock 1.0

Kirigami.ScrollablePage {
    
    title: "Timers"
    
    property bool createdTimer: false
    
    mainAction: Kirigami.Action {
        iconName: "list-add"
        text: "New Timer"
        onTriggered: {
            createdTimer = true
            timerModel.addNew();
        }
    }
    
    Kirigami.CardsListView {
        id: timersList
        model: timerModel
        
        // no timer placeholder
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: Kirigami.Units.largeSpacing
            visible: timersList.count == 0
            text: i18n("Add a timer")
            icon.name: "player-time"
        }
        
        // each timer
        delegate: Kirigami.AbstractCard {
            
            property Timer timerDelegate: timerModel.get(index)
            
            TimerPage {
                id: timerPage
                timer: timerDelegate
                visible: false
            }
            
            Component.onCompleted: {
                if (createdTimer)
                    pageStack.push(timerPage)
            }
            
            showClickFeedback: true
            onClicked: pageStack.push(timerPage)
            
            contentItem: Item {
                implicitWidth: delegateLayout.implicitWidth
                implicitHeight: delegateLayout.implicitHeight
                
                Column {
                    id: delegateLayout
                    anchors {
                        left: parent.left
                        top: parent.top
                        right: parent.right
                    }
                    
                    spacing: Kirigami.Units.largeSpacing
                    
                    ProgressBar {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        value: timerDelegate.elapsed / timerDelegate.length
                        enabled: timerDelegate.running
                    }
                    
                    RowLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        Kirigami.Heading {
                            level: 3
                            Layout.alignment: Qt.AlignLeft
                            text: timerDelegate.elapsedPretty
                        }
                        Kirigami.Heading {
                            level: 3
                            Layout.alignment: Qt.AlignRight
                            text: timerDelegate.lengthPretty
                        }
                    }
                    
                    Label {
                        text: timerDelegate.label
                    }
                }
            }
        }
    }
}
