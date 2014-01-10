#include "pcloudwindow.h"
#include "ui_pcloudwindow.h"
#include "pcloudapp.h"
#include "common.h"

#include <QDesktopServices>
#include <QUrl>
#include<QDebug>


PCloudWindow::PCloudWindow(PCloudApp *a,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PCloudWindow)
{
    app=a;
    ui->setupUi(this);

    ui->listButtonsWidget->setViewMode(QListView::IconMode);
    ui->listButtonsWidget->setFlow(QListWidget::LeftToRight); //orientation
    ui->listButtonsWidget->setSpacing(12);

    ui->listButtonsWidget->setStyleSheet("background-color:transparent");
    //ui->listButtonsWidget->setWrapping(true); //
    //ui->listButtonsWidget->setSelectionRectVisible(false); //
    ui->listButtonsWidget->setMovement(QListView::Static); //not to move items with the mouse
    //temp
    ui->listButtonsWidget->setMinimumWidth(400);
    ui->listButtonsWidget->setMaximumHeight(85);
    ui->listButtonsWidget->setMinimumHeight(84); // precakva mi layouta
    // TEMP comment ui->listButtonsWidget->setFrameStyle(QFrame::Sunken); //hides frame


    //create Items for QListWidget
    new QListWidgetItem(QIcon(":/images/images/user.png"),tr("Account"),ui->listButtonsWidget);
    new QListWidgetItem(QIcon(":/images/images/user.png"),tr("Account"),ui->listButtonsWidget);
    new QListWidgetItem(QIcon(":/images/images/shares.png"),tr("Shares"),ui->listButtonsWidget);
    //QListWidgetItem *linksItem = new QListWidgetItem(QIcon(":/images/images/online.png"),tr("Links"),ui->listButtonsWidget);
    new QListWidgetItem(ui->listButtonsWidget); // temp hide page
    new QListWidgetItem(QIcon(":/images/images/settings.png"),tr("Settings"),ui->listButtonsWidget);
    new QListWidgetItem(QIcon(":/images/images/help.png"),tr("Help"),ui->listButtonsWidget);
    new QListWidgetItem(QIcon(":/images/images/info.png"),tr("About"),ui->listButtonsWidget);

    //ui->tabWidgetShares->setStyleSheet("background-color:none");


    //tova trqbva da e sys signal slot za case kogato ne sm lognata    manageAccountLoggedItem();
    fillAcountNotLoggedPage();
    fillAboutPage();
    fillHelpPage();
    settngsPage = new SettingsPage(this, app);

    // central widget - vertical layout - layoutStrtch: 1,5

    // indexes of Items in listWidget and StackWidget are the same
    connect(ui->listButtonsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));

    connect(ui->btnShareFolder,SIGNAL(clicked()), app, SLOT(shareFolder()));

    for(int i = 0; i < ui->pagesWidget->count(); i++)
        ui->pagesWidget->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);


    ui->verticalLayout_10->setAlignment(Qt::AlignCenter); //tab help

    setWindowIcon(QIcon(WINDOW_ICON));
    setWindowTitle("pCloud");

}

PCloudWindow::~PCloudWindow()
{
    delete ui;
}

void PCloudWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void PCloudWindow::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    int currentIndex = ui->listButtonsWidget->row(current);

    // auto resize
    for(int i = 0; i < ui->pagesWidget->count() && i != currentIndex; i++)
        ui->pagesWidget->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    ui->pagesWidget->widget(currentIndex)->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    updateGeometry();

    qDebug() << currentIndex;

    //layout()->activate();
    //setFixedSize(minimumSizeHint());

    ui->pagesWidget->setCurrentIndex(ui->listButtonsWidget->row(current)); // set page
    if (currentIndex == 4) //settings page
        settngsPage->initSettingsPage();
    //  if ( currentIndex == 2 )
    //    emit sharesPage->load(0);

}

void PCloudWindow::showpcloudWindow(int index)
{
    this->raise();
    this->activateWindow();
    this->showNormal();
    this->setWindowState(Qt::WindowActive);
    app->setActiveWindow(this);

    ui->listButtonsWidget->setCurrentRow(index);
    ui->pagesWidget->setCurrentIndex(index);
}

void PCloudWindow::setOnlineItems(bool online) // change pcloud window menu when is loggedin and loggedout
{
    if(online)
    {
        ui->listButtonsWidget->setRowHidden(0,true); //Accont - not logged
        ui->listButtonsWidget->setRowHidden(1,false); //Account - logged
        ui->listButtonsWidget->setRowHidden(2,false); //Shares
        ui->listButtonsWidget->setRowHidden(3,false); //Links
    }
    else
    {
        ui->listButtonsWidget->setRowHidden(0,false);
        ui->listButtonsWidget->setRowHidden(1,true);
        ui->listButtonsWidget->setRowHidden(2,true);
        ui->listButtonsWidget->setRowHidden(3,true);
    }
}

void PCloudWindow::setOnlinePages()
{
    sharesPage = new SharesPage(this, app);
    fillAccountLoggedPage();

}
void PCloudWindow::fillAcountNotLoggedPage()
{
    connect(ui->btnLogin, SIGNAL(clicked()),app, SLOT(showLogin()));
    connect(ui->btnRegstr, SIGNAL(clicked()), app, SLOT(showRegister()));
    connect(ui->btnExit, SIGNAL(clicked()), app, SLOT(doExit())); // to move slot in this class
    connect(ui->toolBtnContact, SIGNAL(clicked()), this, SLOT(contactUs()));
    ui->toolBtnContact->setStyleSheet("QToolButton{background-color:transparent;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");
}

void PCloudWindow::fillAboutPage()
{
    ui->label_versionVal->setText(APP_VERSION);
}

void PCloudWindow::fillHelpPage()
{
   // ui->tBtnOnlineHelp->setStyleSheet("QToolButton{background-color:transparent;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");
    //ui->tBtnOnlineTutorial->setStyleSheet("QToolButton{background-color:transparent;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");
    //ui->tBtnFeedback->setStyleSheet("QToolButton{background-color:transparent;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");
    connect(ui->tBtnOnlineHelp, SIGNAL(clicked()), this, SLOT(openOnlineHelp()));
    connect(ui->tBtnOnlineTutorial, SIGNAL(clicked()), this, SLOT(openOnlineTutorial()));
    connect(ui->tBtnFeedback, SIGNAL(clicked()), this, SLOT(sendFeedback()));
}

void PCloudWindow::fillAccountLoggedPage()
{       
    ui->pageAccntLogged->setMaximumWidth(700);
    ui->label_email->setText(app->username);
    if (app->isVerified)
        ui->checkBoxVerified->setCheckState(Qt::Checked);
    else
        ui->checkBoxVerified->setCheckState(Qt::Unchecked);
    ui->checkBoxVerified->setEnabled(false);

    ui->progressBar_space->setMinimum(0);
    ui->progressBar_space->setMaximum(100);
    ui->progressBar_space->setValue(app->freeSpacePercentage);
    ui->label_space->setText(app->freeSpace);

    ui->label_planVal->setText(app->plan);

    ui->toolBtnChangePass->setStyleSheet("QToolButton{background-color:transparent;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");  
    connect(ui->toolBtnOpenWeb, SIGNAL(clicked()), this, SLOT(openWebPage()));
    connect(ui->toolBtnChangePass, SIGNAL(clicked()), this, SLOT(changePass()));
    connect(ui->tbtnGetSpace, SIGNAL(clicked()), this, SLOT(upgradePlan()));
    ui->tbtnGetSpace->setVisible(false);
    connect(ui->tbtnOpenFolder, SIGNAL(clicked()),app,SLOT(openCloudDir()));
    connect(ui->tBtnExit, SIGNAL(clicked()), app, SLOT(doExit())); // to move in this class
    connect(ui->tBtnLogout, SIGNAL(clicked()), app, SLOT(logOut()));
    connect(ui->label, SIGNAL(linkActivated(QString)), this, SLOT(upgradePlan()));

}


//SLOTS
void PCloudWindow::openWebPage()
{
    QUrl url("https://my.pcloud.com/#page=filemanager");
    QDesktopServices::openUrl(url);
}

void PCloudWindow::changePass()
{
    QUrl url("https://my.pcloud.com/#page=changepassword");
    QDesktopServices::openUrl(url);
}

void PCloudWindow::upgradePlan()
{
    QUrl url("https://my.pcloud.com/#page=plans&authtoken="+app->authentication);
    QDesktopServices::openUrl(url);
}

void PCloudWindow::sendFeedback()
{
    QUrl url("https://my.pcloud.com/#page=contact"); //to check
    QDesktopServices::openUrl(url);
}

void PCloudWindow::openOnlineTutorial(){

    QUrl url ("https://my.pcloud.com/#page=contact&tutorial=show"); //to check
    QDesktopServices::openUrl(url);
}

void PCloudWindow::openOnlineHelp()
{
    QUrl url ("https://my.pcloud.com/#page=faq"); //to check
    QDesktopServices::openUrl(url);
}

void PCloudWindow::contactUs(){
    QUrl url ("https://my.pcloud.com/#page=contact");
    QDesktopServices::openUrl(url);
}



