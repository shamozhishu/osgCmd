#include "MenuToolButtons.h"
#include "ArmyListWgt.h"
#include <ctk_service/zoocmd_ui/UIManagerService.h>
#include <QMenu>
#include <QMessageBox>

// Qt5��������
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

void MenuToolButtons::create(UIManagerService* service, EMenu menutype, QObject* receiver, bool hasToolButton)
{
	QMenu* pSubMenu = nullptr;
	switch (menutype)
	{
	case scene_:
	{
		pSubMenu = new QMenu("����");
		QAction* act = new QAction(QIcon(QPixmap(":/images/Resources/images/open.png")), "��");
		QObject::connect(act, SIGNAL(triggered()), receiver, SLOT(onOpen()));
		pSubMenu->addAction(act);
		act = new QAction(QIcon(QPixmap(":/images/Resources/images/close.png")), "�ر�");
		QObject::connect(act, SIGNAL(triggered()), receiver, SLOT(onClose()));
		pSubMenu->addAction(act);
		act = new QAction(QIcon(QPixmap(":/images/Resources/images/save.png")), "����");
		act->setEnabled(false);
		QObject::connect(act, SIGNAL(triggered()), receiver, SLOT(onSave()));
		pSubMenu->addAction(act);

		QObject::connect(qobject_cast<ArmyListWgt*>(receiver), &ArmyListWgt::sceneOpendSucceed, [act]
		{
			act->setEnabled(true);
		});
		QObject::connect(qobject_cast<ArmyListWgt*>(receiver), &ArmyListWgt::sceneOpendFailed, [act]
		{
			act->setEnabled(false);
		});
	}
	break;
	case simulation_:
	{
		pSubMenu = new QMenu("����");
		QAction* actStart = new QAction(QIcon(QPixmap(":/images/Resources/images/start.png")), "��ʼ");
		actStart->setEnabled(false);
		actStart->setCheckable(true);
		pSubMenu->addAction(actStart);
		QAction* actPause = new QAction(QIcon(QPixmap(":/images/Resources/images/pause.png")), "��ͣ");
		actPause->setEnabled(false);
		actPause->setCheckable(true);
		pSubMenu->addAction(actPause);

		QObject::connect(qobject_cast<ArmyListWgt*>(receiver), &ArmyListWgt::sceneOpendSucceed, [actStart, actPause]
		{
			actStart->setEnabled(true);
			actPause->setEnabled(false);

			actStart->setText("��ʼ");
			actStart->setIcon(QIcon(QPixmap(":/images/Resources/images/start.png")));
			actStart->setChecked(false);
			actPause->setChecked(false);
		});

		QObject::connect(qobject_cast<ArmyListWgt*>(receiver), &ArmyListWgt::sceneOpendFailed, [actStart, actPause]
		{
			actStart->setEnabled(false);
			actPause->setEnabled(false);

			actStart->setText("��ʼ");
			actStart->setIcon(QIcon(QPixmap(":/images/Resources/images/start.png")));
			actStart->setChecked(false);
			actPause->setChecked(false);
		});

		QObject::connect(qobject_cast<ArmyListWgt*>(receiver), &ArmyListWgt::simStarted, [actStart, actPause]
		{
			actStart->setText("��ֹ");
			actStart->setIcon(QIcon(QPixmap(":/images/Resources/images/stop.png")));
			actPause->setEnabled(true);
		});

		QObject::connect(qobject_cast<ArmyListWgt*>(receiver), &ArmyListWgt::simStoped, [actStart, actPause]
		{
			actStart->setText("��ʼ");
			actStart->setIcon(QIcon(QPixmap(":/images/Resources/images/start.png")));
			actPause->setChecked(false);
			actPause->setEnabled(false);
		});

		QObject::connect(actStart, SIGNAL(triggered(bool)), receiver, SLOT(onSim(bool)));
		QObject::connect(actPause, SIGNAL(triggered(bool)), receiver, SLOT(onPaused(bool)));
	}
	break;
	case help_:
	{
		pSubMenu = new QMenu("����");
		QAction* act = new QAction(QIcon(QPixmap(":/images/Resources/images/about.png")), "����");
		pSubMenu->addAction(act);
		QObject::connect(act, &QAction::triggered, [service]
		{
			QMessageBox::about(dynamic_cast<QWidget*>(service), "����", "ս���༭�� - �汾1.0");
		});
	}
	break;
	default:
		break;
	}

	if (pSubMenu)
		service->addMenu(QString("MENU_%1").arg(menutype), pSubMenu, hasToolButton);
}

void MenuToolButtons::destroy(UIManagerService* service, EMenu menutype)
{
	service->removeMenu(QString("MENU_%1").arg(menutype));
}
