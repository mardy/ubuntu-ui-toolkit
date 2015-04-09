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

/*!
    \qmlabstract StyledItem
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup theming
    \brief The StyledItem class allows items to be styled by the theme.

    StyledItem provides facilities for making an Item stylable by the theme.

    In order to make an Item stylable by the theme, it is enough to make the Item
    inherit from StyledItem and set its \l style property to be the result of the
    appropriate call to theme.createStyleComponent().

    Example definition of a custom Item MyItem.qml:
    \qml
        StyledItem {
            id: myItem
            style: theme.createStyleComponent("MyItemStyle.qml", myItem)
        }
    \endqml

    The Component set on \l style is instantiated and placed below everything else
    that the Item contains.

    A reference to the Item being styled is accessible from the style and named
    'styledItem'.

    \sa {Theme}
*/
StyledItemBase {
    id: styledItem

//    implicitWidth: __styleInstance ? __styleInstance.implicitWidth : 0
//    implicitHeight: __styleInstance ? __styleInstance.implicitHeight : 0
}
