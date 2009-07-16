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

#include <QLabel>
#include <QTabWidget>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <lastfm/User>
#include <lastfm/XmlQuery>
#include "MultiStarterWidget.h"
#include "SourceSelectorWidget.h"
#include "SourceListWidget.h"
#include "SearchBox.h"

#include <QGroupBox>


MultiStarterWidget::MultiStarterWidget(bool advanced, int maxSources, QWidget *parent)
    : QWidget(parent)
    , m_advanced(advanced)
    , m_minTagCount(10)
    , m_minArtistCount(10)
{
    QGridLayout* grid = new QGridLayout(this);

    QTabWidget* tabwidget = new QTabWidget();

    m_artists = new SourceSelectorWidget(new ArtistSearch());    
    tabwidget->addTab(m_artists, tr("Artists"));
    connect(m_artists, SIGNAL(add(QString)), SLOT(onAdd(QString)));
    connect(m_artists, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(onAddItem(QListWidgetItem*)));

    m_tags = new SourceSelectorWidget(new TagSearch());
    tabwidget->addTab(m_tags, tr("Tags"));
    connect(m_tags, SIGNAL(add(QString)), SLOT(onAdd(QString)));
    connect(m_tags, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(onAddItem(QListWidgetItem*)));

    m_users = new SourceSelectorWidget(new TagSearch());
    tabwidget->addTab(m_users, tr("Friends"));
    connect(m_users, SIGNAL(add(QString)), SLOT(onAdd(QString)));
    connect(m_users, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(onAddItem(QListWidgetItem*)));

    m_sourceList = new SourceListWidget(advanced, maxSources);

    QHBoxLayout* hLayout = new QHBoxLayout;
    if (advanced) {
        QVBoxLayout* slidersLayout = new QVBoxLayout();
        slidersLayout->addWidget(new QLabel(tr("Repetition")));
        slidersLayout->addWidget(m_repSlider = new QSlider(Qt::Horizontal));
        slidersLayout->addWidget(new QLabel(tr("Mainstreamness")));
        slidersLayout->addWidget(m_mainstrSlider = new QSlider(Qt::Horizontal));
        m_repSlider->setMinimum(0);
        m_repSlider->setMaximum(8);
        m_repSlider->setValue(4);
        m_mainstrSlider->setMinimum(0);
        m_mainstrSlider->setMaximum(8);
        m_mainstrSlider->setValue(4);
        hLayout->addLayout(slidersLayout);
    }
    hLayout->addWidget(m_playButton = new QPushButton(tr("Play")));

    grid->addWidget(tabwidget, 0, 0, 2, 1);
    grid->addWidget(m_sourceList, 0, 1, 1, 1);
    grid->addLayout(hLayout, 1, 1);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);

    connect(m_playButton, SIGNAL(clicked()), SLOT(onPlayClicked()));

    ///
    lastfm::AuthenticatedUser you;
    connect(you.getTopTags(), SIGNAL(finished()), SLOT(onUserGotTopTags()));
    connect(you.getTopArtists(), SIGNAL(finished()), SLOT(onUserGotTopArtists()));
    connect(you.getFriends(), SIGNAL(finished()), SLOT(onUserGotFriends()));
}

void
MultiStarterWidget::onAdd(const QString& item)
{
    SourceListWidget::SourceType itemType;
    if (m_artists == sender()) {
        itemType = SourceListWidget::Artist;
    } else if (m_tags == sender()) {
        itemType = SourceListWidget::Tag;
    } else if (m_users == sender()) {
        itemType = SourceListWidget::User;
    } else {
        return;
    }

    if (m_sourceList->addSource(itemType, item)) {
        // todo: grey it out if it's in the list?  or grey it some other way?
    }
}

void 
MultiStarterWidget::onAddItem(QListWidgetItem* item)
{
    SourceListWidget::SourceType itemType;
    if (m_artists == sender()) {
        itemType = SourceListWidget::Artist;
    } else if (m_tags == sender()) {
        itemType = SourceListWidget::Tag;
    } else if (m_users == sender()) {
        itemType = SourceListWidget::User;
    } else {
        return;
    }

    if (m_sourceList->addSource(itemType, item)) {
        // todo: grey it out if it's in the list?  or grey it some other way?
    }
}

void
MultiStarterWidget::onUserGotTopTags()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    QStringList tags;
    foreach (lastfm::XmlQuery e, lfm["toptags"].children("tag")) {
        tags += e["name"].text();
    }
    m_tags->list()->insertItems(0, tags);
    if (tags.size() < m_minTagCount) {
        // get global top tags
    }
}

void
MultiStarterWidget::onUserGotTopArtists()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    foreach (lastfm::XmlQuery e, lfm["topartists"].children("artist")) {
        QListWidgetItem* item = new QListWidgetItem(m_artists->list());
        item->setData(Qt::DisplayRole, e["name"].text());
        item->setData(Qt::DecorationRole, e["image size=medium"].text());
    }
    if (m_artists->list()->count() < m_minArtistCount) {
        // get global top artists
    }
}

void
MultiStarterWidget::onUserGotFriends()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    foreach (lastfm::XmlQuery e, lfm["friends"].children("user")) {
        QListWidgetItem* item = new QListWidgetItem(m_users->list());
        item->setData(Qt::DisplayRole, e["name"].text());
        item->setData(Qt::ToolTipRole, e["realname"].text());
        item->setData(Qt::DecorationRole, e["image size=medium"].text());
    }
    if (m_users->list()->count() < m_minArtistCount) {
        // no friends. so?
    }
}

void
MultiStarterWidget::onPlayClicked()
{
    QString rql = m_sourceList->rql();
    if (m_advanced) {
        float r = m_repSlider->value() / (float) m_repSlider->maximum();
        if (r != 0.5) {
            rql += QString(" opt:rep|%1").arg(r);
        }
        float m = m_mainstrSlider->value() / (float) m_mainstrSlider->maximum();
        if (m != 0.5) {
            rql += QString(" opt:mainstr|%1").arg(m);
        }
    }
    RadioStation r = RadioStation::rql(rql);
    r.setTitle(m_sourceList->stationDescription());
    emit startRadio(r);
}


//////

AdvancedComboWidget::AdvancedComboWidget(int maxSource, QWidget* parent)
: MultiStarterWidget(true, maxSource, parent)
{
}

SimpleComboWidget::SimpleComboWidget(int maxSource, QWidget* parent)
: MultiStarterWidget(false, maxSource, parent)
{
}

