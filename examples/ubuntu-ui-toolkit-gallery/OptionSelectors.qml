/*
 * Copyright 2013 Canonical Ltd.
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

import QtQuick 2.0
import Ubuntu.Components 0.1

Template {
    TemplateSection {
        id: section

        className: "OptionSelector"

        Column {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right

           OptionSelector {
                text: i18n.tr("Label")
                expanded: false
                values: [i18n.tr("Value 1"),
                         i18n.tr("Value 2"),
                         i18n.tr("Value 3"),
                         i18n.tr("Value 4")]
            }
        }
    }
}
