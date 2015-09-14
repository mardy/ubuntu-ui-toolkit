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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucitemextension.h"
#include "uctheme.h"
#include <QtQuick/private/qquickitem_p.h>
#include <QtQml/private/qqmlcomponentattached_p.h>
#include <QtGui/QGuiApplication>

/*
 * The UCItemExtension class provides theme handling on Items extending an existing
 * QQuickItem derivate class. Items subject fo theming should derive from this class
 * and implement the virtual methods, as well as add the following two methods:
 * 1) classBegin(item) should be called from the QQuickItem::classBegin() method
 * 2) handleThemeEvent() should be called from QQuickItem::customEvent() method
 * The item can expose the theme property and use the getters defined by the
 * class.
 */

int UCThemeEvent::themeUpdatedId = QEvent::registerEventType();
int UCThemeEvent::themeReloadedId = QEvent::registerEventType();

UCThemeEvent::UCThemeEvent(UCTheme *reloadedTheme)
    : QEvent((QEvent::Type)UCThemeEvent::themeReloadedId)
    , m_oldTheme(Q_NULLPTR)
    , m_newTheme(reloadedTheme)
{
}

UCThemeEvent::UCThemeEvent(UCTheme *oldTheme, UCTheme *newTheme)
    : QEvent((QEvent::Type)UCThemeEvent::themeUpdatedId)
    , m_oldTheme(oldTheme)
    , m_newTheme(newTheme)
{
}

UCThemeEvent::UCThemeEvent(const UCThemeEvent &other)
    : QEvent(other.type())
    , m_oldTheme(other.m_oldTheme)
    , m_newTheme(other.m_newTheme)
{
}

bool UCThemeEvent::isThemeEvent(const QEvent *event)
{
    return ((int)event->type() == themeUpdatedId) || ((int)event->type() == themeReloadedId);
}

void UCThemeEvent::handleEvent(QQuickItem *item, UCThemeEvent *event, bool synchronous)
{
    if (synchronous) {
        QGuiApplication::sendEvent(item, event);
    } else {
        QGuiApplication::postEvent(item, new UCThemeEvent(*event), Qt::HighEventPriority);
    }
}

void UCThemeEvent::forwardEvent(QQuickItem *item, UCThemeEvent *event)
{
    Q_FOREACH(QQuickItem *child, item->childItems()) {
        handleEvent(child, event, false);
        // StyledItem will handle the broadcast itself depending on whether the theme change was appropriate or not
        // and will complete the ascendantStyled/theme itself
        if (child->childItems().size() > 0 && (child->metaObject()->indexOfProperty("theme") < 0)) {
            forwardEvent(child, event);
        }
    }
}

void UCThemeEvent::broadcastThemeChange(QQuickItem *item, UCTheme *oldTheme, UCTheme *newTheme)
{
    UCThemeEvent event(oldTheme, newTheme);
    forwardEvent(item, &event);
}

void UCThemeEvent::broadcastThemeUpdate(QQuickItem *item, UCTheme *theme)
{
    UCThemeEvent event(theme);
    forwardEvent(item, &event);
}

/*************************************************************************
 * Attached to every Item in the system
 */
UCItemAttached::UCItemAttached(QObject *owner)
    : QObject(owner)
    , m_item(static_cast<QQuickItem*>(owner))
    , m_prevParent(Q_NULLPTR)
    , m_extension(Q_NULLPTR)
    , m_isItemThemed(m_item->metaObject()->indexOfProperty("theme") < 0)
{
    // get parent item changes
    connect(m_item, &QQuickItem::parentChanged, this, &UCItemAttached::handleParentChanged);
}

UCItemAttached::~UCItemAttached()
{
}

UCItemAttached *UCItemAttached::qmlAttachedProperties(QObject *owner)
{
    return new UCItemAttached(owner);
}

// handle parent changes
void UCItemAttached::handleParentChanged(QQuickItem *newParent)
{
    if (newParent == m_prevParent) {
        return;
    }

    // make sure we have these handlers attached to each intermediate item
    QQuickItem *oldThemedAscendant = UCItemExtension::ascendantThemed(m_prevParent);
    QQuickItem *newThemedAscendant = UCItemExtension::ascendantThemed(newParent);
    UCTheme *oldTheme = oldThemedAscendant ? oldThemedAscendant->property("theme").value<UCTheme*>() : &UCTheme::defaultTheme();
    UCTheme *newTheme = newThemedAscendant ? newThemedAscendant->property("theme").value<UCTheme*>() : &UCTheme::defaultTheme();

    if (oldTheme != newTheme) {
        // send the event to m_item first
        UCThemeEvent event(oldTheme, newTheme);
        if (m_extension) {
            // only items with extensions should get this event
            m_extension->handleThemeEvent(&event);
        }
        // then broadcast to children, but only if the item is not a styled one
        if (m_isItemThemed) {
            UCThemeEvent::forwardEvent(m_item, &event);
        }
    }
    m_prevParent = newParent;
}

void UCItemAttached::reloadTheme()
{
    m_extension->preThemeChanged();
    m_extension->postThemeChanged();
    // broadcast theme update
    UCThemeEvent::broadcastThemeUpdate(m_item, m_extension->getTheme());
}

/*************************************************************************
 *
 */
UCItemExtension::UCItemExtension()
    : themedItem(Q_NULLPTR)
    , attachedThemer(Q_NULLPTR)
    , theme(&UCTheme::defaultTheme())
    , themeType(Inherited)
{
}

// set the parent of the theme if the themeType is Custom
void UCItemExtension::setParentTheme()
{
    if (themeType != Custom) {
        return;
    }
    QQuickItem *upperThemed = ascendantThemed(QQuickItemPrivate::get(themedItem)->parentItem);
    UCItemAttached *attached = static_cast<UCItemAttached*>(qmlAttachedPropertiesObject<UCItemAttached>(upperThemed));
    UCTheme *parentTheme = (attached && attached->m_extension) ? attached->m_extension->getTheme() : &UCTheme::defaultTheme();
    if (parentTheme != theme) {
        theme->setParentTheme(parentTheme);
    }
}

void UCItemExtension::initTheming(QQuickItem *item)
{
    themedItem = item;
    attachedThemer = static_cast<UCItemAttached*>(qmlAttachedPropertiesObject<UCItemAttached>(themedItem));
    Q_ASSERT(attachedThemer);
    attachedThemer->m_extension = this;
    QObject::connect(theme, SIGNAL(nameChanged()), attachedThemer, SLOT(reloadTheme()));
    QObject::connect(theme, SIGNAL(versionChanged()), attachedThemer, SLOT(reloadTheme()));
}

void UCItemExtension::handleThemeEvent(UCThemeEvent *event)
{
    if ((int)event->type() == UCThemeEvent::themeUpdatedId) {
        switch (themeType) {
        case Inherited: {
            setTheme(event->newTheme(), Inherited);
            return;
        }
        case Custom: {
            // set the theme's parent
            theme->setParentTheme(event->newTheme());
            return;
        }
        default: break;
        }
    } else if ((int)event->type() == UCThemeEvent::themeReloadedId) {
        switch (themeType) {
        case Inherited: {
            // simply emit theme changed
            // reload theme
            preThemeChanged();
            postThemeChanged();
            UCThemeEvent::forwardEvent(themedItem, event);
            return;
        }
        case Custom: {
            // emit theme's parentThemeChanged()
            Q_EMIT theme->parentThemeChanged();
            return;
        }
        default: break;
        }
    }
}

UCTheme *UCItemExtension::getTheme() const
{
    return theme;
}
void UCItemExtension::setTheme(UCTheme *newTheme, ThemeType type)
{
    if (theme == newTheme) {
        return;
    }

    // preform pre-theme change tasks
    preThemeChanged();

    themeType = type;
    UCTheme *oldTheme = theme;

    // disconnect from the previous set
    if (theme) {
        QObject::disconnect(theme, SIGNAL(nameChanged()), attachedThemer, SLOT(reloadTheme()));
        QObject::disconnect(theme, SIGNAL(versionChanged()), attachedThemer, SLOT(reloadTheme()));
    }

    theme = newTheme;

    // connect to the new set
    if (theme) {
        QObject::connect(theme, SIGNAL(nameChanged()), attachedThemer, SLOT(reloadTheme()));
        QObject::connect(theme, SIGNAL(versionChanged()), attachedThemer, SLOT(reloadTheme()));
        // set the parent of the theme if custom
        setParentTheme();
    }

    // perform post-theme changes, update internal styling
    postThemeChanged();

    // broadcast to the children
    UCThemeEvent::broadcastThemeChange(themedItem, oldTheme, theme);
}

void UCItemExtension::resetTheme()
{
    QQuickItem *upperThemed = ascendantThemed(QQuickItemPrivate::get(themedItem)->parentItem);
    UCItemAttached *attached = static_cast<UCItemAttached*>(qmlAttachedPropertiesObject<UCItemAttached>(upperThemed));
    UCTheme *theme = (attached && attached->m_extension) ? attached->m_extension->getTheme() : &UCTheme::defaultTheme();
    setTheme(theme, Inherited);
}

// returns the closest themed ascendant
QQuickItem *UCItemExtension::ascendantThemed(QQuickItem *item)
{
    while (item && (item->metaObject()->indexOfProperty("theme") < 0)) {
        // make sure it also has the theming attached
        qmlAttachedPropertiesObject<UCItemAttached>(item);
        item = item->parentItem();
    }
    return item;
}


#include "moc_ucitemextension.cpp"
