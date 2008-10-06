/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
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

#include "MbidJob.h"
#include <QDebug>


void
MbidJob::run()
{
    if (isValid())
        m_mbid = Mbid::fromLocalFile( m_track.url().path() );
}


void
MbidJob::onFinished()
{
    if (!m_mbid.isNull())
    {
        MutableTrack( m_track ).setMbid( m_mbid );
        qDebug() << "Found MBID for" << m_track << m_mbid;
    }
    else
        qDebug() << "No MBID found for" << m_track;    
}


bool
MbidJob::isValid() const
{
    //FIXME really we should check the file header?
    return m_track.url().scheme() == "file" &&
           m_track.url().path().endsWith( ".mp3", Qt::CaseInsensitive );
}
