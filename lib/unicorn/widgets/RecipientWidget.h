/*
   Copyright 2005-2009 Last.fm Ltd.
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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
#include <QPushButton>

#include "lib/unicorn/StylableWidget.h"
#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT RecipientWidget : public StylableWidget
{
    Q_OBJECT
private:
    struct
    {
        QLabel* recipient;
        QPushButton* deleteButton;
    } ui;

public:
    explicit RecipientWidget(const QString& recipient, QWidget* parent = 0);

    QString recipient() const;

    bool operator==(const RecipientWidget& that) const { return ui.recipient->text() == that.ui.recipient->text();}

signals:
    void deleted( RecipientWidget* deleted );

private slots:
    void onDeleteClicked();
};
