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

#include "spriteresourceitem.h"
#include "dependencies/spriteframe.h"
#include <QJsonArray>
#include <QJsonValue>

SpriteResourceItem::SpriteResourceItem()
    : ResourceItem(ResourceType::Sprite)
{
}


void SpriteResourceItem::load(QJsonObject object)
{
    setName(object["name"].toString());

    auto frames = object["frames"].toArray();
    for (const auto & frameJson : frames)
    {
        auto frame = new SpriteFrame;
        frame->load(frameJson.toObject());
        m_frames.push_back(frame);
    }
}
