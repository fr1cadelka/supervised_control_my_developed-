#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(rclcpp::Node::SharedPtr n,
                       QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    node(n)
{
    ui->setupUi(this);
    //Подписчик на позицию черепахи
    pose_sub = node->create_subscription<turtlesim::msg::Pose>(
        "turtle1/pose",
        10,
        std::bind(&MainWindow::pose_callback,
                  this,
                  std::placeholders::_1));
    action_client =
        rclcpp_action::create_client<MoveToPoint>(node, "move_to_point");
    //Клиент сервиса для сброса черепахи
    reset_srv = node->create_client<std_srvs::srv::Empty>("/reset");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::pose_callback(
    const turtlesim::msg::Pose::SharedPtr msg)
{
    QString label_text =
        "  X: " + QString::number(msg->x) +
        "  Y: " + QString::number(msg->y) +
        "  Theta: " + QString::number(msg->theta);

    ui->label_pose->setText(label_text);
}

void MainWindow::on_pushButton_reset_clicked()
{
    auto request = std::make_shared<std_srvs::srv::Empty::Request>();
    reset_srv->async_send_request(request);
}

void MainWindow::goal_responce_callback(const GoalHandleMoveToPoint::SharedPtr &
                                            goal_handle) {
    //Проверяем, принята ли цель
    if(!goal_handle) {
        QMetaObject::invokeMethod(this, [this]() {
            //Если цель не принята
            ui->lineEdit_status->setText("Цель отклонена");
        });

    }
    else {
        QMetaObject::invokeMethod(this, [this]() {
            ui->lineEdit_status->setText("Сервер принял цель, начало движения...");
        });

    }
}
void MainWindow::feedback_callback(GoalHandleMoveToPoint::SharedPtr handle,
                                   const std::shared_ptr<const MoveToPoint::Feedback> feedback) {
    //Функция обрабатывает обратную связь от процесса
    QMetaObject::invokeMethod(this, [this, feedback]() {
        ui->lineEdit_status->setText("Distance: " + QString::number(feedback->progress));
        ui->progressBar_goal->setValue(100 - feedback->progress);
    });
    RCLCPP_INFO_STREAM(node->get_logger(), "Progress: "<<feedback->progress);

}
void MainWindow::result_callback(const GoalHandleMoveToPoint::WrappedResult &result) {
    //Обрабатывает результат действия
    QMetaObject::invokeMethod(this, [this, result]() {
        if(result.code == rclcpp_action::ResultCode::SUCCEEDED) {
            ui->lineEdit_status->setText("Цель достигнута");
        }
        else {
            ui->lineEdit_status->setText("Цель не достигнута");
        }
    });
}

void MainWindow::on_pushButton_sendToGoal_clicked()
{
    //Проверка связи с сервером действия
    if(!action_client->wait_for_action_server(std::chrono::seconds(2))) {
        ui->lineEdit_status->setText("Нет связи с сервером действия");
        return;
    }

    auto goal_msg = MoveToPoint::Goal();
    goal_msg.target_point.x = ui->doubleSpinBox_targetX->value();
    goal_msg.target_point.y = ui->doubleSpinBox_targetY->value();

    auto send_goal_options = rclcpp_action::Client<MoveToPoint>::SendGoalOptions();
    send_goal_options.goal_response_callback =
        std::bind(&MainWindow::goal_responce_callback, this, std::placeholders::_1);
    send_goal_options.feedback_callback =
        std::bind(&MainWindow::feedback_callback, this, std::placeholders::_1,
                  std::placeholders::_2);
    send_goal_options.result_callback =
        std::bind(&MainWindow::result_callback, this, std::placeholders::_1);

    action_client->async_send_goal(goal_msg, send_goal_options);

}












