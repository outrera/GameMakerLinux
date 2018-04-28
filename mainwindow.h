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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "resources/resourceitem.h"

namespace Ui {
class MainWindow;
}

class ResourcesModel;
class ResourcesTreeDock;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openProject();
    void openRoom(RoomResourceItem* item);
    void openSprite(SpriteResourceItem* item);
    void openScript(ScriptResourceItem* item);
    void openAndroidOptions(AndroidOptionsResourceItem* item);
    void openAmazonFireOptions(AmazonFireOptionsResourceItem* item);
    void openWindowsOptions(WindowsOptionsResourceItem* item);
    void openLinuxOptions(LinuxOptionsResourceItem* item);
    void openMacOptions(MacOptionsResourceItem* item);
    void openiOsOptions(iOSOptionsResourceItem* item);
    void openIncludedFile(IncludedFileResourceItem* item);
    void openMainOptions(MainOptionsResourceItem* item);
    void openObject(ObjectResourceItem* item);

private:
    void loadProject(QString filename);
    bool checkTab(QString id);
    void closeTab(int pos);

    Ui::MainWindow * ui;
    ResourcesModel * resourcesModel;
    ResourcesTreeDock * resourcesTreeDock;
    QTabWidget * tabWidget;
    QVector<QString> idOfOpenedTabs;
    QMap<QString, ResourceItem*> resources;
};

#endif // MAINWINDOW_H
