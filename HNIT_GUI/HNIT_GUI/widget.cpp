#include "widget.h"
#include "ui_widget.h"
#include <QTimer>
#include <QDebug>
#include "math.h"

static char run_flag, run_mode;

//赛道变量
#define RUNWAY1
static char ranway[800][800];
const int begin_x = 500, begin_y = 180;

//控制变量
typedef struct flyControl
{
    int turn_decision;
    int base_x, base_y;
    double fix_x, fix_y;
    double speed;
    double yaw_sita, yaw_offset;
}flyCtrl;
static flyCtrl control;

//图片显示中间变量
const unsigned int view_length = 240;
static unsigned int view_size = view_length * view_length;
static unsigned int *view = (unsigned int*)malloc(view_size*sizeof(unsigned int));

//图片结果变量
static uchar image[80][80];

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    uchar ranway_model[80][80] = {0};
    #ifdef RUNWAY1
    for (int i = 20; i < 60; ++i)
    {
        ranway_model[i][20] = 1;
        ranway_model[i][59] = 1;
        ranway_model[20][i] = 1;
        ranway_model[35][i] = 1;
        ranway_model[59][i] = 1;
    }
    #endif
    #ifdef RANWAY2
    for(int i = 0; i < 10; i++)
    {
        ranway_model[40-5][61-15+i] = 1;
        ranway_model[40+5][61-15+i] = 1;
        ranway_model[40-5-i][40-5] = 1;
        ranway_model[40-5-i][40+5] = 1;
        ranway_model[40+5+i][40-5] = 1;
        ranway_model[40+5+i][40+5] = 1;
    }
    for(int i = 0; i < 11; i++)
    {
        ranway_model[35+i][35] = 1;
        ranway_model[35+i][55] = 1;
        ranway_model[25][35+i] = 1;
        ranway_model[55][35+i] = 1;
    }
    #endif
    for(int i = 10; i < 70; i++)
    {
        ranway_model[i][10] = 1;
        ranway_model[i][69] = 1;
        ranway_model[10][i] = 1;
        ranway_model[69][i] = 1;
    }

    //生成赛道
    for(int i = 0; i < 80; i++)
    {
        for(int j = 0; j < 80; j++)
        {
            for(int k = i*10; k < (i+1)*10; k++)
            {
                for(int l = j*10; l < (j+1)*10; l++)
                {
                    ranway[k][l] = ranway_model[i][j];
                }
            }
        }
    }

    ui->comboBox->addItem("  手 动");
    ui->comboBox->addItem(" 半自动");
    ui->comboBox->addItem(" 全自动");

    ui->vEdit->setValidator(new QIntValidator(0, 99, this));
    ui->wEdit->setValidator(new QIntValidator(0, 90, this));

    ui->pushButton->setEnabled(true);
    ui->pushButton_UP->setEnabled(false);
    ui->pushButton_DN->setEnabled(false);
    ui->pushButton_L->setEnabled(false);
    ui->pushButton_R->setEnabled(false);
    ui->pushButton_EL->setEnabled(false);
    ui->pushButton_ER->setEnabled(false);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()) , this, SLOT(updateNomal()));
}

Widget::~Widget()
{
    delete ui;
}

//启动：正常运行
//停止：“base坐标”回到原点，“绝对方向yaw_sita” 归零，“速度speed”归零
void Widget::on_pushButton_clicked()
{
    control.base_x = begin_x; control.base_y = begin_y;
    if(run_flag == 0)
    {
        run_flag = 1;
        ui->pushButton->setText("停止");
        if(ui->comboBox->currentIndex() != 2 )
        {
            ui->pushButton_UP->setEnabled(true);
            ui->pushButton_DN->setEnabled(true);
            ui->pushButton_EL->setEnabled(false);
            ui->pushButton_ER->setEnabled(true);
            if(ui->comboBox->currentIndex() != 1)
            {
                ui->wEdit->setText("18");
                ui->pushButton_L->setEnabled(true);
                ui->pushButton_R->setEnabled(true);
            }
        }
        else
        {
            ui->vEdit->setText("6");
        }

        if(ui->comboBox->currentIndex() != 0 )
            run_mode = 1;
        else
            run_mode = 0;

        ui->comboBox->setDisabled(true);
        timer->start(200);
    }
    else
    {
        run_flag = 0;
        timer->stop();
        ui->label->clear();

        memset(&control, 0 ,sizeof(control));

        ui->pushButton->setText("启动");
        ui->pushButton_UP->setEnabled(false);
        ui->pushButton_DN->setEnabled(false);
        ui->pushButton_L->setEnabled(false);
        ui->pushButton_R->setEnabled(false);
        ui->pushButton_EL->setEnabled(false);
        ui->pushButton_ER->setEnabled(false);
        ui->vEdit->setText("0");
        ui->wEdit->setText("0");
        ui->sEdit->setText("0");
        ui->comboBox->setDisabled(false);
    }
}

//加速：增大“速度speed”
void Widget::on_pushButton_UP_clicked()
{
    if(control.speed < 50)
    {
        control.speed+=2;
        QString s = QString::number(control.speed);
        ui->vEdit->setText(s);
    }
}

//减速：减小“速度speed”
void Widget::on_pushButton_DN_clicked()
{
    if(control.speed >= 2)
    {
        control.speed-=2;
        QString s = QString::number(control.speed);
        ui->vEdit->setText(s);
    }
}

//左转：减小“绝对方向yaw_sita”
void Widget::on_pushButton_R_clicked()
{
    control.yaw_offset = ui->wEdit->text().toDouble();
    control.yaw_sita -= control.yaw_offset;
}

//右转：增大“绝对方向yaw_sita”
void Widget::on_pushButton_L_clicked()
{
    control.yaw_offset = ui->wEdit->text().toDouble();
    control.yaw_sita += control.yaw_offset;
}

//正常运行：每0.1s动作一次
void Widget::updateNomal()
{
    getImage();
    doYourAIGO();
    goNomal();
    disImage();
}

//获取图像：二值、80*80的image
void Widget::getImage()
{
    uchar image_temp[180][180] = {0};
    uchar image_rotated[80][80] = {0};

    double sin_sita = sin(3.1416 * control.yaw_sita / 180.00);
    double cos_sita = cos(3.1416 * control.yaw_sita / 180.00);

    double xt = 0, yt = 0;
    //所在120*120地面旋转yaw_sita度，即为实际图像
    for(int i = 0; i < 120; i++)
    {
        for(int j = 0; j < 120; j++)
        {
            //旋转公式
            xt = ( (i - 60) * cos_sita - (j - 60) * sin_sita );
            yt = ( (i - 60) * sin_sita + (j - 60) * cos_sita );

            //结果四舍五入
            if(xt >= 0)
                xt += 0.5;
            else
                xt -= 0.5;
            if(yt >= 0)
                yt += 0.5;
            else
                yt -= 0.5;

            //image_temp中心坐标修正90, ranway修正40
            image_temp[(int)xt + 90][(int)yt + 90] = ranway[i+control.base_x-40][j+control.base_y-40];
        }
    }

    //在120*120图中选取中心的80*80作为结果图像
    for(int i = 0; i < 80;  i++)
    {
        for(int j = 0; j < 80; j++)
        {
            image[i][j] = 0;
            image_rotated[i][j] = image_temp[i+50][j+50];
        }
    }

    //膨胀操作，消除杂点
    for(int i = 1; i < 79; i++)
    {
        for(int j = 1; j < 79; j++)
        {
            image[i][j] = 0;
            if(image_rotated[i-1][j-1] || image_rotated[i-1][j] || image_rotated[i-1][j+1] ||
               image_rotated[i][j-1]   || image_rotated[i][j]   || image_rotated[i][j+1]   ||
               image_rotated[i+1][j-1] || image_rotated[i+1][j] || image_rotated[i+1][j+1]  )
            {
                image[i][j] = 1;
            }
        }
    }
}

//自定义循迹算法：image->循迹算法->更新“绝对方向yaw_sita”和“速度speed”
void Widget::doYourAIGO()
{
    int i = 0, j = 0;

    // ------------------------------- 边缘提取 ---------------------------------------
    uchar edge[80][80] = {0};
    for(i = 2; i < 78; i++)
    {
        for(j = 2; j < 78; j++)
        {
            if(image[i][j] != image[i][j+1] ||
               image[i][j] != image[i+1][j])
            {
                edge[i][j] = 3;
            }
        }
    }

    // ------------------------------ 寻找链码起点 --------------------------------------
    int begin_count = 0;
    int left_begin_x = 40, left_begin_y = 40, right_begin_x = 40, right_begin_y = 40;
    for(i = 60; i > 1; i--)
    {
        begin_count = 0;
        for(j = 77; j > 1; j--)
        {
            if(edge[i][j] == 3)
            {
                begin_count++;
                right_begin_y = j;
                break;
            }
        }
        for(j = 2; j < 78; j++)
        {
            if(edge[i][j] == 3)
            {
                begin_count++;
                left_begin_y = j;
                break;
            }
        }
        if(begin_count == 2 && (right_begin_y-left_begin_y) > 4)
        {
            left_begin_x = i;
            right_begin_x = i;
            break;
        }
    }
    edge[left_begin_x][left_begin_y] = 230;
    edge[right_begin_x][right_begin_y] = 230;

    //--------------------------------  线路决策 ----------------------------------------
    //目前用按钮进行决策
    int freeman_x = 0, freeman_y = 0;
    if(control.turn_decision == 0)
    {
        freeman_x = left_begin_x; freeman_y = left_begin_y;
    }
    else
    {
        freeman_x = right_begin_x; freeman_y = right_begin_y;
    }

    // ------------------------------ FreeMan链码 --------------------------------------
    int n = 0, last = -2, current = 0;
    int find[8][2] = {{-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1}};
    while(n < 100)
    {
        for(i = 0; i < 8; i++)
        {
            current = last + i;
            if(current < 0)
                current += 8;
            if(current > 7)
                current -= 8;

            if(edge[freeman_x+find[current][0]][freeman_y+find[current][1]] == 3)
            {
                freeman_x = freeman_x + find[current][0];
                freeman_y = freeman_y + find[current][1];
                edge[freeman_x][freeman_y] = 2;
                break;
            }
        }
        last = current - 2;
        n++;
    }
    edge[freeman_x][freeman_y] = 250;
    edge[freeman_x-1][freeman_y-1] = 250; edge[freeman_x-1][freeman_y+1] = 250;
    edge[freeman_x+1][freeman_y-1] = 250; edge[freeman_x+1][freeman_y+1] = 250;

    //--------------------------------  姿态调整  ---------------------------------------
    if(run_mode != 0)
    {       
        control.yaw_offset = atan((double)(freeman_y-40)/(double)(70-freeman_x))/3.1416*180;
        control.yaw_sita += control.yaw_offset;
        ui->wEdit->setText(QString::number(control.yaw_offset, 10, 1));
    }

    //------------------------------  图像信息加注  ---------------------------------------
    for(i = 0; i < 80; i++)
    {
        for(j = 0; j < 80; j++)
        {
            if(edge[i][j] != 0)
                image[i][j] = edge[i][j];
        }
    }
}

//正常飞行：从 “base坐标” 向 “yaw_sita角度” 飞行 “speed距离”
void Widget::goNomal()
{
    int temp = 0;
    double step = 0.00;
    double sin_sita = sin(3.1416 * control.yaw_sita / 180.00);
    double cos_sita = cos(3.1416 * control.yaw_sita / 180.00);

    control.speed = ui->vEdit->text().toInt();

    if(control.yaw_sita >= 360)
    {
        control.yaw_sita -= 360;
    }
    else if(control.yaw_sita < 0)
    {
        control.yaw_sita += 360;
    }
    ui->sEdit->setText(QString::number(control.yaw_sita, 10, 1));

    step = cos_sita*control.speed + control.fix_x;
    temp = (int)(step);
    if(temp >= 0)
        temp += 0.5;
    else
        temp -= 0.5;
    if(control.base_x-temp > 100 && control.base_x-temp < 700)
    {
        control.base_x -= temp;
        control.fix_x = step - (double)temp;
    }

    step = sin_sita*control.speed + control.fix_y;
    temp = (int)(step);
    if(temp >= 0)
        temp += 0.5;
    else
        temp -= 0.5;

    if(control.base_y+temp > 100 && control.base_y+temp < 700)
    {
        control.base_y += temp;
        control.fix_y = step - (double)temp;
    }
}

//显示图像：image
void Widget::disImage()
{
    //图像显示
    int n = 0;
    uchar *pa = NULL,*pb = NULL,*pc = NULL;
    for (int i = 0; i < 80; ++i)
    {
        for(int k = 0; k < 3; ++k)
        {
            for (int j = 0; j < 80; ++j)
            {
                pa = (uchar *)(view+n);
                pb = (uchar *)(view+n+1);
                pc = (uchar *)(view+n+2);

                view[n] = 0;view[n+1] = 0;view[n+2] = 0;
                if(image[i][j] == 1)
                {
                    pa[0] = 255; pa[1] = 255; pa[2] = 255;
                    pb[0] = 255; pb[1] = 255; pb[2] = 255;
                    pc[0] = 255; pc[1] = 255; pc[2] = 255;
                }
                else if(image[i][j] == 2)
                {
                    pa[0] = 0; pa[1] = 255; pa[2] = 0;
                    pb[0] = 0; pb[1] = 255; pb[2] = 0;
                    pc[0] = 0; pc[1] = 255; pc[2] = 0;
                }
                else if(image[i][j] == 3)
                {
                    pa[0] = 0; pa[1] = 255; pa[2] = 255;
                    pb[0] = 0; pb[1] = 255; pb[2] = 255;
                    pc[0] = 0; pc[1] = 255; pc[2] = 255;
                }
                else
                {
                    pa[0] = 0; pa[1] = 0; pa[2] = image[i][j];
                    pb[0] = 0; pb[1] = 0; pb[2] = image[i][j];
                    pc[0] = 0; pc[1] = 0; pc[2] = image[i][j];
                }
                n+=3;   
            }
        }
    }

    QByteArray image_space = QByteArray( (const char*)view,  view_size*4 );
    uchar*  temp_space = (uchar*)image_space.data();
    QImage my_image = QImage(temp_space, view_length, view_length, QImage::Format_RGB32);
    ui->label->setPixmap(QPixmap::fromImage(my_image));
}

void Widget::on_pushButton_EL_clicked()
{
    control.turn_decision = 0;
    ui->pushButton_EL->setEnabled(false);
    ui->pushButton_ER->setEnabled(true);
}

void Widget::on_pushButton_ER_clicked()
{
    control.turn_decision = 1;
    ui->pushButton_EL->setEnabled(true);
    ui->pushButton_ER->setEnabled(false);
}
