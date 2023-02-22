//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef COMMANDDESIGNERWIDGET_HPP_
#define COMMANDDESIGNERWIDGET_HPP_

#include <QDialog>

class CommandLineEdit;

#include "CtsCmdRegistry.hpp"
#include "CustomCommandHandler.hpp"
#include "MenuHandler.hpp"
#include "NodeQueryResultModel.hpp"
#include "ui_CommandDesignerWidget.h"

class CommandDesignerWidget : public QWidget, private Ui::commandDesignerWidget {
    Q_OBJECT

public:
    explicit CommandDesignerWidget(QWidget* parent = nullptr);
    ~CommandDesignerWidget() override;

    MenuItem& menuItem();
    void setNodes(const std::vector<VInfo_ptr>& nodes);
    const std::vector<VInfo_ptr>& selectedNodes();

public Q_SLOTS:
    void insertComponent(QListWidgetItem*);
    void on_commandLineEdit__textChanged();
    void on_saveNameLineEdit__textChanged();
    void on_addToContextMenuCb__stateChanged();
    void on_overwriteButton__clicked();
    void on_saveAsNewButton__clicked();
    void on_runButton__clicked();
    void on_saveOptionsButton__clicked();
    void on_editCommandButton__clicked();
    void on_useCommandButton__clicked();
    void on_duplicateCommandButton__clicked();
    void on_deleteCommandButton__clicked();
    void on_upButton__clicked();
    void on_downButton__clicked();
    void on_cancelSaveButton__clicked();
    void on_savedCommandsTable__cellClicked(int row, int column);
    void on_savedCommandsTable__cellDoubleClicked(int row, int column);
    void on_componentsList__itemEntered(QListWidgetItem* item);
    void on_componentsList__itemClicked(QListWidgetItem* item);
    void on_componentsList__itemDoubleClicked(QListWidgetItem* item);
    void on_nodeListLinkLabel__linkActivated(const QString& link);
    void on_nodeSelectionChanged();
    void on_tabWidget__currentChanged(int index);
    QPushButton* runButton() { return runButton_; }

private:
    enum TabIndexes { TAB_BUILD, TAB_NODES, TAB_SAVE };

    void initialiseComponentListDetails();
    void updateSaveButtonStatus();
    void addCommandToSavedList(CustomCommand* command, int row);
    void refreshSavedCommandList();
    void addClientCommandsToComponentList();
    void showCommandHelp(QListWidgetItem* item, bool showFullHelp);
    void initialiseCommandLine();
    void setNodeNumberLinkText(int numNodes);
    void setSavedCommandsButtonStatus();
    bool validSaveName(const std::string& name);
    void changeToTab(TabIndexes i);
    void selectRow(int row);
    void selectLastSavedCommand();
    void swapSavedCommands(int i1, int i2);
    void setSaveOptionsState(bool optionsVisible, bool saveOptionsButtonEnabled);

    // bool eventFilter(QObject* object, QEvent* event);

    bool currentCommandSaved_{false};
    bool haveSetUpDefaultCommandLine_{false};
    bool inCommandEditMode_{false};
    bool saveCommandsOnExit_{false};
    bool saveOptionsVisible_{false};
    std::vector<std::string> componentBlacklist_;
    MenuItem menuItem_;

    std::vector<VInfo_ptr> nodes_;
    NodeQueryResultModel nodeModel_;

    CtsCmdRegistry cmdRegistry_;
    boost::program_options::options_description* clientOptionsDescriptions_;
};

#endif
