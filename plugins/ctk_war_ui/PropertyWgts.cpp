#include "PropertyWgts.h"
#include "UIActivator.h"
#include <zoo/Utils.h>
#include <zoo/ServiceLocator.h>
#include <UniversalGlobalServices.h>
#include <ctk_service/Win32Service.h>
#include <ctk_service/UIManagerService.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QDesktopServices>
#include <QStandardItemModel>
#include "ComPropertyBoard.h"
#include "ui_ComListWgt.h"
#include "ui_DoFPropertyWgt.h"
#include "ui_ModelPropertyWgt.h"
#include "ui_CameraPropertyWgt.h"
#include "ui_EarthPropertyWgt.h"
#include "ui_BehaviorPropertyWgt.h"
#include "ui_EnvirPropertyWgt.h"
#include "ui_MeshWgt.h"
#include "ui_MaterialWgt.h"
#include "ui_TextureWgt.h"
#include "ui_ConfigTableWgt.h"

// Qt5��������
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

#define COM_CAST(TYPE) ((TYPE*)_com)
#define BEHAVIOR_COMPONENT_NAME "Behavior"
#define FLOAT_TO_STRING(VAR) QString::number((float)VAR, 'f', 6).replace(QRegExp("(\\.){0,1}0+$"), "").replace(QRegExp("[.]$"), "")
#define DOUBLE_TO_STRING(VAR) QString::number((double)VAR, 'f', 8).replace(QRegExp("(\\.){0,1}0+$"), "").replace(QRegExp("[.]$"), "")

ComListWgt::ComListWgt(ComPropertyBoard* propBoard)
	: _ui(new Ui::ComListWgt)
	, _propBoard(propBoard)
	, _operateEnt(nullptr)
	, _addComponentBtn(false)
{
	QWidget::installEventFilter(this);
	setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint&~Qt::WindowMinimizeButtonHint);
	setFixedSize(300, 300);
	setVisible(false);
	_ui->setupUi(this);

	_ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	_ui->tableWidget->verticalHeader()->setDefaultSectionSize(20);
	_ui->tableWidget->verticalHeader()->setVisible(false);
	_ui->tableWidget->setShowGrid(true);
	_ui->tableWidget->setItemDelegate(new NoFocusDelegate());
	_ui->tableWidget->setColumnCount(2);
	_ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	_ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
	_ui->tableWidget->setColumnWidth(0, 138);
	_ui->tableWidget->setColumnWidth(1, 138);
	_ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("���")));
	_ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("ʵ��")));

	UIManagerService* pUIMgr = UIActivator::getService<UIManagerService>();
	connect(_ui->tableWidget, &QTableWidget::itemDoubleClicked, [this, pUIMgr](QTableWidgetItem *item)
	{
		if (item->flags().testFlag(Qt::ItemIsEditable))
			return;

		if (_operateEnt)
		{
			QString comName = _ui->tableWidget->item(item->row(), 0)->data(Qt::DisplayRole).toString();
			QString comImpl = _ui->tableWidget->item(item->row(), 1)->data(Qt::DisplayRole).toString();
			if (_addComponentBtn) // ������
			{
				zoo::Component* pCom = (comName == "Camera")
					? _operateEnt->addComponent<Camera, int>(comImpl.toStdString(), ZOOCMDWGT)
					: _operateEnt->addComponent(comName.toStdString(), comImpl.toStdString());
				if (pCom)
				{
					_propBoard->showCom(comName, pCom);
					if (pCom->getImp())
					{
						pCom->getImp()->awake();
						pCom->getEntity()->notifyComponents();
						pUIMgr->starWindowTitle();
					}
					else if (comName != BEHAVIOR_COMPONENT_NAME)
					{
						_operateEnt->removeComponent(pCom);
						QMessageBox::warning(this, tr("����"), comName + tr("���ʵ�ֲ����ڣ�"));
					}
					else
						pUIMgr->starWindowTitle();
				}
				else
				{
					QMessageBox::warning(this, tr("����"), comName + tr("��������ڣ�"));
				}
			}
			else // �Ƴ����
			{
				_operateEnt->removeComponent(comName.toStdString());
				_propBoard->hideCom(comName);
				pUIMgr->starWindowTitle();
			}

			hide();
		}
	});
}

ComListWgt::~ComListWgt()
{
	delete _ui;
}

void ComListWgt::refreshComList(zoo::Entity* ent, bool bAddComponentBtn, const QStringList& comlist)
{
	if (!ent)
		return;

	_operateEnt = ent;
	_addComponentBtn = bAddComponentBtn;
	QStringList strlist, addlist, removelist;

	for (int i = 0; i < comlist.size(); ++i)
	{
		if (ent->getComponent(comlist[i].toLocal8Bit().data()))
			removelist << comlist[i];
		else
			addlist << comlist[i];
	}

	if (bAddComponentBtn)
	{
		strlist = addlist;
		setWindowTitle(tr("������"));
	}
	else
	{
		strlist = removelist;
		setWindowTitle(tr("�Ƴ����"));
	}

	int rowCount = strlist.size();
	_ui->tableWidget->setRowCount(rowCount);

	for (int i = 0; i < rowCount; i++)
	{
		QTableWidgetItem* pItem = new QTableWidgetItem(strlist[i]);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
		_ui->tableWidget->setItem(i, 0, pItem);
		if (strlist[i] == BEHAVIOR_COMPONENT_NAME)
		{
			pItem = new QTableWidgetItem();
			pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
		}
		else
			pItem = new QTableWidgetItem(strlist[i] + "Impl");
		_ui->tableWidget->setItem(i, 1, pItem);
	}
}

bool ComListWgt::eventFilter(QObject *o, QEvent *e)
{
	if (QEvent::WindowDeactivate == e->type())
		hide();
	return false;
}

ConfigTableWgt::ConfigTableWgt(QWidget* parent, map<string, vector<double>>& keyValMap)
	: QWidget(parent)
	, _ui(new Ui::ConfigTableWgt)
{
	_ui->setupUi(this);
	_ui->tableWidget->setItemDelegate(new NoFocusDelegate());
	int rowCnt = keyValMap.size();
	if (rowCnt > 0)
	{
		_ui->tableWidget->setRowCount(rowCnt);
		auto it = keyValMap.begin();
		auto itEnd = keyValMap.end();
		for (int i = 0; it != itEnd; ++it, ++i)
		{
			vector<double>& val = it->second;
			int num = val.size();
			if (num == 0)
				continue;

			QWidget* pValWgt = new QWidget;
			QHBoxLayout* pLayout = new QHBoxLayout;
			pLayout->setMargin(0);
			pValWgt->setLayout(pLayout);
			_ui->tableWidget->setCellWidget(i, 1, pValWgt);

			for (int j = 0; j < num; ++j)
			{
				QDoubleSpinBox* spinBox = new QDoubleSpinBox;
				spinBox->setFrame(false);
				spinBox->setRange(-1000000, 1000000);
				spinBox->setDecimals(6);
				spinBox->setValue(val[j]);
				pLayout->addWidget(spinBox);

				connect(spinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this, &keyValMap, i, j](double value)
				{
					QString strVar = _ui->tableWidget->item(i, 0)->text();
					keyValMap[strVar.toStdString()][j] = value;
					emit keyValMapChanged();
				});
			}

			QTableWidgetItem* pItem = new QTableWidgetItem(QString::fromLocal8Bit(it->first.c_str()));
			pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
			pItem->setTextAlignment(Qt::AlignCenter);
			_ui->tableWidget->setItem(i, 0, pItem);
		}
	}
}

ConfigTableWgt::~ConfigTableWgt()
{
	delete _ui;
}

MeshWgt::MeshWgt(QWidget* parent, UIManagerService* uiMgr)
	: QWidget(parent)
	, _mesh(nullptr)
	, _ui(new Ui::MeshWgt)
	, _configWgt(nullptr)
{
	_ui->setupUi(this);
	fillMeshList();
	connect(_ui->toolButton_open, &QToolButton::clicked, [this, uiMgr]
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("��Դ�ļ���"), ZOO_DATA_ROOT_DIR.c_str(), tr("models(*.osg *.osgt *.ive *.flt);;images(*.png *.jpg *.rgba)"));
		if (!fileName.isEmpty())
		{
			_ui->lineEdit->setText(fileName);
			_mesh->_resourceFile = fileName.replace(QString::fromLocal8Bit(ZOO_DATA_ROOT_DIR.c_str()), tr("")).toLocal8Bit();
			_mesh->getParent()->dirtyBit().addState(Mesh::Changed_);
			emit meshChanged();
			if (uiMgr)
				uiMgr->starWindowTitle();
		}
	});
	connect(_ui->comboBox, static_cast<void(QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), [this, uiMgr](const QString& meshName)
	{
		if (!_mesh)
			return;

		if (ServiceLocator<MeshList>().getService()->switchMesh(meshName.toStdString(), _mesh))
		{
			emit meshChanged();
			if (uiMgr)
				uiMgr->starWindowTitle();
		}

		if (_configWgt)
		{
			_ui->verticalLayout->removeWidget(_configWgt);
			_configWgt->deleteLater();
			_configWgt = nullptr;
		}

		if (_mesh->_params.size() > 0)
		{
			_configWgt = new ConfigTableWgt(this, _mesh->_params);
			_ui->verticalLayout->addWidget(_configWgt);
			connect(_configWgt, &ConfigTableWgt::keyValMapChanged, [this, uiMgr]
			{
				_mesh->getParent()->dirtyBit().addState(Mesh::Changed_);
				emit meshChanged();
				if (uiMgr)
					uiMgr->starWindowTitle();
			});
		}

		_ui->lineEdit->setVisible(_mesh->_enableResource);
		_ui->toolButton_open->setVisible(_mesh->_enableResource);
	});
}

MeshWgt::~MeshWgt()
{
	delete _ui;
}

void MeshWgt::resetMesh(Mesh* mesh)
{
	fillMeshList();
	if (mesh)
	{
		_mesh = mesh;
		_ui->comboBox->setCurrentText(mesh->_currentUseMeshName.c_str());
		_ui->lineEdit->setVisible(_mesh->_enableResource);
		_ui->toolButton_open->setVisible(_mesh->_enableResource);

		if (mesh->_resourceFile == "")
			_ui->lineEdit->setText("");
		else
			_ui->lineEdit->setText(QString::fromLocal8Bit((ZOO_DATA_ROOT_DIR + mesh->_resourceFile).c_str()));
	}
}

void MeshWgt::fillMeshList()
{
	_mesh = nullptr;
	QStringList renderMesh;
	vector<string> meshList = ServiceLocator<MeshList>().getService()->getMeshList();
	auto it = meshList.begin();
	auto itEnd = meshList.end();
	for (; it != itEnd; ++it)
		renderMesh << (*it).c_str();

	_ui->comboBox->clear();
	_ui->comboBox->addItems(renderMesh);
}

TextureWgt::TextureWgt(QWidget* parent, Material* mat, int texUnitNum, UIManagerService* uiMgr)
	: QWidget(parent)
	, _ui(new Ui::TextureWgt)
{
	_ui->setupUi(this);
	_ui->label->setText(QString::fromLocal8Bit(mat->_textureFiles[texUnitNum].first.c_str()));
	string texPath = mat->_textureFiles[texUnitNum].second;
	if (texPath != "")
		_ui->lineEdit->setText(QString::fromLocal8Bit((ZOO_DATA_ROOT_DIR + texPath).c_str()));

	connect(_ui->toolButton_open, &QToolButton::clicked, [this, mat, texUnitNum, uiMgr]
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("�����ļ���"), ZOO_DATA_ROOT_DIR.c_str(), tr("texture(*.png *.jpeg *.jpg)"));
		if (!fileName.isEmpty())
		{
			_ui->lineEdit->setText(fileName);
			mat->_textureFiles[texUnitNum].second = fileName.replace(QString::fromLocal8Bit(ZOO_DATA_ROOT_DIR.c_str()), tr("")).toLocal8Bit();
			mat->getParent()->dirtyBit().addState(Material::Changed_);
			emit textureChanged();
			if (uiMgr)
				uiMgr->starWindowTitle();
		}
	});
	connect(_ui->toolButton_view, &QToolButton::clicked, [this]
	{
		QString filename = _ui->lineEdit->text();
		if (!filename.isEmpty())
			QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
	});
}

TextureWgt::~TextureWgt()
{
	delete _ui;
}

MaterialWgt::MaterialWgt(QWidget* parent, UIManagerService* uiMgr)
	: QWidget(parent)
	, _material(nullptr)
	, _ui(new Ui::MaterialWgt)
	, _uiMgr(uiMgr)
{
	_ui->setupUi(this);
	fillMaterialList();
	connect(_ui->comboBox, static_cast<void(QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), [this](const QString& matName)
	{
		if (!_material)
			return;

		if (ServiceLocator<MaterialList>().getService()->switchMaterial(matName.toStdString(), _material))
		{
			emit materialChanged();
			if (_uiMgr)
				_uiMgr->starWindowTitle();
		}

		int len = _wgtlist.length();
		for (int i = 0; i < len; ++i)
		{
			_ui->verticalLayout->removeWidget(_wgtlist[i]);
			_wgtlist[i]->deleteLater();
		}

		_wgtlist.clear();

		for (int i = 0; i < Material::TexUnitNum; ++i)
		{
			if (_material->_textureFiles[i].first != "")
			{
				TextureWgt* pWgt = new TextureWgt(this, _material, i, _uiMgr);
				_wgtlist.push_back(pWgt);
				_ui->verticalLayout->addWidget(pWgt);
				connect(pWgt, &TextureWgt::textureChanged, [this]
				{
					emit materialChanged();
				});
			}
		}

		if (_material->_uniforms.size() > 0)
		{
			ConfigTableWgt* pWgt = new ConfigTableWgt(this, _material->_uniforms);
			_wgtlist.push_back(pWgt);
			_ui->verticalLayout->addWidget(pWgt);
			connect(pWgt, &ConfigTableWgt::keyValMapChanged, [this]
			{
				_material->getParent()->dirtyBit().addState(Material::Uniform_);
				emit materialChanged();
				if (_uiMgr)
					_uiMgr->starWindowTitle();
			});
		}
	});
}

MaterialWgt::~MaterialWgt()
{
	delete _ui;
}

void MaterialWgt::resetMat(Material* material)
{
	fillMaterialList();
	if (material)
	{
		_material = material;
		_ui->comboBox->setCurrentText(material->_currentUseMatName.c_str());
	}
}

void MaterialWgt::fillMaterialList()
{
	_material = nullptr;
	QStringList renderPipeline;
	vector<string> matList = ServiceLocator<MaterialList>().getService()->getMaterialList();
	auto it = matList.begin();
	auto itEnd = matList.end();
	for (; it != itEnd; ++it)
		renderPipeline << (*it).c_str();

	_ui->comboBox->clear();
	_ui->comboBox->addItems(renderPipeline);
}

PropertyWgt::PropertyWgt(QWidget* parent)
	: QWidget(parent)
	, _com(nullptr)
{
	_uiMgr = UIActivator::getService<UIManagerService>();
}

void PropertyWgt::resetCom(zoo::Component* pCom)
{
	_com = pCom;
}

DoFPropertyWgt::DoFPropertyWgt(QWidget* parent)
	: PropertyWgt(parent)
	, _ui(new Ui::DoFPropertyWgt)
{
	_ui->setupUi(this);

	QDoubleValidator* pDoubleValidator = new QDoubleValidator;
	_ui->lineEdit_posx->setValidator(pDoubleValidator);
	_ui->lineEdit_posy->setValidator(pDoubleValidator);
	_ui->lineEdit_posz->setValidator(pDoubleValidator);
	_ui->lineEdit_rotx->setValidator(pDoubleValidator);
	_ui->lineEdit_roty->setValidator(pDoubleValidator);
	_ui->lineEdit_rotz->setValidator(pDoubleValidator);
	_ui->lineEdit_scalex->setValidator(pDoubleValidator);
	_ui->lineEdit_scaley->setValidator(pDoubleValidator);
	_ui->lineEdit_scalez->setValidator(pDoubleValidator);

	connect(_ui->lineEdit_posx, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		double tmp = _ui->lineEdit_posx->text().toDouble();
		if (!zoo::equals(COM_CAST(DoF)->_x, tmp))
		{
			COM_CAST(DoF)->_x = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_posy, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		double tmp = _ui->lineEdit_posy->text().toDouble();
		if (!zoo::equals(COM_CAST(DoF)->_y, tmp))
		{
			COM_CAST(DoF)->_y = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_posz, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		double tmp = _ui->lineEdit_posz->text().toDouble();
		if (!zoo::equals(COM_CAST(DoF)->_z, tmp))
		{
			COM_CAST(DoF)->_z = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_rotx, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		float tmp = _ui->lineEdit_rotx->text().toFloat();
		if (!zoo::equals(COM_CAST(DoF)->_pitch, tmp))
		{
			COM_CAST(DoF)->_pitch = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_roty, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		float tmp = _ui->lineEdit_roty->text().toFloat();
		if (!zoo::equals(COM_CAST(DoF)->_roll, tmp))
		{
			COM_CAST(DoF)->_roll = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_rotz, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		float tmp = _ui->lineEdit_rotz->text().toFloat();
		if (!zoo::equals(COM_CAST(DoF)->_heading, tmp))
		{
			COM_CAST(DoF)->_heading = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_scalex, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		float tmp = _ui->lineEdit_scalex->text().toFloat();
		if (!zoo::equals(COM_CAST(DoF)->_sx, tmp))
		{
			COM_CAST(DoF)->_sx = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_scaley, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		float tmp = _ui->lineEdit_scaley->text().toFloat();
		if (!zoo::equals(COM_CAST(DoF)->_sy, tmp))
		{
			COM_CAST(DoF)->_sy = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_scalez, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		float tmp = _ui->lineEdit_scalez->text().toFloat();
		if (!zoo::equals(COM_CAST(DoF)->_sz, tmp))
		{
			COM_CAST(DoF)->_sz = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->doubleSpinBox_lon, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val)
	{
		if (!zoo::equals(COM_CAST(DoF)->_x, val))
		{
			COM_CAST(DoF)->_x = val;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->doubleSpinBox_lat, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val)
	{
		if (!zoo::equals(COM_CAST(DoF)->_y, val))
		{
			COM_CAST(DoF)->_y = val;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->doubleSpinBox_height, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val)
	{
		if (!zoo::equals(COM_CAST(DoF)->_z, val))
		{
			COM_CAST(DoF)->_z = val;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->radioButton, static_cast<void(QRadioButton::*)(bool)>(&QRadioButton::toggled), [this](bool checked)
	{
		DoF* pDoF = COM_CAST(DoF);
		if (pDoF->_lonLatHeight == checked)
			return;

		_uiMgr->starWindowTitle();
		const_cast<bool&>(pDoF->_lonLatHeight) = checked;
		_ui->stackedWidget->setCurrentIndex(checked ? 1 : 0);
		if (checked)
		{
			double lon, lat, height;
			if (zoo::ServiceLocator<CoordTransformUtil>::getService()->convertXYZToLLH(pDoF->getEntity()->getSpawner()->getContext<CoordTransformUtil::Converter>(), pDoF->_x, pDoF->_y, pDoF->_z, lon, lat, height))
			{
				_ui->doubleSpinBox_lon->setValue(lon);
				_ui->doubleSpinBox_lat->setValue(lat);
				_ui->doubleSpinBox_height->setValue(height);
				pDoF->_x = lon;
				pDoF->_y = lat;
				pDoF->_z = height;
				_com->dirtyBit().addState(DoF::Dof_);
				_com->dirtyBit().addState(DoF::Parent_);
			}
		}
		else
		{
			double X, Y, Z;
			if (zoo::ServiceLocator<CoordTransformUtil>::getService()->convertLLHToXYZ(pDoF->getEntity()->getSpawner()->getContext<CoordTransformUtil::Converter>(), pDoF->_x, pDoF->_y, pDoF->_z, X, Y, Z))
			{
				_ui->lineEdit_posx->setText(DOUBLE_TO_STRING(X));
				_ui->lineEdit_posy->setText(DOUBLE_TO_STRING(Y));
				_ui->lineEdit_posz->setText(DOUBLE_TO_STRING(Z));
				pDoF->_x = X;
				pDoF->_y = Y;
				pDoF->_z = Z;
				_com->dirtyBit().addState(DoF::Dof_);
				_com->dirtyBit().addState(DoF::Parent_);
			}
		}
	});
}

DoFPropertyWgt::~DoFPropertyWgt()
{
	delete _ui;
}

void DoFPropertyWgt::resetCom(zoo::Component* pCom)
{
	PropertyWgt::resetCom(pCom);
	DoF* pDoF = COM_CAST(DoF);
	_ui->lineEdit_rotx->setText(FLOAT_TO_STRING(pDoF->_pitch));
	_ui->lineEdit_roty->setText(FLOAT_TO_STRING(pDoF->_roll));
	_ui->lineEdit_rotz->setText(FLOAT_TO_STRING(pDoF->_heading));
	_ui->lineEdit_scalex->setText(FLOAT_TO_STRING(pDoF->_sx));
	_ui->lineEdit_scaley->setText(FLOAT_TO_STRING(pDoF->_sy));
	_ui->lineEdit_scalez->setText(FLOAT_TO_STRING(pDoF->_sz));

	do 
	{
		DoF* pParent = pCom->getEntity()->getComponent<DoF>()->_parent;
		if (!pCom->getEntity()->getSpawner()->getComponent<Earth>() ||
			(pParent && !pParent->getEntity()->isSpawner()))
		{
			_ui->radioButton->hide();
			_ui->radioButton->setChecked(Qt::Unchecked);
		}
		else
		{
			_ui->radioButton->show();
			_ui->radioButton->setChecked(pDoF->_lonLatHeight ? Qt::Checked : Qt::Unchecked);
			if (pDoF->_lonLatHeight)
			{
				_ui->stackedWidget->setCurrentIndex(1);
				_ui->doubleSpinBox_lon->setValue(pDoF->_x);
				_ui->doubleSpinBox_lat->setValue(pDoF->_y);
				_ui->doubleSpinBox_height->setValue(pDoF->_z);
				break;
			}
		}

		_ui->stackedWidget->setCurrentIndex(0);
		_ui->lineEdit_posx->setText(DOUBLE_TO_STRING(pDoF->_x));
		_ui->lineEdit_posy->setText(DOUBLE_TO_STRING(pDoF->_y));
		_ui->lineEdit_posz->setText(DOUBLE_TO_STRING(pDoF->_z));

	} while (0);
}

ModelPropertyWgt::ModelPropertyWgt(QWidget* parent)
	: PropertyWgt(parent)
	, _ui(new Ui::ModelPropertyWgt)
{
	_ui->setupUi(this);
	_meshWgt = new MeshWgt(this, _uiMgr);
	_matWgt = new MaterialWgt(this, _uiMgr);
	_ui->verticalLayout->insertWidget(1, _meshWgt);
	_ui->verticalLayout->addWidget(_matWgt);

	connect(_ui->checkBox, &QCheckBox::stateChanged, [this](int boxState)
	{
		bool isShow = (boxState == Qt::Checked ? true : false);
		if (COM_CAST(Model)->_visible != isShow)
		{
			COM_CAST(Model)->_visible = isShow;
			_com->dirtyBit().addState(Model::Visible_);
			_uiMgr->starWindowTitle();
		}
	});
}

ModelPropertyWgt::~ModelPropertyWgt()
{
	delete _ui;
}

void ModelPropertyWgt::resetCom(zoo::Component* pCom)
{
	PropertyWgt::resetCom(pCom);
	Model* pModel = COM_CAST(Model);
	_ui->checkBox->setCheckState(pModel->_visible ? Qt::Checked : Qt::Unchecked);
	_meshWgt->resetMesh(&pModel->_mesh);
	_matWgt->resetMat(&pModel->_material);
}

CameraPropertyWgt::CameraPropertyWgt(QWidget* parent)
	: PropertyWgt(parent)
	, _ui(new Ui::CameraPropertyWgt)
{
	_ui->setupUi(this);
	
	QIntValidator*pValidator = new QIntValidator;
	pValidator->setRange(0, 100);
	_ui->lineEdit_left->setValidator(pValidator);
	_ui->lineEdit_right->setValidator(pValidator);
	_ui->lineEdit_bottom->setValidator(pValidator);
	_ui->lineEdit_top->setValidator(pValidator);

	pValidator->setRange(-1, 9999);
	_ui->lineEdit_id->setValidator(pValidator);
	_ui->lineEdit_breed->setValidator(pValidator);

	QStringList manipulatorTypes;
	manipulatorTypes << tr("Ĭ��") << tr("�ڵ������") << tr("�켣��") << tr("����") << tr("��ʻ")
		<< tr("���") << tr("��һ�˳��ӽ�") << tr("����") << tr("����");
	_ui->comboBox->addItems(manipulatorTypes);

	QStringList rtStrList;
	rtStrList << tr("��") << tr("����") << tr("��ɫ����") << tr("�������") << tr("ƽ����ʾ��");

	std::function<void(QComboBox*, Camera::PassIndex, unsigned int)> rtFunc = [this]
	(QComboBox* combox, Camera::PassIndex passInx, unsigned int passMask)
	{
		connect(combox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this, passInx, passMask](int index)
		{
			if (COM_CAST(Camera)->_passes[passInx]._rt != index)
			{
				COM_CAST(Camera)->_passes[passInx]._rt = (Camera::RenderTarget)index;
				_com->dirtyBit().addState(passMask);
				_uiMgr->starWindowTitle();
			}
		});
	};
	_ui->comboBox_pass1->addItems(rtStrList);
	rtFunc(_ui->comboBox_pass1, 0, Camera::Pass1_);
	_ui->comboBox_pass2->addItems(rtStrList);
	rtFunc(_ui->comboBox_pass2, 1, Camera::Pass2_);
	_ui->comboBox_pass3->addItems(rtStrList);
	rtFunc(_ui->comboBox_pass3, 2, Camera::Pass3_);
	_ui->comboBox_pass4->addItems(rtStrList);
	rtFunc(_ui->comboBox_pass4, 3, Camera::Pass4_);
	_ui->comboBox_pass5->addItems(rtStrList);
	rtFunc(_ui->comboBox_pass5, 4, Camera::Pass5_);
	_ui->comboBox_pass6->addItems(rtStrList);
	rtFunc(_ui->comboBox_pass6, 5, Camera::Pass6_);
	_ui->comboBox_pass7->addItems(rtStrList);
	rtFunc(_ui->comboBox_pass7, 6, Camera::Pass7_);
	_ui->comboBox_pass8->addItems(rtStrList);
	rtFunc(_ui->comboBox_pass8, 7, Camera::Pass8_);

	QString leftPercentSS = _ui->lineEdit_left->styleSheet();
	QString rightPercentSS = _ui->lineEdit_right->styleSheet();
	QString bottomPercentSS = _ui->lineEdit_bottom->styleSheet();
	QString topPercentSS = _ui->lineEdit_top->styleSheet();
	QString bgColorSS = _ui->pushButton_color->styleSheet();

	connect(_ui->lineEdit_left, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this, leftPercentSS]()
	{
		int leftPercent = _ui->lineEdit_left->text().toInt();
		int rightPercent = _ui->lineEdit_right->text().toInt();

		if (leftPercent >= rightPercent)
		{
			_ui->lineEdit_left->setStyleSheet("background-color:rgba(255,0,0,255)");
		}
		else
		{
			float tmp = leftPercent / 100.0f;
			if (!zoo::equals(COM_CAST(Camera)->_lRatio, tmp))
			{
				_ui->lineEdit_left->setStyleSheet(leftPercentSS);
				COM_CAST(Camera)->_lRatio = tmp;
				_com->dirtyBit().addState(Camera::Viewport_);
				_uiMgr->starWindowTitle();
			}
		}
	});
	connect(_ui->lineEdit_right, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this, rightPercentSS]()
	{
		int leftPercent = _ui->lineEdit_left->text().toInt();
		int rightPercent = _ui->lineEdit_right->text().toInt();

		if (leftPercent >= rightPercent)
		{
			_ui->lineEdit_right->setStyleSheet("background-color:rgba(255,0,0,255)");
		}
		else
		{
			float tmp = rightPercent / 100.0f;
			if (!zoo::equals(COM_CAST(Camera)->_rRatio, tmp))
			{
				_ui->lineEdit_right->setStyleSheet(rightPercentSS);
				COM_CAST(Camera)->_rRatio = tmp;
				_com->dirtyBit().addState(Camera::Viewport_);
				_uiMgr->starWindowTitle();
			}
		}
	});
	connect(_ui->lineEdit_bottom, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this, bottomPercentSS]()
	{
		int bottomPercent = _ui->lineEdit_bottom->text().toInt();
		int topPercent = _ui->lineEdit_top->text().toInt();

		if (bottomPercent >= topPercent)
		{
			_ui->lineEdit_bottom->setStyleSheet("background-color:rgba(255,0,0,255)");
		}
		else
		{
			float tmp = bottomPercent / 100.0f;
			if (!zoo::equals(COM_CAST(Camera)->_bRatio, tmp))
			{
				_ui->lineEdit_bottom->setStyleSheet(bottomPercentSS);
				COM_CAST(Camera)->_bRatio = tmp;
				_com->dirtyBit().addState(Camera::Viewport_);
				_uiMgr->starWindowTitle();
			}
		}
	});
	connect(_ui->lineEdit_top, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this, topPercentSS]()
	{
		int bottomPercent = _ui->lineEdit_bottom->text().toInt();
		int topPercent = _ui->lineEdit_top->text().toInt();

		if (bottomPercent >= topPercent)
		{
			_ui->lineEdit_top->setStyleSheet("background-color:rgba(255,0,0,255)");
		}
		else
		{
			float tmp = topPercent / 100.0f;
			if (!zoo::equals(COM_CAST(Camera)->_tRatio, tmp))
			{
				_ui->lineEdit_top->setStyleSheet(topPercentSS);
				COM_CAST(Camera)->_tRatio = tmp;
				_com->dirtyBit().addState(Camera::Viewport_);
				_uiMgr->starWindowTitle();
			}
		}
	});
	connect(_ui->pushButton_color, &QPushButton::clicked, [this, bgColorSS]()
	{
		Camera* pCam = COM_CAST(Camera);
		QColor initclr = QColor(pCam->_red, pCam->_green, pCam->_blue, pCam->_alpha);
		QColorDialog dlg(initclr, this);
		if (dlg.exec() == QDialog::Accepted)
		{
			QColor clr = dlg.currentColor();
			if (clr != initclr)
			{
				pCam->_red = clr.red();
				pCam->_green = clr.green();
				pCam->_blue = clr.blue();
				pCam->_alpha = clr.alpha();
				_com->dirtyBit().addState(Camera::Bgcolour_);
				_ui->pushButton_color->setStyleSheet(QString(tr("background-color:rgba(%1,%2,%3,%4)").arg(
					QString::number(pCam->_red), QString::number(pCam->_green),
					QString::number(pCam->_blue), QString::number(pCam->_alpha))));
				_uiMgr->starWindowTitle();
			}
		}
	});
	connect(_ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index)
	{
		if (COM_CAST(Camera)->_manipulatorKey != index)
		{
			COM_CAST(Camera)->_manipulatorKey = index;
			_com->dirtyBit().addState(Camera::Manipulator_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_id, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]()
	{
		int id = _ui->lineEdit_id->text().toInt();
		if (COM_CAST(Camera)->_trackEntID != id)
		{
			COM_CAST(Camera)->_trackEntID = id;
			_com->dirtyBit().addState(Camera::TrackEnt_);
			_uiMgr->starWindowTitle();
			Entity* ent = COM_CAST(Camera)->getEntity()->getSpawner()->gain(id, COM_CAST(Camera)->_trackEntBreed);
			if (ent)
				_ui->lineEdit_desc->setText(QString::fromLocal8Bit(ent->desc().c_str()));
			else
				_ui->lineEdit_desc->setText(tr(""));
		}
	});
	connect(_ui->lineEdit_breed, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]()
	{
		int breed = _ui->lineEdit_breed->text().toInt();
		if (COM_CAST(Camera)->_trackEntBreed != breed)
		{
			COM_CAST(Camera)->_trackEntBreed = breed;
			_com->dirtyBit().addState(Camera::TrackEnt_);
			_uiMgr->starWindowTitle();
			Entity* ent = COM_CAST(Camera)->getEntity()->getSpawner()->gain(COM_CAST(Camera)->_trackEntID, breed);
			if (ent)
				_ui->lineEdit_desc->setText(QString::fromLocal8Bit(ent->desc().c_str()));
			else
				_ui->lineEdit_desc->setText(tr(""));
		}
	});
}

CameraPropertyWgt::~CameraPropertyWgt()
{
	delete _ui;
}

void CameraPropertyWgt::resetCom(zoo::Component* pCom)
{
	PropertyWgt::resetCom(pCom);
	Camera* pCam = COM_CAST(Camera);
	_ui->lineEdit_viewid->setText(QString("%1").arg(pCam->_viewID));

	_ui->comboBox->setCurrentIndex(pCam->_manipulatorKey);
	_ui->lineEdit_left->setText(FLOAT_TO_STRING(pCam->_lRatio * 100.0f));
	_ui->lineEdit_right->setText(FLOAT_TO_STRING(pCam->_rRatio * 100.0f));
	_ui->lineEdit_bottom->setText(FLOAT_TO_STRING(pCam->_bRatio * 100.0f));
	_ui->lineEdit_top->setText(FLOAT_TO_STRING(pCam->_tRatio * 100.0f));
	_ui->pushButton_color->setStyleSheet(QString(tr("background-color:rgba(%1,%2,%3,%4)").arg(
		QString::number(pCam->_red), QString::number(pCam->_green),
		QString::number(pCam->_blue), QString::number(pCam->_alpha))));

	_ui->comboBox_pass1->setCurrentIndex(pCam->_passes[0]._rt);
	_ui->comboBox_pass2->setCurrentIndex(pCam->_passes[1]._rt);
	_ui->comboBox_pass3->setCurrentIndex(pCam->_passes[2]._rt);
	_ui->comboBox_pass4->setCurrentIndex(pCam->_passes[3]._rt);
	_ui->comboBox_pass5->setCurrentIndex(pCam->_passes[4]._rt);
	_ui->comboBox_pass6->setCurrentIndex(pCam->_passes[5]._rt);
	_ui->comboBox_pass7->setCurrentIndex(pCam->_passes[6]._rt);
	_ui->comboBox_pass8->setCurrentIndex(pCam->_passes[7]._rt);
}

EarthPropertyWgt::EarthPropertyWgt(QWidget* parent)
	: PropertyWgt(parent)
	, _ui(new Ui::EarthPropertyWgt)
{
	_ui->setupUi(this);
	connect(_ui->toolButton_open, &QToolButton::clicked, [this]
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("Earth�ļ���"), ZOO_DATA_ROOT_DIR.c_str(), tr("����(*.earth)"));
		if (!fileName.isEmpty())
		{
			_ui->lineEdit->setText(fileName);
			COM_CAST(Earth)->_earthFile = fileName.replace(QString::fromLocal8Bit(ZOO_DATA_ROOT_DIR.c_str()), tr("")).toLocal8Bit();
			_com->getImp()->awake();
			_com->getEntity()->notifyComponents();
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->toolButton_edit, &QToolButton::clicked, [this]
	{
		QString filename = _ui->lineEdit->text();
		if (!filename.isEmpty())
			QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
	});
	connect(_ui->toolButton_reset, &QToolButton::clicked, [this]
	{
		_com->getImp()->awake();
		_com->getEntity()->notifyComponents();
	});
	connect(_ui->checkBox_sun, &QCheckBox::stateChanged, [this]
	{
		bool isVisibility = _ui->checkBox_sun->checkState() == Qt::Unchecked ? false : true;
		if (COM_CAST(Earth)->_skyVisibility[Earth::sun_] != isVisibility)
		{
			COM_CAST(Earth)->_skyVisibility[Earth::sun_] = isVisibility;
			_com->dirtyBit().addState(Earth::SunVisible_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->checkBox_moon, &QCheckBox::stateChanged, [this]
	{
		bool isVisibility = _ui->checkBox_moon->checkState() == Qt::Unchecked ? false : true;
		if (COM_CAST(Earth)->_skyVisibility[Earth::moon_] != isVisibility)
		{
			COM_CAST(Earth)->_skyVisibility[Earth::moon_] = isVisibility;
			_com->dirtyBit().addState(Earth::MoonVisible_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->checkBox_star, &QCheckBox::stateChanged, [this]
	{
		bool isVisibility = _ui->checkBox_star->checkState() == Qt::Unchecked ? false : true;
		if (COM_CAST(Earth)->_skyVisibility[Earth::star_] != isVisibility)
		{
			COM_CAST(Earth)->_skyVisibility[Earth::star_] = isVisibility;
			_com->dirtyBit().addState(Earth::StarVisible_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->checkBox_nebula, &QCheckBox::stateChanged, [this]
	{
		bool isVisibility = _ui->checkBox_nebula->checkState() == Qt::Unchecked ? false : true;
		if (COM_CAST(Earth)->_skyVisibility[Earth::nebula_] != isVisibility)
		{
			COM_CAST(Earth)->_skyVisibility[Earth::nebula_] = isVisibility;
			_com->dirtyBit().addState(Earth::NebulaVisible_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->checkBox_atmosphere, &QCheckBox::stateChanged, [this]
	{
		bool isVisibility = _ui->checkBox_atmosphere->checkState() == Qt::Unchecked ? false : true;
		if (COM_CAST(Earth)->_skyVisibility[Earth::atmosphere_] != isVisibility)
		{
			COM_CAST(Earth)->_skyVisibility[Earth::atmosphere_] = isVisibility;
			_com->dirtyBit().addState(Earth::AtmosphereVisible_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->horizontalSlider, &QSlider::valueChanged, [this](int val)
	{
		float intensity = (float)val / _ui->horizontalSlider->maximum();
		if (!zoo::equals(COM_CAST(Earth)->_sunlightIntensity, intensity))
		{
			COM_CAST(Earth)->_sunlightIntensity = intensity;
			_com->dirtyBit().addState(Earth::SunlightIntensity_);
			_uiMgr->starWindowTitle();
		}
	});
}

EarthPropertyWgt::~EarthPropertyWgt()
{
	delete _ui;
}

void EarthPropertyWgt::resetCom(zoo::Component* pCom)
{
	PropertyWgt::resetCom(pCom);
	Earth* pEarth = COM_CAST(Earth);
	if (pEarth->_earthFile == "")
		_ui->lineEdit->setText("");
	else
		_ui->lineEdit->setText(QString::fromLocal8Bit((ZOO_DATA_ROOT_DIR + pEarth->_earthFile).c_str()));

	_ui->checkBox_sun->setCheckState(pEarth->_skyVisibility[Earth::sun_] ? Qt::Checked : Qt::Unchecked);
	_ui->checkBox_moon->setCheckState(pEarth->_skyVisibility[Earth::moon_] ? Qt::Checked : Qt::Unchecked);
	_ui->checkBox_star->setCheckState(pEarth->_skyVisibility[Earth::star_] ? Qt::Checked : Qt::Unchecked);
	_ui->checkBox_nebula->setCheckState(pEarth->_skyVisibility[Earth::nebula_] ? Qt::Checked : Qt::Unchecked);
	_ui->checkBox_atmosphere->setCheckState(pEarth->_skyVisibility[Earth::atmosphere_] ? Qt::Checked : Qt::Unchecked);

	int pos = pEarth->_sunlightIntensity * _ui->horizontalSlider->maximum();
	if (pos < _ui->horizontalSlider->minimum())
		pos = _ui->horizontalSlider->minimum();
	if (pos > _ui->horizontalSlider->maximum())
		pos = _ui->horizontalSlider->maximum();
	_ui->horizontalSlider->setSliderPosition(pos);
}

BehaviorPropertyWgt::BehaviorPropertyWgt(QWidget* parent)
	: PropertyWgt(parent)
	, _ui(new Ui::BehaviorPropertyWgt)
{
	_ui->setupUi(this);
	connect(_ui->toolButton_open, &QToolButton::clicked, [this]
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("�ű��ļ���"), ZOO_DATA_ROOT_DIR.c_str(), tr("�ű�(*.lua)"));
		if (!fileName.isEmpty())
		{
			_ui->lineEdit->setText(fileName);
			COM_CAST(Behavior)->_scriptFile = fileName.replace(QString::fromLocal8Bit(ZOO_DATA_ROOT_DIR.c_str()), tr("")).toLocal8Bit();
			COM_CAST(Behavior)->_scriptInited = false;
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->toolButton_edit, &QToolButton::clicked, [this]
	{
		QString filename = _ui->lineEdit->text();
		if (!filename.isEmpty())
			QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
	});
	connect(_ui->toolButton_reset, &QToolButton::clicked, [this]
	{
		COM_CAST(Behavior)->_scriptInited = false;
	});
}

BehaviorPropertyWgt::~BehaviorPropertyWgt()
{
	delete _ui;
}

void BehaviorPropertyWgt::resetCom(zoo::Component* pCom)
{
	PropertyWgt::resetCom(pCom);
	Behavior* pBehavior = COM_CAST(Behavior);
	if (pBehavior->_scriptFile == "")
		_ui->lineEdit->setText("");
	else
		_ui->lineEdit->setText(QString::fromLocal8Bit((ZOO_DATA_ROOT_DIR + pBehavior->_scriptFile).c_str()));
}

EnvirPropertyWgt::EnvirPropertyWgt(QWidget* parent)
	: PropertyWgt(parent)
	, _ui(new Ui::EnvirPropertyWgt)
{
	_ui->setupUi(this);
	for (int i = 0; i < 6; ++i)
		_skyboxTexWgts[i] = nullptr;

	QStringList weatherType;
	weatherType << tr("��") << tr("��") << tr("ѩ");
	_ui->comboBox->addItems(weatherType);
	connect(_ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index)
	{
		if (COM_CAST(Environment)->_type != index)
		{
			COM_CAST(Environment)->_type = (Environment::Weather)index;
			_com->dirtyBit().addState(Environment::Weather_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->horizontalSlider, &QSlider::valueChanged, [this](int val)
	{
		float intensity = (float)val / _ui->horizontalSlider->maximum();
		if (!zoo::equals(COM_CAST(Environment)->_intensity, intensity))
		{
			COM_CAST(Environment)->_intensity = intensity;
			_com->dirtyBit().addState(Environment::Weather_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->checkBox, &QCheckBox::stateChanged, [this](int boxState)
	{
		for (int i = 0; i < 6; ++i)
		{
			if (_skyboxTexWgts[i])
			{
				_ui->verticalLayout->removeWidget(_skyboxTexWgts[i]);
				_skyboxTexWgts[i]->deleteLater();
				_skyboxTexWgts[i] = nullptr;
			}
		}

		bool isShow = (boxState == Qt::Checked ? true : false);
		if (isShow)
		{
			for (int i = 0; i < 6; ++i)
			{
				QWidget* pWgt = new TextureWgt(this, &(COM_CAST(Environment)->_skyBox), i, _uiMgr);
				_skyboxTexWgts[i] = pWgt;
				_ui->verticalLayout->addWidget(pWgt);
			}

			if (COM_CAST(Environment)->_skyBox._currentUseMatName != ZOO_STRING(SkyBox))
			{
				COM_CAST(Environment)->_skyBox._currentUseMatName = ZOO_STRING(SkyBox);
				_com->dirtyBit().addState(Environment::SkyBox_);
				_uiMgr->starWindowTitle();
			}
		}
		else
		{
			if (COM_CAST(Environment)->_skyBox._currentUseMatName == ZOO_STRING(SkyBox))
			{
				COM_CAST(Environment)->_skyBox._currentUseMatName = "";
				_com->dirtyBit().addState(Environment::SkyBox_);
				_uiMgr->starWindowTitle();
			}
		}
	});
}

EnvirPropertyWgt::~EnvirPropertyWgt()
{
	delete _ui;
}

void EnvirPropertyWgt::resetCom(zoo::Component* pCom)
{
	PropertyWgt::resetCom(pCom);
	_ui->checkBox->setCheckState(COM_CAST(Environment)->_skyBox._currentUseMatName == ZOO_STRING(SkyBox) ? Qt::Checked : Qt::Unchecked);
	_ui->comboBox->setCurrentIndex(COM_CAST(Environment)->_type);
	int pos = COM_CAST(Environment)->_intensity * _ui->horizontalSlider->maximum();
	if (pos < _ui->horizontalSlider->minimum())
		pos = _ui->horizontalSlider->minimum();
	if (pos > _ui->horizontalSlider->maximum())
		pos = _ui->horizontalSlider->maximum();
	_ui->horizontalSlider->setSliderPosition(pos);
}
