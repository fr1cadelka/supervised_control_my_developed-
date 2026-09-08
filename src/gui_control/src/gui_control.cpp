#include <QCoreApplication>
#include <thread>
#include "mainwindow.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    rclcpp::init(argc,argv);

    rclcpp::Node::SharedPtr node =
        std::make_shared<rclcpp::Node>("gui_node"); //имя узла

    MainWindow m(node);
    m.show();
    //Запуск отдельного потока через лямбда функцию
    std::thread ros_thread([node]() {
        rclcpp::spin(node);
    });

    int result = app.exec();
    rclcpp::shutdown();
    ros_thread.join();

    return result;
}
