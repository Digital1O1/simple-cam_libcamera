#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    MyWidget()
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        button = new QPushButton("Click me", this);
        label = new QLabel("Hello, world!", this);

        layout->addWidget(button);
        layout->addWidget(label);

        connect(button, &QPushButton::clicked, this, &MyWidget::onButtonClicked);
    }

private slots:
    void onButtonClicked()
    {
        label->setText("Button clicked!");
    }

private:
    QPushButton *button;
    QLabel *label;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MyWidget widget;
    widget.show();

    return app.exec();
}

#include "main.moc"
