#pragma once

#include <QObject>
#include <ctk_service/war/WarService.h>

class WarServiceImpl : public QObject, public WarService
{
	Q_OBJECT
	Q_INTERFACES(WarService)
public:
	WarServiceImpl();
	~WarServiceImpl();
	bool openScene(int id);
	void closeScene();
	void saveScene();
	void startSimulation();
	void stopSimulation();
};
