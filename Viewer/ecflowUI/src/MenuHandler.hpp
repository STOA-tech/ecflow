//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef MENUHANDLER_HPP_
#define MENUHANDLER_HPP_

#include <map>
#include <vector>

#include <QIcon>
#include <QList>
#include <QPoint>
#include <QString>

#include "NodeExpression.hpp"
#include "VInfo.hpp"

class QMenu;
class QAction;
class QShortcut;
class QWidget;
class Node;
class BaseNodeCondition;

// -------------------------
// MenuItem
// A single item in a menu
// -------------------------

class MenuItem {
public:
    explicit MenuItem(const std::string& name);
    ~MenuItem();

    void setCommand(const std::string& command);
    // bool compatibleWithNode(VInfo_ptr nodeInfo);
    // void addValidType(std::string type);
    // void addValidState(std::string type);
    void setHandler(const std::string& handler);
    void setViews(const std::vector<std::string>& views) { views_ = views; }
    void setQuestion(const std::string& question) { question_ = question; }
    void setQuestionControl(const std::string& questionControl) { questionControl_ = questionControl; }
    void setPanelPopupControl(const std::string& c) { panelPopupControl_ = c; }
    void setWarning(const std::string& warning) { warning_ = warning; }
    void setIcon(const std::string& icon);
    void setStatustip(const std::string& statustip) { statustip_ = statustip; }
    void setShortcut(const std::string& shortcut) { shortcut_ = shortcut; }
    void setHidden(bool b) { hidden_ = b; }
    void setMultiSelect(bool b) { multiSelect_ = b; }
    void setAsSubMenu() { isSubMenu_ = true; }
    void setVisibleCondition(BaseNodeCondition* cond) { visibleCondition_ = cond; }
    void setEnabledCondition(BaseNodeCondition* cond) { enabledCondition_ = cond; }
    void setQuestionCondition(BaseNodeCondition* cond) { questionCondition_ = cond; }
    void setCustom(bool b) { isCustom_ = b; }
    BaseNodeCondition* visibleCondition() const { return visibleCondition_; }
    BaseNodeCondition* enabledCondition() const { return enabledCondition_; }
    BaseNodeCondition* questionCondition() const { return questionCondition_; }
    bool shouldAskQuestion(const std::vector<VInfo_ptr>& nodes) const;
    bool isSubMenu() const { return isSubMenu_; }
    bool isDivider() const { return isDivider_; }
    bool isCustom() const { return isCustom_; }
    const std::string& name() const { return name_; }
    const std::string& handler() const { return handler_; }
    const std::string& shortcut() const { return shortcut_; }
    bool isValidView(const std::string&) const;
    const std::string& command() const { return command_; }
    const std::string& question() const { return question_; }
    const std::string& questionControl() const { return questionControl_; }
    const std::string& panelPopupControl() const { return panelPopupControl_; }
    const std::string& warning() const { return warning_; }
    bool hidden() const { return hidden_; }
    bool multiSelect() const { return multiSelect_; }
    bool isValidFor(const std::vector<VInfo_ptr>& nodes, bool allowHidden = false) const;
    int id() const { return id_; }
    QAction* createAction(QWidget* parent);
    QShortcut* createShortcut(QWidget* parent, const std::string& view);

private:
    // No copy allowed
    MenuItem(const MenuItem&)            = delete;
    MenuItem& operator=(const MenuItem&) = delete;

    // bool isNodeTypeValidForMenuItem(NodeType type);

    std::string name_;
    int id_;
    std::string tooltip_;
    std::string command_;
    std::string statustip_;
    std::string question_;
    std::string questionControl_;
    std::string defaultAnswer_;
    std::string panelPopupControl_;
    std::string warning_;
    std::string handler_;
    std::string shortcut_;
    std::vector<std::string> views_;
    bool hidden_;
    bool multiSelect_; // multiple selecttion

    // std::vector<NodeType>      validNodeTypes_;
    // std::vector<DState::State> validNodeStates_;

    BaseNodeCondition* visibleCondition_;
    BaseNodeCondition* enabledCondition_;
    BaseNodeCondition* questionCondition_;

    bool isSubMenu_;
    bool isDivider_;
    bool isCustom_;

    QIcon icon_;

    static int idCnt_;
};

// -------------------------------------------------------------
// Menu
// Contains all the possible items for a given menu. These will
// be filtered at run-time according to the state of
// the given item which has been clicked.
// -------------------------------------------------------------

class Menu {
public:
    explicit Menu(const std::string& name);
    ~Menu();
    QString exec(std::vector<Node*> nodes);
    const std::string& name() const { return name_; }
    void addItemToFixedList(MenuItem* item) { itemsFixed_.push_back(item); }
    void addItemToCustomList(MenuItem* item) { itemsCustom_.push_back(item); }
    void clearFixedList() { itemsFixed_.clear(); }
    QMenu* generateMenu(std::vector<VInfo_ptr> nodes,
                        QWidget* parent,
                        QMenu* parentMenu,
                        const std::string& view,
                        QList<QAction*>&);
    const std::vector<MenuItem*>& itemsFixed() const { return itemsFixed_; }
    const std::vector<MenuItem*>& items() const { return itemsCombined_; }

private:
    void buildMenuTitle(std::vector<VInfo_ptr> nodes, QMenu* qmenu);

    std::string name_;
    std::vector<MenuItem*> itemsFixed_;
    std::vector<MenuItem*> itemsCustom_;
    std::vector<MenuItem*> itemsCombined_; // items from config file plus custom commands
};

// --------------------------------------------------------------
// MenuHandler
// Responsible for creating menus (read from configuration files)
// and generating 'actual' (i.e. context-dependent filtered)
// menus at run-time.
// --------------------------------------------------------------

class MenuHandler {
public:
    MenuHandler();

    // Menu *createMenu(QString &name);
    static bool readMenuConfigFile(const std::string& configFile);
    static MenuItem* invokeMenu(const std::string& menuName,
                                std::vector<VInfo_ptr> nodes,
                                QPoint pos,
                                QWidget* parent,
                                const std::string& view);
    static bool addItemToMenu(MenuItem* item, const std::string& menuName);
    static Menu* findMenu(const std::string& name);
    static MenuItem* newItem(const std::string& name);
    static void addMenu(Menu* menu) { menus_.push_back(menu); }
    static void interceptCommandsThatNeedConfirmation(MenuItem* item);
    static void refreshCustomMenuCommands();
    static void setupShortcut(QObject* receiver, QWidget* w, const std::string& view);
    static MenuItem* findItemById(int id);

private:
    typedef std::map<std::string, std::string> ConfirmationMap;
    static MenuItem* findItem(QAction*);
    static ConfirmationMap& getCommandsThatRequireConfirmation();

    static std::vector<Menu*> menus_;
    static ConfirmationMap commandsWhichRequireConfirmation_;
    static TrueNodeCondition trueCond_;
    static FalseNodeCondition falseCond_;
};

#endif
