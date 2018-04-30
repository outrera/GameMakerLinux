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

#include "utils.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

// FILES

QJsonObject Utils::readFileToJSON(QString filename)
{
    QFile f(filename);
    if (!f.open(QFile::ReadOnly))
    {
        qCritical() << "Can't open file" << filename;
        return {};
    }

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(f.readAll(), &error);
    f.close();

    if (error.error != QJsonParseError::NoError)
    {
        qCritical() << "JSON error:" << error.errorString();
        return {};
    }

    return doc.object();
}

QString Utils::readFile(QString filename)
{
    QFile f(filename);
    if (!f.open(QFile::ReadOnly))
    {
        qCritical() << "Can't open file" << filename;
        return {};
    }

    auto str = f.readAll();
    f.close();

    return str;
}

// EVENTS

enum EventType {
    CreateEvent,
    DestroyEvent,
    AlarmEvent,
    StepEvent,
    CollisionEvent,
    KeyboardEvent,
    MouseEvent,
    OtherEvent,
    DrawEvent,
    KeyPressEvent,
    KeyReleaseEvent,
    AsyncEvent,
    CleanUpEvent,
    GestureEvent,
    EVENTS_COUNT
};

static QString eventsTypeFileNames[] = {
    "Create",
    "Destroy",
    "Alarm",
    "Step",
    "Collision",
    "Keyboard",
    "Mouse",
    "Other",
    "Draw",
    "KeyPress",
    "KeyRelease",
    "Trigger",
    "CleanUp",
    "Gesture"
};

static QMap<int, QString> eventsTypeNames[] = {
    { { 0, "Create" } },
    { { 0, "Destroy" } },
    { { 0, "Alarm" } },
    { // STEP
        { 0, "Step" },
        { 1, "Begin Step" },
        { 2, "End Step" }
    },
    { { 0, "Collision" } },
    { { 0, "Keyboard" } },
    { { 0, "Mouse" } },
    { // OTHER
        { 0, "Outside Room" },
        { 1, "Intersect Boundary" },
        { 2, "Game Start" },
        { 3, "Game End" },
        { 4, "Room Start" },
        { 5, "Room End" },
        { 6, "No lives(?)" },
        { 7, "Animation End" },
        { 8, "Path Ended" },
        { 9, "No health(?)" },
        { 10, "User Event 0" },
        { 11, "User Event 1" },
        { 12, "User Event 2" },
        { 13, "User Event 3" },
        { 14, "User Event 4" },
        { 15, "User Event 5" },
        { 16, "User Event 6" },
        { 17, "User Event 7" },
        { 18, "User Event 8" },
        { 19, "User Event 9" },
        { 20, "User Event 10" },
        { 21, "User Event 11" },
        { 22, "User Event 12" },
        { 23, "User Event 13" },
        { 24, "User Event 14" },
        { 25, "User Event 15" },
        { 30, "Close button(?)" },
        { 40, "Intersect View 0 Boundary" },
        { 41, "Intersect View 1 Boundary" },
        { 42, "Intersect View 2 Boundary" },
        { 43, "Intersect View 3 Boundary" },
        { 44, "Intersect View 4 Boundary" },
        { 45, "Intersect View 5 Boundary" },
        { 46, "Intersect View 6 Boundary" },
        { 47, "Intersect View 7 Boundary" },
        { 50, "Boundary View 0" },
        { 51, "Boundary View 1" },
        { 52, "Boundary View 2" },
        { 53, "Boundary View 3" },
        { 54, "Boundary View 4" },
        { 55, "Boundary View 5" },
        { 56, "Boundary View 6" },
        { 57, "Boundary View 7" },
        { 58, "Animation Update" },
        { 60, "Image Loaded" },
        { 61, "Sound Loaded" },
        { 62, "Async(?)" },
        { 63, "Dialog" },
        { 66, "In-App Purchase" },
        { 67, "Cloud" },
        { 68, "Networking" },
        { 69, "Steam" },
        { 70, "Social" },
        { 71, "Push Notification" },
        { 72, "Save/Load" },
        { 73, "Audio Recording" },
        { 74, "Audio Playback" },
        { 75, "System" },
    },
    { // DRAW
        {  0, "Draw" },
        { 64, "Draw GUI" },
        { 65, "Window Resize" },
        { 72, "Draw Begin" },
        { 73, "Draw End" },
        { 74, "Draw GUI Begin" },
        { 75, "Draw GUI End" },
        { 76, "Pre-Draw" },
        { 77, "Post-Draw" }
    },
    { { 0, "Key Press" }, },
    { { 0, "Key Release" } },
    { { 0, "Trigger" } },
    { { 0, "Clean Up" } },
    { // GESTURES
        { 0, "Tap" },
        { 1, "Double tap" },
        { 2, "Drag start" },
        { 3, "Drag move" },
        { 4, "Drag end" },
        { 5, "Flick" },
        { 6, "Pinch start" },
        { 7, "Pinch in" },
        { 8, "Pinch out" },
        { 9, "Pinch end" },
        { 10, "Rotate start" },
        { 11, "Rotating" },
        { 12, "Rotate end" },
        /* 64 -> 76 = Global 0 -> 12 */
    }
};

QString Utils::getEventName(int eventType, int eventNumber)
{
    if (eventType < EVENTS_COUNT)
    {
        switch (eventType)
        {
        case AlarmEvent:
            return QString("%1 %2").arg(eventsTypeNames[eventType][0]).arg(eventNumber);
        case KeyboardEvent:
        case KeyPressEvent:
        case KeyReleaseEvent:
            return QString("%1 - %2").arg(eventsTypeNames[eventType][0]).arg(eventNumber);
        case AsyncEvent:
            return QString("Async - %1").arg(eventsTypeNames[eventType][eventNumber]);
        case GestureEvent:
        {
            QString str;
            if (eventNumber > 60)
            {
                str = "Global ";
                eventNumber -= 64;
            }
            return QString("%1%2").arg(str, eventsTypeNames[eventType][eventNumber]);
        }
        case CreateEvent:
        case DestroyEvent:
        case StepEvent:
        case CollisionEvent:
        case MouseEvent:
        case OtherEvent:
        case DrawEvent:
        case CleanUpEvent:
            if (eventsTypeNames[eventType].contains(eventNumber))
            {
                return QString(eventsTypeNames[eventType][eventNumber]);
            }
        }
    }

    qDebug() << "Unknown:" << eventType << eventNumber;
    return QString("Invalid %1 %2").arg(eventType).arg(eventNumber);
}

QString Utils::getEventFileName(int eventType, int eventNumber)
{
    if (eventType < EVENTS_COUNT)
    {
        return QString("%1_%2").arg(eventsTypeFileNames[eventType]).arg(eventNumber);
    }

    return QString("invalid_%1_%2").arg(eventType).arg(eventNumber);
}

// RESOURCES

static QMap<QString, ResourceType> resourcesTypesStrings = {
    { "GMFolder",           ResourceType::Folder            },
    { "GMObject",           ResourceType::Object            },
    { "GMRoom",             ResourceType::Room              },
    { "GMSprite",           ResourceType::Sprite            },
    { "GMScript",           ResourceType::Script            },
    { "GMFont",             ResourceType::Font              },
    { "GMSound",            ResourceType::Sound             },
    { "GMMainOptions",      ResourceType::MainOptions,      },
    { "GMiOSOptions",       ResourceType::iOSOptions,       },
    { "GMIncludedFile",     ResourceType::IncludedFile,     },
    { "GMAmazonFireOptions",ResourceType::AmazonFireOptions,},
    { "GMLinuxOptions",     ResourceType::LinuxOptions,     },
    { "GMWindowsOptions",   ResourceType::WindowsOptions,   },
    { "GMAndroidOptions",   ResourceType::AndroidOptions,   },
    { "GMMacOptions",       ResourceType::MacOptions,       },
    { "GMConfig",           ResourceType::Config,           },
    { "GMTileSet",          ResourceType::TileSet,          },
    { "GMPath",             ResourceType::Path,             },
    { "GMNotes",            ResourceType::Notes,            },
    { "GMExtension",        ResourceType::Extension,        },
    { "GMTimeline",         ResourceType::Timeline,         },
    { "GMOptions",          ResourceType::Options,          },
    { "GMShader",           ResourceType::Shader,           },
    { "root",               ResourceType::Root,             }
};

QString Utils::resourceTypeToString(ResourceType type)
{
    return resourcesTypesStrings.key(type, "Unknown");
}

ResourceType Utils::resourceStringToType(QString type)
{
    if (resourcesTypesStrings.contains(type))
    {
        return resourcesTypesStrings[type];
    }
    qDebug() << "unknown type:" << type;
    return ResourceType::Unknown;
}
