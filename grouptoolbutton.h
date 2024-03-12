#pragma once

#include <QtWidgets>
#include <QToolButton>
#include <QMouseEvent>
#include "ui_grouptoolbutton.h"

class GroupToolButton : public QToolButton
{
	Q_OBJECT

public:
	GroupToolButton(int index, QString m_groupName = "", QWidget* parent = Q_NULLPTR);
	virtual void mousePressEvent(QMouseEvent* event) override;
	QString getGroupName() const;
	int getIndex() const;

signals:
	void myClicked(QString groupName);

private:
	Ui::GroupToolButton ui;
	QString m_groupName;
	int m_index;
};
