#ifndef SHAREINFOWINDOW_H
#define SHAREINFOWINDOW_H

#include "shareinfowindow.h"
#include <QDialog>
#include "shareinfowindow.moc"

class ShareInfoWindow : public QDialog
{
    Q_OBJECT

public:
    ShareInfoWindow(const QString &shareTicker, QWidget *parent = nullptr);

private:
    QString ticker;
};

#endif // SHAREINFOWINDOW_H
