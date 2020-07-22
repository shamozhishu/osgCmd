#include "MenuToolButtons.h"
#include <ctk_service/zoocmd_ui/UIManagerService.h>
#include <QMenu>

// Qt5��������
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

void MenuToolButtons::create(UIManagerService* service, EMenu menutype, const QObject* receiver)
{
	QMenu* pSubMenu = nullptr;
	switch (menutype)
	{
	case file_:
	{
		pSubMenu = new QMenu("�ļ�");
		QAction* act = new QAction(QIcon(QPixmap(":/images/Resources/images/open.png")), "��");
		QObject::connect(act, SIGNAL(triggered()), receiver, SLOT(onOpen()));
		pSubMenu->addAction(act);
		act = new QAction(QIcon(QPixmap(":/images/Resources/images/save.png")), "����");
		QObject::connect(act, SIGNAL(triggered()), receiver, SLOT(onSave()));
		pSubMenu->addAction(act);
	}
	break;
	case edit_:
	{
		pSubMenu = new QMenu("�༭");
		QIcon icon;
		icon.addPixmap(QPixmap(":/images/Resources/images/start.png"), QIcon::Normal, QIcon::Off);
		icon.addPixmap(QPixmap(":/images/Resources/images/stop.png"), QIcon::Normal, QIcon::On);
		QAction* act = new QAction(icon, "����");
		QObject::connect(act, SIGNAL(triggered(bool)), receiver, SLOT(onSim(bool)));
		pSubMenu->addAction(act);
	}
	break;
	default:
		break;
	}

	if (pSubMenu)
		service->addMenu(pSubMenu->title(), pSubMenu);
}

void MenuToolButtons::destroy(UIManagerService* service, EMenu menutype)
{
	switch (menutype)
	{
	case file_:
		service->removeMenu("�ļ�");
		break;
	case edit_:
		service->removeMenu("�༭");
		break;
	default:
		break;
	}
}
