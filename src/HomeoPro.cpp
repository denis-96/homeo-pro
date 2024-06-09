#include "HomeoPro.h"
#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <PatientCard.h>
#include <Repertorization.h>

HomePro::HomePro(QWidget *parent)
    : QMainWindow(parent)
{
    resize(900, 500);
    createActions();
    createMenu();
    createDialogs();

    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &HomePro::close);
    connect(tabWidget, &QTabWidget::currentChanged, this, &HomePro::updateActions);
    this->setCentralWidget(tabWidget);
    this->setWindowIcon(QIcon(":/resources/icons/main-icon.svg"));

    setFont(QFont("Segoe UI", 12));
}
HomePro::~HomePro() {}

void HomePro::createActions()
{
    newFileAction = new QAction("Новый файл", this);
    newFileAction->setIcon(QIcon(":/resources/icons/new-file.svg"));
    connect(newFileAction, &QAction::triggered, this, &HomePro::newFile);

    openAction = new QAction("Открыть", this);
    openAction->setIcon(QIcon(":/resources/icons/open-file.svg"));
    connect(openAction, &QAction::triggered, this, &HomePro::open);

    saveAction = new QAction("Сохранить", this);
    saveAction->setDisabled(true);
    saveAction->setIcon(QIcon(":/resources/icons/save-file.svg"));
    connect(saveAction, &QAction::triggered, this, &HomePro::save);

    saveAllAction = new QAction("Сохранить всё", this);
    saveAllAction->setDisabled(true);
    saveAllAction->setIcon(QIcon(":/resources/icons/save-file.svg"));
    connect(saveAllAction, &QAction::triggered, this, &HomePro::saveAll);

    closeAction = new QAction("Закрыть", this);
    closeAction->setDisabled(true);
    closeAction->setIcon(QIcon(":/resources/icons/close-file.svg"));
    connect(closeAction, &QAction::triggered, this, &HomePro::close);

    closeAllAction = new QAction("Закрыть всё", this);
    closeAllAction->setDisabled(true);
    closeAllAction->setIcon(QIcon(":/resources/icons/close-file.svg"));
    connect(closeAllAction, &QAction::triggered, this, &HomePro::closeAll);

    exitAction = new QAction("Выход", this);
    exitAction->setIcon(QIcon(":/resources/icons/exit.svg"));
    connect(exitAction, &QAction::triggered, this, &HomePro::close);
}

void HomePro::createMenu()
{
    auto menu = menuBar()->addMenu("Файл");
    menu->addAction(newFileAction);
    menu->addAction(openAction);
    menu->addSeparator();
    menu->addAction(saveAction);
    menu->addAction(saveAllAction);
    menu->addSeparator();
    menu->addAction(closeAction);
    menu->addAction(closeAllAction);
    menu->addSeparator();
    menu->addAction(exitAction);
}

void HomePro::createDialogs()
{
    newFileDialog = new QDialog(this);

    QLabel *label = new QLabel("Выберите тип файла", newFileDialog);
    QRadioButton *repOption = new QRadioButton("Реперторизация", newFileDialog);
    QRadioButton *pcardOption = new QRadioButton("Карточка пациента", newFileDialog);

    QLineEdit *titleEdit = new QLineEdit(newFileDialog);

    connect(repOption, &QRadioButton::toggled, titleEdit, [titleEdit, repOption] {
        if (repOption->isChecked())
            titleEdit->setText("Новая реперторизация");
    });
    connect(pcardOption, &QRadioButton::toggled, titleEdit, [titleEdit, pcardOption] {
        if (pcardOption->isChecked())
            titleEdit->setText("Новая карточка");
    });

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel,
                                                       newFileDialog);
    connect(buttonBox, &QDialogButtonBox::accepted, newFileDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, newFileDialog, &QDialog::reject);

    auto layout = new QVBoxLayout(newFileDialog);
    layout->addWidget(label);
    layout->addWidget(repOption);
    layout->addWidget(pcardOption);
    layout->addWidget(titleEdit);
    layout->addWidget(buttonBox);

    newFileDialog->setLayout(layout);

    repOption->setChecked(true);
}

void HomePro::addHomeoEntity(HomeoEntity *homeoEntity, const QString &title)
{
    int idx = tabWidget->addTab(homeoEntity, title);
    tabWidget->setCurrentIndex(idx);

    connect(homeoEntity, &HomeoEntity::changed, this, [this, idx] { updateTabTitle(idx, false); });
    connect(homeoEntity, &HomeoEntity::saved, this, [this, idx](const QString &fileName) {
        updateTabTitle(idx, true, getTitleFromFileName(fileName));
    });
}

bool HomePro::readFile(const QString &fileName)
{
    QString extension = fileName.split(".").back();
    HomeoEntity *homeoEntity = nullptr;

    if (extension == "rep") {
        homeoEntity = new Repertorization();
    } else if (extension == "pcard") {
        homeoEntity = new PatientCard();
    } else
        return false;

    if (!homeoEntity->load(fileName)) {
        delete homeoEntity;
        return false;
    }

    addHomeoEntity(homeoEntity, getTitleFromFileName(fileName));

    return true;
}

void HomePro::updateTabTitle(int tabIdx, bool isPageSaved, const QString &title)
{
    QString prevTitle = tabWidget->tabText(tabIdx);
    if (isPageSaved) {
        if (title.size())
            tabWidget->setTabText(tabIdx, title);
        else if (prevTitle.endsWith("*"))
            tabWidget->setTabText(tabIdx, prevTitle.chopped(1));
    }

    if (!isPageSaved && !prevTitle.endsWith("*"))
        tabWidget->setTabText(tabIdx, prevTitle + "*");
}

void HomePro::updateActions()
{
    saveAction->setEnabled(tabWidget->count());
    saveAllAction->setEnabled(tabWidget->count());
    closeAction->setEnabled(tabWidget->count());
    closeAllAction->setEnabled(tabWidget->count());
}

QString HomePro::getTitleFromFileName(const QString &fileName)
{
    return fileName.right(fileName.size() - fileName.lastIndexOf("/") - 1);
}

void HomePro::newFile()
{
    if (newFileDialog->exec() == QDialog::Accepted) {
        auto layout = newFileDialog->layout();
        auto title = dynamic_cast<QLineEdit *>(layout->itemAt(3)->widget())->text();

        HomeoEntity *homeoEntity = nullptr;
        if (dynamic_cast<QRadioButton *>(layout->itemAt(1)->widget())->isChecked()) {
            homeoEntity = new Repertorization();
        } else if (dynamic_cast<QRadioButton *>(layout->itemAt(2)->widget())->isChecked()) {
            homeoEntity = new PatientCard();
        } else
            return;

        addHomeoEntity(homeoEntity, title);
    }
}

void HomePro::open()
{
    QString fileName
        = QFileDialog::getOpenFileName(this,
                                       "Выберите реперторизацию",
                                       QString(),
                                       "Реперторизации (*.rep);;Карточки пациентов (*.pcard)");
    if (fileName.isEmpty())
        return;

    readFile(fileName);
}

void HomePro::save(int fileIdx)
{
    if (!tabWidget->count())
        return;
    int idx = fileIdx != -1 ? fileIdx : tabWidget->currentIndex();
    HomeoEntity *homeoEntity = dynamic_cast<HomeoEntity *>(tabWidget->widget(idx));
    QString fileFilter;

    if (Repertorization *rep = dynamic_cast<Repertorization *>(homeoEntity)) {
        fileFilter = "Реперторизации (*.rep)";
    } else if (PatientCard *pcard = dynamic_cast<PatientCard *>(homeoEntity)) {
        fileFilter = "Карточки пациентов (*.pcard)";
    } else
        return;

    QString fileName = homeoEntity->fileName();
    if (fileName.isEmpty()) {
        QString defaultName = tabWidget->tabText(idx);
        if (defaultName.endsWith("*"))
            defaultName.chop(1);
        fileName = QFileDialog::getSaveFileName(this, "Сохранить", defaultName, fileFilter);
        if (fileName.isEmpty())
            return;
    }

    if (!homeoEntity->save(fileName)) {
        QMessageBox::warning(this, "Предупреждение", "Ошибка при записи в файл");
    }
}

void HomePro::saveAll()
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        save(i);
    }
}

void HomePro::close(int fileIdx)
{
    if (!tabWidget->count())
        return;
    int idx = fileIdx != -1 ? fileIdx : tabWidget->currentIndex();
    if (tabWidget->tabText(idx).endsWith("*")) {
        QMessageBox::StandardButton answer = QMessageBox::question(this,
                                                                   "Несохранённые изменения",
                                                                   "Сохранить изменения?",
                                                                   QMessageBox::Yes
                                                                       | QMessageBox::No
                                                                       | QMessageBox::Cancel,
                                                                   QMessageBox::Yes);

        if (answer == QMessageBox::Cancel)
            return;
        if (answer == QMessageBox ::Yes)
            save(fileIdx);
    }

    tabWidget->removeTab(idx);
}

void HomePro::closeAll()
{
    while (tabWidget->count()) {
        close();
    }
}

void HomePro::exit()
{
    QMainWindow::close();
}

void HomePro::openWith()
{
    QStringList arguments = qApp->arguments();
    if (arguments.size() > 1 && QFile::exists(arguments.back())) {
        auto fileName = arguments.last();
        readFile(fileName);
    }
}

void HomePro::closeEvent(QCloseEvent *event)
{
    event->ignore();
    closeAll();
    event->accept();
}
