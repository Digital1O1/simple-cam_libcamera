#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

/*
    `MyWidget inherits from `QWidget`

    Base class : QWidget
        - Base class provided by QT for all GUI elements
        - Provides fundamental features like:
            - Layout management
            - Event handling
            - Painting
    
    Derived Class : MyWidget
        - Adds custom functionality/members specific to this widget
*/
class MyWidget : public QWidget
{
    // Macro that enables QT meta-object feature like signals/slots
    Q_OBJECT

public:
    // Constructor for 'MyWidget' class
    MyWidget(QWidget *parent = nullptr);

// Slot function to handle button clicks
private slots:
    void handleButtonClick();

private:
    QLabel *label;
    QPushButton *button;
};

MyWidget::MyWidget(QWidget *parent) : QWidget(parent)
{
    label = new QLabel("Hello, World!", this);
    button = new QPushButton("Click Me", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(button);

    connect(button, &QPushButton::clicked, this, &MyWidget::handleButtonClick);
}

void MyWidget::handleButtonClick()
{
    label->setText("Button Clicked!");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MyWidget window;
    window.show();

    return app.exec();
}

// Needed since Q_OBJECT macro needs MOC processing to generate more code for the class
#include "main.moc"
