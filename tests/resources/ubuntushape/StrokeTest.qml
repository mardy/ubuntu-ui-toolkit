/*
 * Copyright 2015 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Private 1.3

Item {
    id: root
    width: 600
    height: 600
    focus: true

    property string textOverlayString:
        "size     (s/S): " + stroke.size.toFixed(1) + "\n" +
        "color      (c): " + stroke.color + "\n" +
        "opacity  (o/O): " + stroke.opacity.toFixed(2) + "\n"

    Item {
        id: scene
        anchors.fill: parent

        Stroke {
            id: stroke
            anchors.fill: parent
            anchors.margins: 100.0
            color: "blue"
        }
    }

    Text {
        id: textOverlay
        width: 200
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.family: "Ubuntu Mono"
        font.pixelSize: 14
        font.weight: Font.Bold
        color: "black"
        text: textOverlayString
    }

    Keys.onPressed: {
        var shift = event.modifiers & Qt.ShiftModifier;
        if (event.key == Qt.Key_S) {
            stroke.size = stroke.size + (shift ? 1.0 : -1.0);
        } else if (event.key == Qt.Key_C) {
            stroke.color = Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0);
        } else if (event.key == Qt.Key_O) {
            stroke.opacity = stroke.opacity + (shift ? 0.02 : -0.02);
        }
    }
}
