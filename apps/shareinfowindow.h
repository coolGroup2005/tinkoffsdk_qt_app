#include "shareinfowindow.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

ShareInfoWindow::ShareInfoWindow(const QString &shareTicker, QWidget *parent)
    : QDialog(parent)
    , ticker(shareTicker)
{
    setWindowTitle("Share Information - " + ticker);

    QLabel *tickerLabel = new QLabel("Ticker: " + ticker, this);
    QLabel *nameLabel = new QLabel("Name: TBD", this);     // Placeholder for name
    QLabel *indexLabel = new QLabel("Index: TBD", this);   // Placeholder for index
    QLabel *ratingLabel = new QLabel("Rating: TBD", this); // Placeholder for rating

    QPushButton *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &ShareInfoWindow::close);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tickerLabel);
    layout->addWidget(nameLabel);
    layout->addWidget(indexLabel);
    layout->addWidget(ratingLabel);
    layout->addWidget(closeButton);

    setLayout(layout);
}
