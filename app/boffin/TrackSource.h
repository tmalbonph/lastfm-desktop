/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef TRACK_SOURCE_H
#define TRACK_SOURCE_H

#include <QList>
#include <lastfm/Track>
#include "BoffinRqlRequest.h"


class TrackSource 
    : public QObject
{
    Q_OBJECT

public:
    TrackSource(BoffinRqlRequest* req);    

signals:
    void ready( QList<Track>  );

private slots:
    void onPlayableItems(QList<BoffinPlayableItem> tracks);

private:
    QList<BoffinPlayableItem> m_tracks;
};

#endif