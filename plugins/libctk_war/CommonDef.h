#pragma once

enum {ID = 0, DESCRIPTION, SCRIPT, POSX, POSY, POSZ, HEADING, PITCH, ROLL, SCALEX, SCALEY, SCALEZ, PARENT, VISIBLE, MODEL_FILE, SOUND_FILE, TRAJ_FILE};
enum {RATIO_LEFT = 3, RATIO_RIGHT, RATIO_BOTTOM, RATIO_TOP, TRACK_ENTITY};

static const char* TableField[] = { "id", "description", "script", "x", "y", "z", "heading", "pitch", "roll", "sx", "sy", "sz", "parent",
"visible", "model_file", "sound_file", "traj_file" };

static const char* WarReporter_TableField[] = { "id", "description", "script", "ratio_left", "ratio_right", "ratio_bottom", "ratio_top", "track_entity" };

static const char* Chs_TableField[] = { "���", "����", "�ű�", "λ��X", "λ��Y", "λ��Z", "��תH", "��תP", "��תR", "����X", "����Y", "����Z", "����ʵ��",
"�ɼ���", "ģ���ļ�", "��Ч�ļ�", "�켣�ļ�" };

static const char* Chs_WarReporter_TableField[] = { "���", "����", "�ű�", "�ӿ�������", "�ӿ��Ҳ����", "�ӿڵײ�����", "�ӿڶ�������", "����ʵ��" };
