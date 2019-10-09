#include "mainwindow.h"
#include "Board.hpp"
#include "jsonReceive.hpp"
#include "Child.hpp"
#include "Monte_Carlo.hpp"
#include "Node.hpp"
#include "Agent-Action.hpp"
#include "Point.hpp"
#include <QtWidgets/QApplication>
#include <vector>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
