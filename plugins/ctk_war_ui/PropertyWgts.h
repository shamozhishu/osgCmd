#ifndef __PROPERTY_WGTS_H__
#define __PROPERTY_WGTS_H__

#include <QWidget>
#include <QHBoxLayout>
#include <component/WarComponents.h>

namespace Ui
{
	class ComListWgt;
	class MeshWgt;
	class ShaderWgt;
	class TextureWgt;
	class MaterialWgt;
	class ConfigTableWgt;
	class DoFPropertyWgt;
	class ModelPropertyWgt;
	class CameraPropertyWgt;
	class EarthPropertyWgt;
	class BehaviorPropertyWgt;
	class EnvirPropertyWgt;
}

class UIManagerService;
class ComPropertyBoard;
class ComListWgt : public QWidget
{
public:
	ComListWgt(ComPropertyBoard* propBoard);
	~ComListWgt();
	void refreshComList(zoo::Entity* ent, bool bAddComponentBtn, const QStringList& comlist);
protected:
	bool eventFilter(QObject *o, QEvent *e);
private:
	Ui::ComListWgt* _ui;
	bool _addComponentBtn;
	zoo::Entity* _operateEnt;
	ComPropertyBoard* _propBoard;
};

class ConfigTableWgt : public QWidget
{
	Q_OBJECT
public:
	ConfigTableWgt(QWidget* parent, map<string, vector<double>>& keyValMap);
	~ConfigTableWgt();

signals:
	void keyValMapChanged();

private:
	Ui::ConfigTableWgt* _ui;
};

class MeshWgt : public QWidget
{
	Q_OBJECT
public:
	MeshWgt(QWidget* parent, UIManagerService* uiMgr);
	~MeshWgt();
	void resetMesh(Mesh* mesh);

signals:
	void meshChanged();
private:
	void fillMeshList();

private:
	Mesh* _mesh;
	Ui::MeshWgt* _ui;
	ConfigTableWgt* _configWgt;
};

class TextureWgt : public QWidget
{
	Q_OBJECT
public:
	TextureWgt(QWidget* parent, Material* mat, int texUnitNum, UIManagerService* uiMgr);
	~TextureWgt();

signals:
	void textureChanged();

private:
	Ui::TextureWgt* _ui;
};

class MaterialWgt : public QWidget
{
	Q_OBJECT
public:
	MaterialWgt(QWidget* parent, UIManagerService* uiMgr);
	~MaterialWgt();
	void resetMat(Material* material);

signals:
	void materialChanged();
private:
	void fillMaterialList();

private:
	Material* _material;
	Ui::MaterialWgt* _ui;
	UIManagerService* _uiMgr;
	QVector<QWidget*> _wgtlist;
};

class PropertyWgt : public QWidget
{
public:
	PropertyWgt(QWidget* parent);
	virtual void resetCom(zoo::Component* pCom);

protected:
	zoo::Component* _com;
	UIManagerService* _uiMgr;
};

class DoFPropertyWgt : public PropertyWgt
{
public:
	DoFPropertyWgt(QWidget* parent);
	~DoFPropertyWgt();
	void resetCom(zoo::Component* pCom);

private:
	Ui::DoFPropertyWgt* _ui;
};

class ModelPropertyWgt : public PropertyWgt
{
public:
	ModelPropertyWgt(QWidget* parent);
	~ModelPropertyWgt();
	void resetCom(zoo::Component* pCom);

private:
	MeshWgt* _meshWgt;
	MaterialWgt* _matWgt;
	Ui::ModelPropertyWgt* _ui;
};

class CameraPropertyWgt : public PropertyWgt
{
public:
	CameraPropertyWgt(QWidget* parent);
	~CameraPropertyWgt();
	void resetCom(zoo::Component* pCom);

private:
	Ui::CameraPropertyWgt* _ui;
};

class EarthPropertyWgt : public PropertyWgt
{
public:
	EarthPropertyWgt(QWidget* parent);
	~EarthPropertyWgt();
	void resetCom(zoo::Component* pCom);

private:
	Ui::EarthPropertyWgt* _ui;
};

class BehaviorPropertyWgt : public PropertyWgt
{
public:
	BehaviorPropertyWgt(QWidget* parent);
	~BehaviorPropertyWgt();
	void resetCom(zoo::Component* pCom);

private:
	Ui::BehaviorPropertyWgt* _ui;
};

class EnvirPropertyWgt : public PropertyWgt
{
public:
	EnvirPropertyWgt(QWidget* parent);
	~EnvirPropertyWgt();
	void resetCom(zoo::Component* pCom);

private:
	Ui::EnvirPropertyWgt* _ui;
	QWidget* _skyboxTexWgts[6];
};

#endif // __PROPERTY_WGTS_H__
