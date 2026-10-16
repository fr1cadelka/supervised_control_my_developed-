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
            // Для активации кнопки после того как мы нажали
            ui->pushButton_start->setEnabled(true);
        });

    }
    else {
        // копируем цель в текущую позицию , для того чтобы робот запомнил где он


        QMetaObject::invokeMethod(this, [this,goal_handle]() {

        curent_goal_handle= goal_handle;
        ui->lineEdit_status->setText("Сервер принял цель, начало движения...");

        });

    }
}
void MainWindow::feedback_callback(GoalHandleMoveToPoint::SharedPtr handle,
                                   const std::shared_ptr<const MoveToPoint::Feedback> feedback) {
    //Лямбда Функция обрабатывает обратную связь от процесса
    QMetaObject::invokeMethod(this, [this, feedback]() {
        ui->lineEdit_status->setText("Distance: " + QString::number(feedback->progress));
        ui->progressBar_goal->setValue(100 - feedback->progress);




    });
    // RCLCPP_INFO_STREAM(node->get_logger(), "Progress: "<<feedback->progress);


    // if(feedback->progress<70 && feedback->progress>5){
    //     ui->pushButton_start->setEnabled(false);

    //     RCLCPP_INFO_STREAM(node->get_logger(), " диактивация кнопки старт ");
    // }

    // else{
    //      ui->pushButton_start->setEnabled(true);
    //     RCLCPP_INFO_STREAM(node->get_logger(), " активация кнопки старт ");
    // }

}
void MainWindow::result_callback(const GoalHandleMoveToPoint::WrappedResult &result) {
    QMetaObject::invokeMethod(this, [this, result]() {

        if (result.code == rclcpp_action::ResultCode::SUCCEEDED) {

            // Если миссия активна — отправляем следующую точку
            if (current_mission_index >= 0) {

                ++current_mission_index;

                if (current_mission_index < (int)mission_points.size()) {

                    double x = mission_points[current_mission_index].x();
                    double y = mission_points[current_mission_index].y();

                    ui->lineEdit_status->setText(
                        "Миссия: точка "
                        + QString::number(current_mission_index + 1)
                        + " из " + QString::number(mission_points.size()));

                    send_goal_to_point(x, y);
                    return;   // не сбрасываем handle и не включаем кнопки
                }
                else {
                    // Все точки пройдены
                    ui->lineEdit_status->setText("Миссия завершена");
                    current_mission_index = -1;
                    ui->pushButton_along_the_point->setEnabled(true);
                    ui->pushButton_start->setEnabled(true);
                }
            }
            else {
                ui->lineEdit_status->setText("Цель достигнута");
                ui->pushButton_start->setEnabled(true);
            }
        }
        else if (result.code == rclcpp_action::ResultCode::CANCELED) {
            ui->lineEdit_status->setText("Цель отменена");
            current_mission_index = -1;
            ui->pushButton_along_the_point->setEnabled(true);
            ui->pushButton_start->setEnabled(true);
        }
        else {
            ui->lineEdit_status->setText("Цель не достигнута");
            current_mission_index = -1;
            ui->pushButton_along_the_point->setEnabled(true);
            ui->pushButton_start->setEnabled(true);
        }

        curent_goal_handle.reset();
    });
}

void MainWindow::on_pushButton_sendToGoal_clicked()
{

    send_goal();

}


bool MainWindow::send_goal(){


    return send_goal_to_point(ui->doubleSpinBox_targetX->value(),
                              ui->doubleSpinBox_targetY->value());
    // //Проверка связи с сервером действия
    // if(!action_client->wait_for_action_server(std::chrono::seconds(2))) {
    //     ui->lineEdit_status->setText("Нет связи с сервером действия");
    //     return false ;
    // }



    // auto goal_msg = MoveToPoint::Goal();

    // auto progress_msg = MoveToPoint::Goal();
    // goal_msg.target_point.x = ui->doubleSpinBox_targetX->value();
    // goal_msg.target_point.y = ui->doubleSpinBox_targetY->value();

    // auto send_goal_options = rclcpp_action::Client<MoveToPoint>::SendGoalOptions();

    // send_goal_options.goal_response_callback =
    //     std::bind(&MainWindow::goal_responce_callback, this, std::placeholders::_1);

    // send_goal_options.feedback_callback =
    //     std::bind(&MainWindow::feedback_callback, this, std::placeholders::_1,
    //               std::placeholders::_2);

    // send_goal_options.result_callback =
    //     std::bind(&MainWindow::result_callback, this, std::placeholders::_1);

    // action_client->async_send_goal(goal_msg, send_goal_options);

    // return true;
}



void MainWindow::on_pushButton_start_clicked()
{

    // ui->pushButton_start->setEnabled(false);

    if(send_goal()){
        ui->pushButton_start->setEnabled(false);
    }


}


void MainWindow::on_pushButton_stop_clicked()
{
    if (curent_goal_handle) {
        action_client->async_cancel_goal(curent_goal_handle);
        curent_goal_handle.reset();
        ui->lineEdit_status->setText("Отмена движения робота");
    }
    else {
        ui->lineEdit_status->setText("Нет активной цели");
    }

    current_mission_index = -1;                              // ← миссия прервана
    ui->pushButton_along_the_point->setEnabled(true);        // ← кнопка активна
    ui->pushButton_start->setEnabled(true);
}

void MainWindow::on_pushButton_loafFile_with_point_clicked(){

    QString path= "/home/dmitry/Downloads/deepseek_text_20261015_ccf142.txt";
    QFile file(path);
    if(path.isEmpty()){
        return;
    }

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        ui->lineEdit_status->setText("Не удалось открыть файл");

        return;
    }

    mission_points.clear();

    QTextStream in(&file);

    int loaded=0;

    int skipped=0;


    while(!in.atEnd()){

        QString line = in.readLine().trimmed();

        if(line.isEmpty()){
            continue;

        }


        QStringList parts = line.split(QRegularExpression("\\s+"),Qt::SkipEmptyParts);

// парсим данные из файла
        if(parts.size()<2){

            ++skipped;
            continue;
        }

        bool ok_x= false;
        bool ok_y= false;

        double x= parts[0].toDouble(&ok_x);
        double y= parts[1].toDouble(&ok_y);

        if(!ok_x || !ok_y){

            ++skipped;
            continue;

        }


        mission_points.push_back(QPointF(x,y));
        ++loaded;

    }


    file.close();


    ui->lineEdit_status->setText(QString("Загруженно точек %1, пропущено: %2")
                                     .arg(loaded)
                                     .arg(skipped));





}


void MainWindow::on_pushButton_along_the_point_clicked()
{
    if (mission_points.empty()) {
        ui->lineEdit_status->setText("Нет загруженных точек");
        return;
    }

    current_mission_index = 0;

    double x = mission_points[0].x();
    double y = mission_points[0].y();

    if (!send_goal_to_point(x, y)) {
        current_mission_index = -1;
        return;
    }

    ui->pushButton_along_the_point->setEnabled(false);
    ui->pushButton_start->setEnabled(false);
    ui->lineEdit_status->setText("Миссия запущена: точка 1 из "
                                 + QString::number(mission_points.size()));
}

bool MainWindow:: send_goal_to_point(double x, double y){

    // Проверка связи с сервером действия
        if(!action_client->wait_for_action_server(std::chrono::seconds(2))) {
        ui->lineEdit_status->setText("Нет связи с сервером действия");
        return false ;
    }



    auto goal_msg = MoveToPoint::Goal();

    auto progress_msg = MoveToPoint::Goal();
    goal_msg.target_point.x = x;
    goal_msg.target_point.y = y;

    auto send_goal_options = rclcpp_action::Client<MoveToPoint>::SendGoalOptions();

    send_goal_options.goal_response_callback =
        std::bind(&MainWindow::goal_responce_callback, this, std::placeholders::_1);

    send_goal_options.feedback_callback =
        std::bind(&MainWindow::feedback_callback, this, std::placeholders::_1,
                  std::placeholders::_2);

    send_goal_options.result_callback =
        std::bind(&MainWindow::result_callback, this, std::placeholders::_1);

    action_client->async_send_goal(goal_msg, send_goal_options);

    return true;

}






