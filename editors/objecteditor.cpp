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

#include "objecteditor.h"
#include "gamesettings.h"
#include "utils/utils.h"
#include "widgets/codeeditor.h"
#include "widgets/selectitem.h"
#include "resources/spriteresourceitem.h"
#include "models/sortedeventsmodel.h"

ObjectEditor::ObjectEditor(ObjectResourceItem* item)
    : MainEditor { item }
    , ui { new Ui::ObjectEditor }
{
    QTabWidget * tabWidget = new QTabWidget;
    ui->setupUi(tabWidget);
    setWidget(tabWidget);

    tabWidget->setCurrentIndex(0);

    auto setParentAction = ui->parentLineEdit->addAction(QIcon::fromTheme("edit-undo"), QLineEdit::TrailingPosition);
    connect(setParentAction, &QAction::triggered, this, &ObjectEditor::chooseParent);

    auto setMaskAction = ui->maskLineEdit->addAction(QIcon::fromTheme("edit-undo"), QLineEdit::TrailingPosition);
    connect(setMaskAction, &QAction::triggered, this, &ObjectEditor::chooseMask);

    auto * sortedModel = new SortedEventsModel;
    sortedModel->setSourceModel(&eventsModel);
    ui->eventsListView->setModel(sortedModel);
    ui->eventsListView->selectionModel()->select(sortedModel->index(0, 0), QItemSelectionModel::ClearAndSelect);

    connect(&eventsModel, &EventsModel::rowsInserted, this, &ObjectEditor::onEventsAdded);
    connect(&eventsModel, &EventsModel::rowsRemoved, this, &ObjectEditor::onEventsRemoved);
    connect(&eventsModel, &EventsModel::modelReset, this, &ObjectEditor::onEventsCleared);

    connect(ui->spriteViewer, &QPushButton::clicked, this, &ObjectEditor::chooseSprite);

    connect(ui->eventsListView, &QListView::clicked, [this](const QModelIndex & index) {
        ui->stackedCodeEditorWidget->setCurrentIndex(index.row());
    });

    connect(ui->nameLineEdit, &QLineEdit::textEdited, [this](QString) {
        this->setDirty();
    });

    reset();
}

void ObjectEditor::save()
{
    auto pItem = item<ObjectResourceItem>();

    // GENERAL SETTINGS
    auto name = ui->nameLineEdit->text();
    pItem->setName(name);
    pItem->setSprite(m_sprite);
    pItem->setMaskSprite(m_maskSprite);

    // EVENTS (TODO: improve?)
    for (int i = 0; i < ui->stackedCodeEditorWidget->count(); i++)
    {
        auto editor = qobject_cast<CodeEditor*>(ui->stackedCodeEditorWidget->widget(i));
        if (editor)
        {
            auto f = eventsModel.getFilename(i);
            Utils::writeFile(f, editor->getCode().toLocal8Bit());
        }
    }

    // HIERARCHY
    pItem->setParentObject(m_parentObject);

    pItem->save();

    setDirty(false);
}

void ObjectEditor::reset()
{
    auto pItem = item<ObjectResourceItem>();

    // GENERAL SETTINGS
    ui->nameLineEdit->setText(pItem->name());

    if (auto sprite = pItem->sprite())
    {
        auto pix = sprite->thumbnail();
        ui->spriteViewer->setIcon(pix);
    }
    else
    {
        ui->spriteViewer->setIcon({});
    }

    // EVENTS
    eventsModel.clear();

    for (int i = 0; i < pItem->eventsCount(); i++)
    {
        auto event = pItem->getEvent(i);
        eventsModel.addEvent(event);
    }

    // HIERARCHY
    if (pItem->parentObject() != nullptr)
    {
        m_parentObject = pItem->parentObject();
        ui->parentLineEdit->setText(m_parentObject->name());
    }
    else
    {
        m_parentObject = nullptr;
        ui->parentLineEdit->setText({});
    }

    auto objectItems = ResourceItem::findAll(ResourceType::Object);
    for (auto & childId : objectItems)
    {
        auto pChildItem = ResourceItem::get<ObjectResourceItem>(childId);
        auto pParent = pChildItem->parentObject();
        if (pParent && pParent->id() == pItem->id())
            ui->childrenTextEdit->appendPlainText(pChildItem->name());
    }

    setDirty(false);
}

void ObjectEditor::setDirty(bool dirty)
{
    if (isDirty() == dirty) return;

    if (dirty == false)
    {
        for (int i = 0; i < ui->stackedCodeEditorWidget->count(); i++)
        {
            auto editor = qobject_cast<CodeEditor*>(ui->stackedCodeEditorWidget->widget(i));
            if (editor)
            {
                editor->setDirty(dirty);
                eventsModel.setModified(i, false);
            }
        }
    }
    else
    {
        for (int i = 0; i < ui->stackedCodeEditorWidget->count(); i++)
        {
            auto widget = ui->stackedCodeEditorWidget->widget(i);
            if (widget == sender())
            {
                eventsModel.setModified(i, true);
            }
        }
    }

    MainEditor::setDirty(dirty);
}

void ObjectEditor::onEventsAdded(const QModelIndex & parent, int first, int last)
{
    Q_UNUSED(parent)
    for (int i = first; i <= last; i++)
    {
        auto editor = new CodeEditor;
        ui->stackedCodeEditorWidget->insertWidget(i, editor);

        connect(editor, &CodeEditor::dirtyChanged, this, &ObjectEditor::setDirty);

        auto filename = eventsModel.getFilename(i);
        auto code = Utils::readFile(filename);
        editor->setCode(code);
    }
}

void ObjectEditor::onEventsRemoved(const QModelIndex & parent, int first, int last)
{
    Q_UNUSED(parent)
    for (int i = first; i <= last; i++)
    {
        ui->stackedCodeEditorWidget->removeWidget(ui->stackedCodeEditorWidget->widget(first));
    }
}

void ObjectEditor::onEventsCleared()
{
    while (ui->stackedCodeEditorWidget->count() > 0)
    {
        ui->stackedCodeEditorWidget->removeWidget(ui->stackedCodeEditorWidget->widget(0));
    }
}

void ObjectEditor::chooseParent()
{
    ResourceItem * pItem = ResourceItem::findFolder(ResourceType::Object);
    SelectItem selectParent(pItem, item<ObjectResourceItem>());
    if (selectParent.exec())
    {
        auto parentItem = selectParent.choice();

        if (parentItem)
            ui->parentLineEdit->setText(parentItem->name());
        else
            ui->parentLineEdit->setText({});
        setDirty();
    }
}

void ObjectEditor::chooseMask()
{
    ResourceItem * pItem = ResourceItem::findFolder(ResourceType::Sprite);
    SelectItem selectMask(pItem);
    if (selectMask.exec())
    {
        auto maskItem = selectMask.choice();

        if (maskItem)
            ui->maskLineEdit->setText(maskItem->name());
        else
            ui->maskLineEdit->setText({});
        setDirty();
    }
}

void ObjectEditor::chooseSprite()
{
    ResourceItem * pItem = ResourceItem::findFolder(ResourceType::Sprite);
    SelectItem selectMask(pItem);
    if (selectMask.exec())
    {
        auto spriteItem = selectMask.choice();

        if (spriteItem)
        {
            auto sprite = qobject_cast<SpriteResourceItem*>(spriteItem);
            ui->spriteViewer->setIcon(sprite->thumbnail());
        }
        else
            ui->maskLineEdit->setText({});
        setDirty();
    }
}
