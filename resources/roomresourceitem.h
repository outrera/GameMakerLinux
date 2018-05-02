/*
    Copyright (C) 2018  Alexander Roper

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ROOMRESOURCEITEM_H
#define ROOMRESOURCEITEM_H

#include "resourceitem.h"

class RoomResourceItem : public ResourceItem
{
    Q_OBJECT

public:
    RoomResourceItem();

    void load(QJsonObject object) override;

    QVector<RoomLayer *> layers() const;

private:
    QVector<RoomLayer *> m_layers;
};

#endif // ROOMRESOURCEITEM_H
