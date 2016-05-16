#include "askuserforresourcesdialog.h"
#include "ui_askuserforresourcesdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include "every_cpp.h"


namespace BrowserAutomationStudioFramework
{
    AskUserForResourcesDialog::AskUserForResourcesDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::AskUserForResourcesDialog), Widget(0)
    {
        ui->setupUi(this);
        ClearContentWidget();
        Validator = 0;
        IsAdvanced = false;

        //qDebug()<<"Add";
        DatabaseButton = ui->buttonBox->addButton(tr("Data"),QDialogButtonBox::ResetRole);
        DatabaseButton->setIcon(QIcon(":/engine/images/database.png"));
        connect(DatabaseButton, SIGNAL(clicked()),this,SIGNAL(ShowDatabase()));
    }

    void AskUserForResourcesDialog::HideDatabaseButton()
    {
        DatabaseButton->setVisible(false);
    }

    void AskUserForResourcesDialog::RemoveLanguageChooser()
    {
        ui->pushButton_4->setVisible(false);
    }

    QPushButton * AskUserForResourcesDialog::AddResetButton()
    {
        QPushButton *ResetButton = ui->buttonBox->addButton("",QDialogButtonBox::ResetRole);
        connect(ResetButton, SIGNAL(clicked()),this,SIGNAL(Reset()));
        connect(ResetButton, SIGNAL(clicked()),this,SLOT(reject()));
        return ResetButton;
    }

    void AskUserForResourcesDialog::SetValidator(IValidator *Validator)
    {
        this->Validator = Validator;
    }

    AskUserForResourcesDialog::~AskUserForResourcesDialog()
    {
        delete ui;
    }

    void AskUserForResourcesDialog::ClearContentWidget()
    {
        if(Widget)
        {
           Widget->deleteLater();
        }

        Widget = new QWidget(ui->scrollArea);
        ui->scrollArea->setWidget(Widget);
    }

    QWidget *AskUserForResourcesDialog::GetContentWidget()
    {
        return Widget;
    }

    void AskUserForResourcesDialog::on_pushButton_clicked()
    {
        IsAdvanced = false;
        ui->pushButton_5->setText(tr("Show Advanced"));
        emit Default();
    }

    void AskUserForResourcesDialog::on_pushButton_2_clicked()
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Project"), "", tr("Project Files (*.xml);;All Files (*.*)"));
        if(fileName.length()>0)
        {
            emit Save(fileName);
        }
    }

    void AskUserForResourcesDialog::on_pushButton_3_clicked()
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open Project"), "", tr("Project Files (*.xml);;All Files (*.*)"));

        if(fileName.length()>0)
        {
            ui->pushButton_5->setText(tr("Show Advanced"));
            emit ShowLanguage();
            emit Load(fileName);
        }
    }

    void AskUserForResourcesDialog::on_pushButton_4_clicked()
    {
        IsAdvanced = false;
        ui->pushButton_5->setText(tr("Show Advanced"));
        emit ShowLanguage();
    }

    void AskUserForResourcesDialog::changeEvent(QEvent *e)
    {
        QDialog::changeEvent(e);
        switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
        }
    }

    void AskUserForResourcesDialog::done(int r)
    {
        if(!Validator || r == QDialog::Rejected)
        {
            QDialog::done(r);
            return;
        }
        bool IsAdvancedOld = IsAdvanced;
        IsAdvanced = true;
        ui->pushButton_5->setText(tr("Hide Advanced"));

        emit ShowAdvanced();
        bool validate = Validator->Validate();

        IsAdvanced = IsAdvancedOld;
        if(IsAdvanced)
        {
            ui->pushButton_5->setText(tr("Hide Advanced"));
            emit ShowAdvanced();
        }else
        {
            ui->pushButton_5->setText(tr("Show Advanced"));
            emit HideAdvanced();
        }

        if(!validate)
        {

            QMessageBox msgBox;
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowIcon(QIcon(":/engine/images/Logo.png"));
            msgBox.setText(tr("Data is not valid. Are you sure, that you want to continue?"));
            if(msgBox.exec() == QMessageBox::Ok)
            {
                QDialog::done(r);
                return;
            }
        }else
        {

            QDialog::done(r);
            return;
        }

    }
    void AskUserForResourcesDialog::on_pushButton_5_clicked()
    {
        IsAdvanced = !IsAdvanced;
        if(IsAdvanced)
        {
            ui->pushButton_5->setText(tr("Hide Advanced"));
            emit ShowAdvanced();
        }else
        {
            ui->pushButton_5->setText(tr("Show Advanced"));
            emit HideAdvanced();
        }
    }

    void AskUserForResourcesDialog::on_pushButton_6_clicked()
    {
        emit ShowLogLocation();
    }

}












