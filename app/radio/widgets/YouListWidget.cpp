/*
   Copyright 2005-2009 Last.fm Ltd. 

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "YouListWidget.h"
#include "../SourceListModel.h"
#include "lib/unicorn/UnicornSettings.h"
#include <lastfm.h>


YouListWidget::YouListWidget(const QString& username, QWidget* parent)
    : QTreeWidget(parent) 
    , m_username(username)
    , m_subscriber(false)
{
    QString displayName = (username == lastfm::ws::Username) ? "Your " : username + "'s ";

    // subscribers can listen to loved tracks and personal tags.
    {
        unicorn::UserSettings us;
        QVariant v = us.value(unicorn::UserSettings::subscriptionKey(), false);
        m_subscriber = v.toBool();
    }

    setAlternatingRowColors( true );
    setHeaderHidden( true );
    setIndentation( 10 );
    //setRootIsDecorated( true );
    //setExpandsOnDoubleClick( false );
    //setItemsExpandable( true );
    QList<QTreeWidgetItem*> headerItems;
    QTreeWidgetItem *h, *item;
    headerItems << (h = new QTreeWidgetItem(QStringList(displayName + "Stations")));

    item = new QTreeWidgetItem(h, QStringList(displayName + "Library"));
    item->setData(0, SourceListModel::SourceType, RqlSource::User);
    item->setData(0, SourceListModel::Arg1, username);

    item = new QTreeWidgetItem(h, QStringList(displayName + "Recommendations"));
    item->setData(0, SourceListModel::SourceType, RqlSource::Rec);
    item->setData(0, SourceListModel::Arg1, username);

    item = new QTreeWidgetItem(h, QStringList(displayName + "Neighbourhood"));
    item->setData(0, SourceListModel::SourceType, RqlSource::Neigh);
    item->setData(0, SourceListModel::Arg1, username);

    headerItems << (m_friendsItem = new QTreeWidgetItem(QStringList(displayName + "Friends")));
    headerItems << (m_artistsItem = new QTreeWidgetItem(QStringList(displayName + "Artists")));

    lastfm::User user(username);
    connect(user.getFriends(), SIGNAL(finished()), SLOT(gotFriends()));
    connect(user.getTopArtists(), SIGNAL(finished()), SLOT(gotTopArtists()));

    addTopLevelItems( headerItems );
    h->setExpanded( true );
    h->setFlags( h->flags() & ~Qt::ItemIsSelectable );
}

void
YouListWidget::gotFriends()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm( lastfm::ws::parse( r ) );

    foreach (lastfm::XmlQuery e, lfm["friends"].children("user")) {
        QString name = e["name"].text();
        QTreeWidgetItem* item = new QTreeWidgetItem(m_friendsItem, QStringList(name));
        item->setToolTip(0, e["realname"].text());
        item->setData(0, SourceListModel::SourceType, RqlSource::User);
        item->setData(0, SourceListModel::Arg1, name);
        item->setData(0, SourceListModel::ImageUrl, e["image size=small"].text());

        // this will determine the sort order
        item->setText(0, name.toLower());
    }

    m_friendsItem->sortChildren(0, Qt::AscendingOrder);
}

void
YouListWidget::gotTopArtists()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    foreach (lastfm::XmlQuery e, lfm["topartists"].children("artist")) {
        QString artist = e["name"].text();
        QTreeWidgetItem* item = new QTreeWidgetItem(m_artistsItem, QStringList(artist));
        item->setData(0, SourceListModel::SourceType, RqlSource::SimArt);
        item->setData(0, SourceListModel::Arg1, artist);
        item->setData(0, SourceListModel::ImageUrl, e["image size=small"].text());
    }
}
