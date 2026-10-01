#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <ui_mainwindow.h>
#include <rclcpp/rclcpp.hpp>
#include <turtlesim/msg/pose.hpp>
#include <std_srvs/srv/empty.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <move_to_point_action/action/move_to_point.hpp>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {class MainWindow; }
QT_END_NAMESPACE


class MainWindow: public QMainWindow {
    Q_OBJECT
public:
    MainWindow(rclcpp::Node::SharedPtr n,
        QWidget *parent = nullptr);
    ~MainWindow();
    //Для удобства используем локальные сокращения
    using MoveToPoint = move_to_point_action::action::MoveToPoint;
    using GoalHandleMoveToPoint =
        rclcpp_action::ClientGoalHandle<MoveToPoint>;
private slots:
    void on_pushButton_reset_clicked();

    void on_pushButton_sendToGoal_clicked();

    void  on_pushButton_start_clicked();

    void  on_pushButton_stop_clicked();

private:
    Ui::MainWindow *ui;
    rclcpp::Node::SharedPtr node;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr
        pose_sub;
    rclcpp::Client<std_srvs::srv::Empty>::SharedPtr
        reset_srv;
    rclcpp_action::Client<MoveToPoint>::SharedPtr action_client;

    GoalHandleMoveToPoint::SharedPtr curent_goal_handle;

    void pose_callback(const turtlesim::msg::Pose::SharedPtr msg);

   void send_goal();


   double progress_in_process = 0.0;

    //Функции обратного вызова для действия
    void goal_responce_callback(const GoalHandleMoveToPoint::SharedPtr &
                                    goal_handle);
    void feedback_callback(GoalHandleMoveToPoint::SharedPtr handle,
                           const std::shared_ptr<const MoveToPoint::Feedback> feedback);
    void result_callback(const GoalHandleMoveToPoint::WrappedResult &result);
};












#endif // MAINWINDOW_H
